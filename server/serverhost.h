#ifndef SERVERHOST_H
#define SERVERHOST_H
#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QDebug>
#include <QMap>

class ServerHost : public QTcpServer {
public:
    ServerHost();
    ~ServerHost();
    QTcpServer* server;
    QTcpSocket** clientSocket;
    int addClient(QTcpSocket* socket);
    int clientCount;
    int capacity;
    void resizeArray();
    void start();
    void stop();
    void newConnections();
    char** getIpAddressList();
};

#endif // SERVERHOST_H
