#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "jsonclient.h"
#include "fsystem.h"

class clientSocket : public QObject
{
    Q_OBJECT

public:
    explicit clientSocket(QString username, QObject *parent = nullptr);
    explicit clientSocket(int clientId, QObject *parent = nullptr);

    QString GetName() const;
    int     GetId()   const;

    void ConnectClient(QString ip, quint16 port);
    void DisconnectClient();
    void SendDirectMessage(int receiverId, const QString &content);

signals:
    void connected();
    void disconnected();
    void ConnectError();
    void idReceived(int userId);
    void messageReceived(const QString &from, const QString &content);

private slots:
    void onSocketConnected();
    void onReadyRead();

private:
    void initSocketAndParser();
    void SendRequest(const QByteArray &data);
    void SendRegistrationRequest();
    void SendAuthenticationRequest();

    QTcpSocket  *socket  = nullptr;
    JSONClient  *parser  = nullptr;

    QString      username;
    int          id;
    QByteArray   buffer;
};