#include "serverhost.h"
#include <QAbstractSocket>
#include <QString>

ServerHost::ServerHost() {
    clientCount = 0;
    capacity = 1;
    clients = new ClientInfo[capacity];
}

ServerHost::~ServerHost() {
    for (int i = 0; i < clientCount; i++) {
        if (clients[i].socket)
            clients[i].socket->disconnectFromHost();
    }
    delete[] clients;
}

void ServerHost::start() {
    disconnect(this, &QTcpServer::newConnection, this, &ServerHost::newConnections);
    connect(this, &QTcpServer::newConnection, this, &ServerHost::newConnections);
    this->listen(QHostAddress::Any, 8888);
}

void ServerHost::stop() {
    for (int i = 0; i < clientCount; i++) {
        if (clients[i].socket) {
            clients[i].socket->disconnectFromHost();
            clients[i].socket = nullptr;
        }
    }
    clientCount = 0;
    this->close();
}

int ServerHost::generateId() {
    quint64 id = QRandomGenerator::global()->bounded(10000000,99999999);
    return int(id);
}

ClientInfo* ServerHost::findClientById(const int& id) {
    for (int i = 0; i < clientCount; i++) {
        if (clients[i].id == id)
            return &clients[i];
    }
    return nullptr;
}

void ServerHost::resizeArray() {
    int newCapacity = capacity * 2;
    ClientInfo* newClients = new ClientInfo[newCapacity];

    for (int i = 0; i < clientCount; i++)
        newClients[i] = clients[i];

    delete[] clients;
    clients = newClients;
    capacity = newCapacity;
}

int ServerHost::addClient(QTcpSocket* socket) {
    if (clientCount >= capacity)
        resizeArray();

    int id = generateId();

    while (findClientById(id))
        id = generateId();

    clients[clientCount].id = id;
    clients[clientCount].username = "";
    clients[clientCount].socket = socket;

    return clientCount++;
}

void ServerHost::removeClient(QTcpSocket* socket) {
    for (int i = 0; i < clientCount; i++) {
        if (clients[i].socket == socket) {
            for (int j = i; j < clientCount - 1; j++)
                clients[j] = clients[j + 1];

            clients[clientCount - 1].socket = nullptr;
            clients[clientCount - 1].id = 0;
            clients[clientCount - 1].username = "";

            clientCount--;

            return;
        }
    }
}

char** ServerHost::getClientList(){
    char** listOfAddr = new char*[clientCount + 1];
    for (int i = 0; i < clientCount; i++) {
        QString entry =  QString::number(clients[i].id) + " | " + clients[i].username + " | " +clients[i].socket->peerAddress().toString();
        QByteArray arr = entry.toUtf8();
        listOfAddr[i] = new char[arr.size() + 1];
        strcpy(listOfAddr[i], arr.constData());
    }
    listOfAddr[clientCount] = nullptr;
    return listOfAddr;
}

void ServerHost::MessageType(QByteArray data, QTcpSocket* socket){
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isNull() && doc.isObject()) {
        QJsonObject obj = doc.object();
        QString type = obj["type"].toString();

        if (type=="AUTH"){
            QString username = obj["name"].toString();
            QJsonObject response;
            response["type"] = "AUTH_OK";
            response["name"] = username;
            for (int i = 0; i < clientCount; i++) {
                if (clients[i].socket == socket) {
                    clients[i].username = username;
                    response["id"] = clients[i].id;
                    emit serverLogMessage(QString("[AUTH]") +QString::number(clients[i].id) + " authenticated as " + clients[i].username);
                    break;
                }
            }
            socket->write(QJsonDocument(response).toJson(QJsonDocument::Compact));
            emit clientListChanged();
        }
        else if (type == "EXIT") {
            QString username = obj["name"].toString();
            for (int i = 0; i < clientCount; i++) {
                if (clients[i].username == username) {
                    emit serverLogMessage(QString("[EXIT]" + clients[i].username + " disconnected"));
                    removeClient(clients[i].socket);
                    break;
                }
            }
            emit clientListChanged();
        }
        else{
            socket->write("Hello");
        }
    }
}
void ServerHost::newConnections() {
    while (this->hasPendingConnections()) {
        QTcpSocket* socket = this->nextPendingConnection();
        socket->setParent(nullptr);
        int idx = addClient(socket);
        int assignedId = clients[idx].id;

        qDebug() << "ID:" << assignedId;
        emit serverLogMessage("[CONNECT] New client ID:" +QString::number(assignedId) +" from " +socket->peerAddress().toString());
        emit clientListChanged();

        socket->write(("ID:" + QString::number(assignedId)).toUtf8());

        QThread* thread = new QThread;
        socket->moveToThread(thread);

        connect(socket, &QTcpSocket::readyRead,[this, socket]() {
            QByteArray data = socket->readAll();
            qDebug() << "Received:" << QString::fromUtf8(data);
            MessageType(data, socket);
        });

        connect(socket, &QTcpSocket::disconnected, this, [this, socket]() {
            for (int i = 0; i < clientCount; i++) {
                if (clients[i].socket == socket) {
                    emit serverLogMessage("[DISCONNECT] ID:" +QString::number(clients[i].id) +" " + clients[i].username +" disconnected");
                    break;
                }
            }
            removeClient(socket);
            emit clientListChanged();
        }, Qt::QueuedConnection);

        connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
        connect(socket, &QTcpSocket::disconnected, thread, &QThread::quit);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        thread->start();
    }
}
