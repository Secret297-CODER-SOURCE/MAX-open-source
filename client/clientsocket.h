#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H
#include <QTcpSocket>
#include <QByteArray>
#include <jsonclient.h>
class clientSocket: public QObject
{
    Q_OBJECT
private:
    QTcpSocket *socket;
    QString username;
private slots:
    void onReadyRead();
    void onSocketConnected();

public:
    explicit clientSocket(QString username, QObject *parent = nullptr);
    void ConnectClient(QString ip, quint16 id);
    void DisconnectClient();
    void SendRequest(const QByteArray &data);
signals:
    void connected();
    void disconnected();
};

#endif // CLIENTSOCKET_H
