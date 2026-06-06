#ifndef API_H
#define API_H

#include <QObject>
#include <QByteArray>
#include <QJsonObject>
#include <QTcpSocket>
#include "clients.h"
#include "database.h"

class api : public QObject
{
    Q_OBJECT
public:
    explicit api(Clients* clients, database* db, QObject* parent = nullptr);

    void handle(const QByteArray& data, QTcpSocket* socket);
    void sendWelcome(QTcpSocket* socket, int tempId);
    static void sendJson(QTcpSocket* socket, const QJsonObject& obj);

signals:
    void logMessage(const QString& msg);
    void clientListChanged();

private:
    void handleReg(const QJsonObject& obj, QTcpSocket* socket);
    void handleAuth(const QJsonObject& obj, QTcpSocket* socket);
    void handleMsg(const QJsonObject& obj, QTcpSocket* socket);
    void handleExit(const QJsonObject& obj, QTcpSocket* socket);
    void handleList(const QJsonObject& obj, QTcpSocket* socket);
    void handleUserInfo(const QJsonObject& obj, QTcpSocket* socket);

    Clients* m_clients;
    database* m_db;
};

#endif
