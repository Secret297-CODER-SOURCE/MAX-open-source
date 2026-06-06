#include "clients.h"
#include <QRandomGenerator>
#include <cstring>

Clients::Clients()
    : m_clients(nullptr), m_count(0), m_capacity(1)
{
    m_clients = new ClientInfo[m_capacity];
}

Clients::~Clients()
{
    delete[] m_clients;
}

int Clients::generateId() const
{
    return int(QRandomGenerator::global()->bounded(10000000, 99999999));
}

void Clients::resizeArray()
{
    int newCapacity = m_capacity * 2;
    ClientInfo* arr = new ClientInfo[newCapacity];
    for (int i = 0; i < m_count; i++)
        arr[i] = m_clients[i];
    delete[] m_clients;
    m_clients = arr;
    m_capacity = newCapacity;
}

ClientInfo* Clients::findClientById(int id)
{
    for (int i = 0; i < m_count; i++)
        if (m_clients[i].id == id) return &m_clients[i];
    return nullptr;
}

ClientInfo* Clients::findClientByUsername(const QString& username)
{
    for (int i = 0; i < m_count; i++)
        if (m_clients[i].username == username) return &m_clients[i];
    return nullptr;
}

ClientInfo* Clients::findClientBySocket(QTcpSocket* socket)
{
    for (int i = 0; i < m_count; i++)
        if (m_clients[i].socket == socket) return &m_clients[i];
    return nullptr;
}

int Clients::addClient(QTcpSocket* socket)
{
    if (m_count >= m_capacity)
        resizeArray();

    int id = generateId();
    while (findClientById(id))
        id = generateId();

    m_clients[m_count].id = id;
    m_clients[m_count].username = "";
    m_clients[m_count].socket = socket;

    return m_count++;
}

void Clients::removeClient(QTcpSocket* socket)
{
    for (int i = 0; i < m_count; i++) {
        if (m_clients[i].socket == socket) {
            for (int j = i; j < m_count - 1; j++)
                m_clients[j] = m_clients[j + 1];
            m_clients[m_count - 1].socket = nullptr;
            m_clients[m_count - 1].id = 0;
            m_clients[m_count - 1].username = "";
            m_count--;
            return;
        }
    }
}

void Clients::clear()
{
    for (int i = 0; i < m_count; i++) {
        if (m_clients[i].socket) {
            m_clients[i].socket->disconnectFromHost();
            m_clients[i].socket = nullptr;
        }
    }
    m_count = 0;
}

char** Clients::getClientList() const
{
    char** out = new char*[m_count + 1];
    for (int i = 0; i < m_count; i++) {
        QString entry = QString::number(m_clients[i].id) + " | "
                        + m_clients[i].username + " | "
                        + m_clients[i].socket->peerAddress().toString();
        QByteArray arr = entry.toUtf8();
        out[i] = new char[arr.size() + 1];
        strcpy(out[i], arr.constData());
    }
    out[m_count] = nullptr;
    return out;
}
