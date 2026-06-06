#ifndef SERVERHOST_H
#define SERVERHOST_H

#include <QTcpServer>
#include <QTcpSocket>
#include "clients.h"
#include "database.h"
#include "api.h"

class ServerHost : public QTcpServer {
    Q_OBJECT
public:
    ServerHost();
    ~ServerHost();

    void start();
    void stop();

    char** getClientList() const { return m_clients.getClientList(); }

signals:
    void clientListChanged();
    void serverLogMessage(const QString& msg);

private slots:
    void newConnections();

private:
    database m_db;
    Clients  m_clients;
    api      m_api;
};

#endif
