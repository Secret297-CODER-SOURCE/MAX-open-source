#include "serverhost.h"
#include <QAbstractSocket>
#include <QString>
ServerHost::ServerHost() {
    server = new QTcpServer(this);
    clientCount = 0;
    capacity = 1;
    clientSocket = new QTcpSocket*[capacity];
    for (int i = 0; i < capacity; i++)
        clientSocket[i] = nullptr;
}


ServerHost::~ServerHost() {
    for (int i = 0; i < clientCount; i++) {
        if (clientSocket[i])
            delete clientSocket[i];
    }
    delete[] clientSocket;
}

char** ServerHost::getIpAddressList(){
    char** listOfAddr = new char*[clientCount];
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
    connect(server, &QTcpServer::newConnection, this, &ServerHost::newConnections);
    server->listen(QHostAddress::Any, 8888);
}

void ServerHost::stop(){
    for(int i = 0; i < clientCount; i++){
        if(clientSocket[i]){
            clientSocket[i]->disconnectFromHost();
            clientSocket[i] = nullptr;
        }
    }
    server->close();
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
    while (server->hasPendingConnections()) {
        QTcpSocket* socket = server->nextPendingConnection();
        QThread* thread = new QThread;

        socket->moveToThread(thread);
        qDebug()<<"connect +1";

        connect(socket, &QTcpSocket::readyRead, [socket]() {
            QByteArray data = socket->readAll();
            qDebug() << "Received" << data;
            socket->write("Hello");
        });

        connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
        connect(socket, &QTcpSocket::disconnected, thread, &QThread::quit);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        thread->start();
    }
}
