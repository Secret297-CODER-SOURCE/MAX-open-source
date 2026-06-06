#ifndef CLIENTS_H
#define CLIENTS_H

#include <QTcpSocket>
#include <QString>

struct ClientInfo {
    QTcpSocket* socket;
    int id;
    QString username;
};

class Clients
{
public:
    Clients();
    ~Clients();

    int addClient(QTcpSocket* socket);
    void removeClient(QTcpSocket* socket);
    ClientInfo* findClientById(int id);
    ClientInfo* findClientByUsername(const QString& username);
    ClientInfo* findClientBySocket(QTcpSocket* socket);
    char** getClientList() const;
    int count() const { return m_count; }
    ClientInfo* at(int i) { return &m_clients[i]; }
    void clear();

private:
    int generateId() const;
    void resizeArray();

    ClientInfo* m_clients;
    int m_count;
    int m_capacity;
};

#endif
