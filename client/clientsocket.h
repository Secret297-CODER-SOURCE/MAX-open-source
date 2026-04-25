#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H
#include <QTcpSocket>
class clientSocket: public QObject
{
    Q_OBJECT
private:
    QTcpSocket *socket;
public:
    explicit clientSocket(QObject *parent = nullptr);
    void ConnectClient(QString ip, quint16 id);
};

#endif // CLIENTSOCKET_H
