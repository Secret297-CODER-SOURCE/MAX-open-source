#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpServer>
class client: public QTcpServer
{
    int clientCount;
    int capacity;
    int* id;
    QTcpSocket** socket;
    QString* username;


public:
    client();
    void findClientById(const int& id);
};

#endif // CLIENT_H

