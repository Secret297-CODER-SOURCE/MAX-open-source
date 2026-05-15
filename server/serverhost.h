#ifndef SERVERHOST_H
#define SERVERHOST_H
#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QDebug>
#include <QMap>
#include <QRandomGenerator>
#include <cstring>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "database.h"

struct ClientInfo {
    QTcpSocket* socket;
    int id;
    QString username;
};

class ServerHost : public QTcpServer {
    Q_OBJECT
public:
    ServerHost();
    ~ServerHost();
    void removeClient(QTcpSocket* socket);
    char** getClientList();
    int generateId();
    int addClient(QTcpSocket* socket);
    void MessageType(QByteArray data,QTcpSocket* socket);
    void resizeArray();
    void start();
    void stop();
    void newConnections();
    ClientInfo* findClientById(const int& id);
    ClientInfo* findClientByUsername(const QString& username);
    void sendMessage(QTcpSocket* socket, const QJsonObject& obj);
    void sendHistory(QTcpSocket* socket, int userId);
    database m_db;
    int clientCount;
    int capacity;
    ClientInfo* clients;

signals:
    void clientListChanged();
    void serverLogMessage(const QString& msg);
};

#endif // SERVERHOST_H
