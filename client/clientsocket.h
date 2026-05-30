#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QTcpSocket>
#include <QByteArray>
#include <jsonclient.h>
#include <fsystem.h>

class clientSocket: public QObject
{
    Q_OBJECT
private:
    QTcpSocket *socket;
    QString username;
    int id;
    QByteArray buffer;
    JSONClient* parser;

private slots:
    void onReadyRead();
    void onSocketConnected();

public:

    explicit clientSocket(QString username, QObject *parent = nullptr);
    explicit clientSocket(int clientId, QObject *parent = nullptr);

    void ConnectClient(QString ip, quint16 port);
    void DisconnectClient();
    void SendRequest(const QByteArray &data);
    QString GetName() const;
    int GetId() const;
    void setClientId(int newId);
    void SendRegistrationRequest();
    void SendAuthenticationRequest();
    void initSocketAndParser();
signals:
    void connected();
    void disconnected();
    void idReceived(int userId);
    void ConnectError();
};

#endif // CLIENTSOCKET_H