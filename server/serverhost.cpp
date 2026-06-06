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
        qDebug() << "[DB] Database opened";
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

void ServerHost::MessageType(QByteArray data, QTcpSocket* socket) {
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return;
    QJsonObject obj = doc.object();
    QString type = obj["type"].toString();

    if (type == "REG") {
        QString name = obj["name"].toString();
        if (name.isEmpty()) return;
        int customId = -1;
        for (int i = 0; i < clientCount; i++) {
            if (clients[i].socket == socket) {
                clients[i].username = name;
                customId = clients[i].id;
                break;
            }
        }
        if (customId == -1) return;
        m_db.registerUser(name, customId);
        int dbId = customId;
        for (int i = 0; i < clientCount; i++) {
            if (clients[i].socket == socket) {
                clients[i].username = name;
                break;
            }
        }
        QJsonObject resp;
        resp["type"] = "AUTH_OK";
        resp["id"]   = dbId;
        socket->write(QJsonDocument(resp).toJson(QJsonDocument::Compact) + "\n");
        emit serverLogMessage("[REG] " + name + " id=" + QString::number(dbId));
        emit clientListChanged();
    }
    else if (type == "AUTH") {
        int userid = obj["userid"].toInt();
        QString name = m_db.getUsernameById(userid);
        QJsonObject resp;
        if (name.isEmpty()) {
            resp["type"] = "AUTH_FAIL";
        } else {
            for (int i = 0; i < clientCount; i++) {
                if (clients[i].socket == socket) {
                    clients[i].username = name;
                    break;
                }
            }
            resp["type"] = "AUTH_OK";
            resp["id"]   = userid;
        }
        socket->write(QJsonDocument(resp).toJson(QJsonDocument::Compact) + "\n");
        emit serverLogMessage("[AUTH] id=" + QString::number(userid) + " -> " + (name.isEmpty() ? "FAIL" : name));
        emit clientListChanged();
    }
    else if (type == "MSG") {
        QString senderName = obj["senderName"].toString();
        int     receiverId = obj["receiverId"].toInt();  // по ID!
        QString content    = obj["content"].toString();
        if (senderName.isEmpty() || content.isEmpty()) return;

        int sId = m_db.getUserId(senderName);
        if (sId != -1 && receiverId != -1)
            m_db.saveMessage(sId, receiverId, content);

        // шукаємо отримувача по ID
        ClientInfo* receiver = findClientById(receiverId);
        if (receiver && receiver->socket->state() == QAbstractSocket::ConnectedState) {
            QJsonObject delivery;
            delivery["type"]       = "MSG";
            delivery["senderName"] = senderName;
            delivery["content"]    = content;
            receiver->socket->write(QJsonDocument(delivery).toJson(QJsonDocument::Compact) + "\n");
        }
        emit serverLogMessage("[MSG] " + senderName + " -> id:" + QString::number(receiverId) + ": " + content);
    }
    else if (type == "EXIT") {
        QString name = obj["name"].toString();
        ClientInfo* c = findClientByUsername(name);
        if (c) {
            emit serverLogMessage("[EXIT] " + name);
            removeClient(c->socket);
            emit clientListChanged();
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

        //socket->write((("{ID:" + QString::number(assignedId)) + "}").toUtf8());
        QJsonObject welcome;
        welcome["type"] = "WELCOME";
        welcome["tempid"] = assignedId;
        socket->write(QJsonDocument(welcome).toJson(QJsonDocument::Compact) + "\n");

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
