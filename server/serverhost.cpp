#include "serverhost.h"
#include <QDebug>

ServerHost::ServerHost()
    : m_clients(), m_api(&m_clients, &m_db)
{
    m_db.open();
    if (!m_db.isOpen()) {
        qDebug() << "[DB] Fail";
    } else {
        qDebug() << "[DB] Database opened";
    }

    connect(&m_api, &api::logMessage,        this, &ServerHost::serverLogMessage);
    connect(&m_api, &api::clientListChanged, this, &ServerHost::clientListChanged);
}

ServerHost::~ServerHost()
{
    m_clients.clear();
}

void ServerHost::start()
{
    disconnect(this, &QTcpServer::newConnection, this, &ServerHost::newConnections);
    connect(this,    &QTcpServer::newConnection, this, &ServerHost::newConnections);
    this->listen(QHostAddress::Any, 8888);
}

void ServerHost::stop()
{
    m_clients.clear();
    this->close();
}

void ServerHost::newConnections()
{
    while (this->hasPendingConnections()) {
        QTcpSocket* socket = this->nextPendingConnection();
        int idx = m_clients.addClient(socket);
        int assignedId = m_clients.at(idx)->id;

        emit serverLogMessage("[CONNECT] New client ID:" + QString::number(assignedId)
                              + " from " + socket->peerAddress().toString());
        emit clientListChanged();

        m_api.sendWelcome(socket, assignedId);

        connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
            QByteArray data = socket->readAll();
            m_api.handle(data, socket);
        });

        connect(socket, &QTcpSocket::disconnected, this, [this, socket]() {
            ClientInfo* c = m_clients.findClientBySocket(socket);
            if (c) {
                emit serverLogMessage("[DISCONNECT] ID:" + QString::number(c->id)
                                      + " " + c->username + " disconnected");
            }
            m_clients.removeClient(socket);
            emit clientListChanged();
        });

        connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    }
}
