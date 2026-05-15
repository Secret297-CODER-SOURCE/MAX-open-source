#include "serverhost.h"
#include <QAbstractSocket>
#include <QString>

ServerHost::ServerHost() {
    clientCount = 0;
    capacity = 1;
    clients = new ClientInfo[capacity];
    m_db.open();
    if (!m_db.isOpen()) {
        qDebug() << "[DB] Fail";
    } else {
        qDebug() << "[DB] Database opened successfully";
    }
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
    quint64 id = QRandomGenerator::global()->bounded(10000000, 99999999);
    return int(id);
}

ClientInfo* ServerHost::findClientById(const int& id) {
    for (int i = 0; i < clientCount; i++) {
        if (clients[i].id == id)
            return &clients[i];
    }
    return nullptr;
}

ClientInfo* ServerHost::findClientByUsername(const QString& username) {
    for (int i = 0; i < clientCount; i++) {
        if (clients[i].username == username)
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

char** ServerHost::getClientList() {
    char** listOfAddr = new char*[clientCount + 1];
    for (int i = 0; i < clientCount; i++) {
        QString entry = QString::number(clients[i].id) + " | " + clients[i].username + " | " + clients[i].socket->peerAddress().toString();
        QByteArray arr = entry.toUtf8();
        listOfAddr[i] = new char[arr.size() + 1];
        strcpy(listOfAddr[i], arr.constData());
    }
    listOfAddr[clientCount] = nullptr;
    return listOfAddr;
}

void ServerHost::sendHistory(QTcpSocket* socket, int userId) {
    if (!m_db.isOpen()) return;

    QList<MessageRecord> history = m_db.getAllMessages(userId);
    if (history.isEmpty()) return;

    QJsonArray arr;
    for (const MessageRecord& r : history) {
        QJsonObject msg;
        msg["senderId"]   = r.senderId;
        msg["receiverId"] = r.receiverId;
        msg["content"]    = r.content;
        msg["timestamp"]  = r.timestamp;
        arr.append(msg);
    }
    QJsonObject packet;
    packet["type"]     = "HISTORY";
    packet["messages"] = arr;
    if (socket && socket->state() == QAbstractSocket::ConnectedState)
        socket->write(QJsonDocument(packet).toJson(QJsonDocument::Compact));
    emit serverLogMessage(QString("[HISTORY] Sent"));
}

void ServerHost::MessageType(QByteArray data, QTcpSocket* socket) {
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isNull() && doc.isObject()) {
        QJsonObject obj = doc.object();
        QString type = obj["type"].toString();

        if (type == "AUTH") {
            QString username = obj["name"].toString();

            for (int i = 0; i < clientCount; i++) {
                if (clients[i].socket == socket) {
                    clients[i].username = username;
                    int dbUserId = -1;
                    if (m_db.isOpen()) {
                        m_db.registerUser(username);
                        dbUserId = m_db.getUserId(username);
                    }

                    QJsonObject response;
                    response["type"] = "AUTH_OK";
                    response["name"] = username;
                    response["id"]   = clients[i].id;
                    if (socket && socket->state() == QAbstractSocket::ConnectedState)
                        socket->write(QJsonDocument(response).toJson(QJsonDocument::Compact));
                    emit serverLogMessage(QString("[AUTH] ID:%1 authenticated as %2")
                                              .arg(clients[i].id).arg(username));
                    emit clientListChanged();
                    if (dbUserId != -1)
                        sendHistory(socket, dbUserId);

                    break;
                }
            }
        }
        else if (type == "MESSAGE") {
            QString senderName   = obj["senderName"].toString();
            QString receiverName = obj["receiverName"].toString();
            QString content      = obj["content"].toString();

            if (senderName.isEmpty() || receiverName.isEmpty() || content.isEmpty())
                return;

            ClientInfo* sender = findClientByUsername(senderName);
            if (!sender)
                return;

            int senderId = sender->id;

            int senderDbId   = m_db.getUserId(senderName);
            int receiverDbId = m_db.getUserId(receiverName);

            if (senderDbId == -1) {
                m_db.registerUser(senderName);
                senderDbId = m_db.getUserId(senderName);
            }

            if (receiverDbId == -1) {
                m_db.registerUser(receiverName);
                receiverDbId = m_db.getUserId(receiverName);
            }

            m_db.saveMessage(senderDbId, receiverDbId, content);

            QJsonObject delivery;
            delivery["type"]     = "MESSAGE";
            delivery["senderId"] = senderId;
            delivery["content"]  = content;

            ClientInfo* receiver = findClientByUsername(receiverName);
            if (receiver && receiver->socket) {
                if (receiver->socket->state() == QAbstractSocket::ConnectedState)
                    receiver->socket->write(QJsonDocument(delivery).toJson(QJsonDocument::Compact));
                emit serverLogMessage(QString("[MSG] ") + senderName + " to " + receiverName + ": " + content);
            } else {
                QJsonObject offline;
                offline["type"]    = "STATUS";
                offline["message"] = receiverName + " is offline. Message saved.";
                if (socket && socket->state() == QAbstractSocket::ConnectedState)
                    socket->write(QJsonDocument(offline).toJson(QJsonDocument::Compact));
                emit serverLogMessage(QString("[MSG] stored)"));
            }
        }
        else if (type == "HISTORY_REQUEST") {
            QString username = obj["name"].toString();
            int userId = m_db.getUserId(username);
            sendHistory(socket, userId);
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
        else {
            socket->write("Hello");
        }
    }
}

void ServerHost::newConnections() {
    while (this->hasPendingConnections()) {
        QTcpSocket* socket = this->nextPendingConnection();
        int idx = addClient(socket);
        int assignedId = clients[idx].id;

        qDebug() << "ID:" << assignedId;
        emit serverLogMessage("[CONNECT] New client ID:" + QString::number(assignedId) + " from " + socket->peerAddress().toString());
        emit clientListChanged();

        socket->write((("{ID:" + QString::number(assignedId)) + "}").toUtf8());

        connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
            QByteArray data = socket->readAll();
            qDebug() << "Received:" << QString::fromUtf8(data);
            MessageType(data, socket);
        });

        connect(socket, &QTcpSocket::disconnected, this, [this, socket]() {
            for (int i = 0; i < clientCount; i++) {
                if (clients[i].socket == socket) {
                    emit serverLogMessage("[DISCONNECT] ID:" + QString::number(clients[i].id) + " " + clients[i].username + " disconnected");
                    break;
                }
            }
            removeClient(socket);
            emit clientListChanged();
        });

        connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    }
}
