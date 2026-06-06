#include "api.h"
#include <QJsonDocument>
#include <QAbstractSocket>

api::api(Clients* clients, database* db, QObject* parent)
    : QObject(parent), m_clients(clients), m_db(db) {}

void api::sendJson(QTcpSocket* socket, const QJsonObject& obj)
{
    if (!socket) return;
    socket->write(QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n");
}

void api::sendWelcome(QTcpSocket* socket, int tempId)
{
    QJsonObject welcome;
    welcome["type"]   = "WELCOME";
    welcome["tempid"] = tempId;
    sendJson(socket, welcome);
}

void api::handle(const QByteArray& data, QTcpSocket* socket)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return;
    QJsonObject obj = doc.object();
    QString type = obj["type"].toString();

    if      (type == "REG")  handleReg(obj, socket);
    else if (type == "AUTH") handleAuth(obj, socket);
    else if (type == "MSG")  handleMsg(obj, socket);
    else if (type == "EXIT") handleExit(obj, socket);
}

void api::handleReg(const QJsonObject& obj, QTcpSocket* socket)
{
    QString name = obj["name"].toString();
    if (name.isEmpty()) return;

    ClientInfo* c = m_clients->findClientBySocket(socket);
    if (!c) return;

    c->username = name;
    int dbId = c->id;
    m_db->registerUser(name, dbId);

    QJsonObject resp;
    resp["type"] = "AUTH_OK";
    resp["id"]   = dbId;
    sendJson(socket, resp);

    emit logMessage("[REG] " + name + " id=" + QString::number(dbId));
    emit clientListChanged();
}

void api::handleAuth(const QJsonObject& obj, QTcpSocket* socket)
{
    int userid = obj["userid"].toInt();
    QString name = m_db->getUsernameById(userid);
    QJsonObject resp;
    if (name.isEmpty()) {
        resp["type"] = "AUTH_FAIL";
    } else {
        ClientInfo* c = m_clients->findClientBySocket(socket);
        if (c) c->username = name;
        resp["type"] = "AUTH_OK";
        resp["id"]   = userid;
    }
    sendJson(socket, resp);
    emit logMessage("[AUTH] id=" + QString::number(userid) + " -> " + (name.isEmpty() ? "FAIL" : name));
    emit clientListChanged();
}

void api::handleMsg(const QJsonObject& obj, QTcpSocket* socket)
{
    Q_UNUSED(socket);
    QString senderName = obj["senderName"].toString();
    int     receiverId = obj["receiverId"].toInt();
    QString content    = obj["content"].toString();
    if (senderName.isEmpty() || content.isEmpty()) return;

    int sId = m_db->getUserId(senderName);
    if (sId != -1 && receiverId != -1)
        m_db->saveMessage(sId, receiverId, content);

    ClientInfo* receiver = m_clients->findClientById(receiverId);
    if (receiver && receiver->socket->state() == QAbstractSocket::ConnectedState) {
        QJsonObject delivery;
        delivery["type"]       = "MSG";
        delivery["senderName"] = senderName;
        delivery["content"]    = content;
        sendJson(receiver->socket, delivery);
    }
    emit logMessage("[MSG] " + senderName + " -> id:" + QString::number(receiverId) + ": " + content);
}

void api::handleExit(const QJsonObject& obj, QTcpSocket* socket)
{
    Q_UNUSED(socket);
    QString name = obj["name"].toString();
    ClientInfo* c = m_clients->findClientByUsername(name);
    if (c) {
        emit logMessage("[EXIT] " + name);
        m_clients->removeClient(c->socket);
        emit clientListChanged();
    }
}
