#include "clientsocket.h"

clientSocket::clientSocket(QString username, QObject *parent) : QObject(parent) {
    this->username = username;
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::disconnected, this, [this]() {
        qDebug() << "Lost connection with server!";
        emit disconnected();
    });
    connect(socket, &QTcpSocket::readyRead, this, &clientSocket::onReadyRead);

    connect(socket, &QTcpSocket::connected, this, &clientSocket::onSocketConnected);
}
//----------------------------------
void clientSocket::onSocketConnected(){
    qDebug() << "Successfully connected";
    QByteArray authData = JSONClient::PackLoginForm(this->username);
    socket->write(authData);
    qDebug() << "sent info to server(about auth)";
    emit connected();
}
void clientSocket::onReadyRead(){
    QByteArray data = socket->readAll();
    qDebug() << "Received data:" << data;
}
//---------------------------------



void clientSocket::DisconnectClient(){
    if (socket->state() == QAbstractSocket::ConnectedState) {
        QByteArray exitData = JSONClient::PackExitForm(this->username);
        socket->write(exitData);
        socket->waitForBytesWritten(1000);
        socket->disconnectFromHost();
    }
    else{
        qDebug() << "Socket isnt connected to anything.";
    }
}
void clientSocket::ConnectClient(QString ip, quint16 id){
    if (socket->state() == QAbstractSocket::UnconnectedState) {
        socket->connectToHost(ip, id);
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
