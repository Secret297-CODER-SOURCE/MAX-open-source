#include "clientsocket.h"

clientSocket::clientSocket(QString username, QObject *parent) : QObject(parent) {
    this->username = username;
    id = 0;
    socket = new QTcpSocket(this);
    parser = new JSONClient(this);
    connect(socket, &QTcpSocket::disconnected, this, [this]() {
        qDebug() << "Lost connection with server!";
        emit disconnected();
    });
    connect(socket, &QTcpSocket::readyRead, this, &clientSocket::onReadyRead);

    connect(socket, &QTcpSocket::connected, this, &clientSocket::onSocketConnected);
    connect(parser, &JSONClient::authOkReceived, this, [this](int userId) {
        this->id = userId;
        qDebug() << "id: " << this->id;
        emit idReceived(userId);
    });
}
//----------------------------------
void clientSocket::onSocketConnected(){
    qDebug() << "Successfully connected";
    QByteArray authData = parser->PackLoginForm(this->username);
    socket->write(authData);
    qDebug() << "sent info to server(about auth)";
    emit connected();
}
void clientSocket::onReadyRead(){

    buffer.append(socket->readAll());
    int nextLineIndex;
    while ((nextLineIndex = buffer.indexOf('\n')) != -1) {
        QByteArray buf = buffer.left(nextLineIndex).trimmed();
        buffer.remove(0, nextLineIndex + 1);
        if (buf.isEmpty()) continue;
        QJsonDocument doc = QJsonDocument::fromJson(buf);

        if (doc.isObject()){
            QJsonObject jsonObject = doc.object();
            QString type = jsonObject["type"].toString();
            parser->OnGetInfo(jsonObject, type);
        }
    }

}
//---------------------------------

QString clientSocket::GetName() const{
    return this->username;
}
int clientSocket::GetId() const {
    return this->id;
}

void clientSocket::DisconnectClient(){
    if (socket->state() == QAbstractSocket::ConnectedState) {
        QByteArray exitData = parser->PackExitForm(this->username);
        socket->write(exitData);
        socket->waitForBytesWritten(1000);
        socket->disconnectFromHost();
    }
    else{
        qDebug() << "Socket isnt connected to anything.";
    }
}
void clientSocket::ConnectClient(QString ip, quint16 port){
    if (socket->state() == QAbstractSocket::UnconnectedState) {
        socket->connectToHost(ip, port);
        if (!socket->waitForConnected(5000)) {
            throw std::runtime_error("Connection timed out or failed!");
        }
    }
    else{
        qDebug() << "Connection already exists or failed";
    }

}
void clientSocket::SendRequest(const QByteArray &data){
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(data);
    }
}
