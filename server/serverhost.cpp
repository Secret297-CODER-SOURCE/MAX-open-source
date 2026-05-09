#include "serverhost.h"
#include <QAbstractSocket>
#include <QString>
ServerHost::ServerHost() {
    clientCount = 0;
    capacity = 1;
    clientSocket = new QTcpSocket*[capacity];
    for (int i = 0; i < capacity; i++)
        clientSocket[i] = nullptr;
}


ServerHost::~ServerHost() {
    for (int i = 0; i < clientCount; i++) {
        if (clientSocket[i]) {
            clientSocket[i]->disconnectFromHost();
        }
    }
    delete[] clientSocket;
}

char** ServerHost::getIpAddressList(){
    char** listOfAddr = new char*[clientCount + 1];
    QByteArray arr;
    for (int i = 0; i < clientCount; i++){
        arr = clientSocket[i]->peerAddress().toString().toUtf8();
        listOfAddr[i] = new char[arr.size() + 1];
        strcpy(listOfAddr[i], arr.constData());
    }
    listOfAddr[clientCount] = nullptr;
    return listOfAddr;
}

void ServerHost::start(){
    disconnect(this, &QTcpServer::newConnection, this, &ServerHost::newConnections);
    connect(this, &QTcpServer::newConnection, this, &ServerHost::newConnections);
    this->listen(QHostAddress::Any, 8888);
}

void ServerHost::stop(){
    for(int i = 0; i < clientCount; i++){
        if(clientSocket[i]){
            clientSocket[i]->disconnectFromHost();
            clientSocket[i] = nullptr;
        }
    }
    clientCount = 0;
    this->close();
}


void ServerHost::resizeArray() {
    int newCapacity = capacity * 2;
    QTcpSocket** newArray = new QTcpSocket*[newCapacity];

    for (int i = 0; i < newCapacity; i++)
        newArray[i] = nullptr;

    for (int i = 0; i < clientCount; i++)
        newArray[i] = clientSocket[i];

    delete[] clientSocket;
    clientSocket = newArray;
    capacity = newCapacity;
}

int ServerHost::addClient(QTcpSocket* socket) {
    if (clientCount >= capacity)
        resizeArray();

    clientSocket[clientCount] = socket;
    return clientCount++;
}

void ServerHost::newConnections() {
    while (this->hasPendingConnections()) {
        QTcpSocket* socket = this->nextPendingConnection();
        socket->setParent(nullptr);

        addClient(socket);
        emit clientListChanged();
        qDebug() << "connect +1";

        QThread* thread = new QThread;
        socket->moveToThread(thread);

        connect(socket, &QTcpSocket::readyRead, [socket]() {
            QByteArray data = socket->readAll();
            qDebug() << "Received" << data;
            QMetaObject::invokeMethod(socket, [socket]() {
                socket->write("Hello");
            }, Qt::QueuedConnection);
        });

        connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
        connect(socket, &QTcpSocket::disconnected, thread, &QThread::quit);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        thread->start();
    }
}
