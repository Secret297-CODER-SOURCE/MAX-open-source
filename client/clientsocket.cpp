#include "clientsocket.h"

clientSocket::clientSocket(QObject *parent) : QObject(parent) {
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, [this]() {

        qDebug() << "Successfully connected!";

    });
}
x
void clientSocket::ConnectClient(QString ip, quint16 id){
    if (socket->state() == QAbstractSocket::UnconnectedState) {
        socket->connectToHost(ip, id);
    }
    else{
        qDebug() << "Connection already exists or failed";
    }
}
