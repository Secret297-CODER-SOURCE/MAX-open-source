#ifndef SERVERHOST_H
#define SERVERHOST_H
#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QDebug>
#include <QMap>

class ServerHost : public QTcpServer {
    Q_OBJECT
public:
    ServerHost();
    ~ServerHost();
    QTcpSocket** clientSocket;
    int addClient(QTcpSocket* socket);
    int clientCount;
    int capacity;
    void resizeArray();
    void start();
    void stop();
    void newConnections();
    char** getIpAddressList();

signals:
    void clientListChanged();
};

#endif // SERVERHOST_H
