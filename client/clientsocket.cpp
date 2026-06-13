#include "clientsocket.h"

clientSocket::clientSocket(QString username, QObject *parent) : QObject(parent) {
    this->username = username;
    this->id = 0;

    initSocketAndParser();
}

clientSocket::clientSocket(int clientId, QObject *parent)
    : QObject(parent)
{
    this->id = clientId;
    this->username = "";

    initSocketAndParser();
}

void clientSocket::initSocketAndParser()
{
    socket = new QTcpSocket(this);
    parser = new JSONClient(this);

    connect(socket, &QTcpSocket::disconnected, this, [this]() {
        qDebug() << "Lost connection with server!";
        emit disconnected();
    });
    connect(socket, &QTcpSocket::readyRead, this, &clientSocket::onReadyRead);
    connect(socket, &QTcpSocket::connected, this, &clientSocket::onSocketConnected);
    connect(parser, &JSONClient::UserNotFound, this, &clientSocket::UserNotFound);
    connect(parser,&JSONClient::UsernameRecieved,this,&clientSocket::UsernameRecieved);
    connect(parser, &JSONClient::AuthFailed, this, [this](){
        qWarning() << "Authentication failed, resetting ID and sending registration request.";
        this->id = 0;
        Fsystem::saveId(0);
        SendRegistrationRequest();
    });
    connect(parser, &JSONClient::authOkReceived, this, [this](int userId) {
        this->id = userId;
        Fsystem::saveId(userId);
        if (!this->username.isEmpty())
            Fsystem::saveName(this->username);
        emit idReceived(userId);
    });
    connect(parser, &JSONClient::messageReceived, this, &clientSocket::messageReceived);
}

void clientSocket::onSocketConnected(){
    qDebug() << "Successfully connected";
    if (this->id > 0) {
        qDebug() << "ID exists (" << this->id << "), sending authentication request.";
        SendAuthenticationRequest();
    } else {
        qDebug() << "No valid ID found, sending registration request.";
        SendRegistrationRequest();
    }
    emit connected();
}

void clientSocket::onReadyRead(){
    QByteArray data = socket->readAll();
    qDebug() << "Received data chunk, bytes:" << data.size();
    buffer.append(data);

    int nextLineIndex;
    while ((nextLineIndex = buffer.indexOf('\n')) != -1) {
        QByteArray buf = buffer.left(nextLineIndex).trimmed();
        buffer.remove(0, nextLineIndex + 1);
        if (buf.isEmpty()) continue;

        qDebug() << "Parsing JSON packet:" << buf;
        QJsonDocument doc = QJsonDocument::fromJson(buf);

        if (doc.isObject()){
            QJsonObject jsonObject = doc.object();
            QString type = jsonObject["type"].toString();
            qDebug() << "Processing packet type:" << type;
            parser->OnGetInfo(jsonObject, type);
        } else {
            qWarning() << "Received invalid JSON format (not an object).";
        }
    }
}

QString clientSocket::GetName() const{
    return this->username;
}

int clientSocket::GetId() const {
    return this->id;
}

void clientSocket::DisconnectClient(){
    if (socket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "Disconnecting client. Sending exit form for username:" << this->username;
        QByteArray exitData = parser->PackExitForm(this->id);
        SendRequest(exitData);
        if (!socket->waitForBytesWritten(1000)) {
            qWarning() << "Timeout waiting for bytes to be written during disconnect.";
        }
        socket->disconnectFromHost();
    }
    else{
        qWarning() << "Socket isnt connected to anything. Cannot disconnect.";
    }
}

void clientSocket::ConnectClient(QString ip, quint16 port){
    if (socket->state() == QAbstractSocket::UnconnectedState) {
        qDebug() << "Attempting to connect to" << ip << ":" << port;
        socket->connectToHost(ip, port);
        if (!socket->waitForConnected(5000)) {
            qCritical() << "Socket connection timeout or failed to connect to" << ip << ":" << port;
            emit ConnectError();
        }
    }
    else{
        qWarning() << "Connection already exists or failed. Current state:" << socket->state();
    }
}

void clientSocket::SendRequest(const QByteArray &data){
    if (socket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "Sending request data, size:" << data.size();
        socket->write(data);
    } else {
        qWarning() << "Cannot send request, socket is not connected. Current state:" << socket->state();
    }
}

void clientSocket::SendRegistrationRequest(){
    qDebug() << "Sending registration request for username:" << this->username;
    QByteArray authData = parser->PackRegisterForm(this->username);
    SendRequest(authData);
}

void clientSocket::SendAuthenticationRequest(){
    qDebug() << "Sending authentication request for id:" << this->id;
    QByteArray authData = parser->PackLoginForm(this->id);
    SendRequest(authData);
}
void clientSocket::SendDirectMessage(int receiverId, const QString &content) {
    QByteArray data = parser->PackDirectMessage(this->username, receiverId, content);
    SendRequest(data);
}
void clientSocket::RequestUserInfo(int id)
{
    socket->write(parser->PackUserInfo(id));
}