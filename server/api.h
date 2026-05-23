#ifndef API_H
#define API_H
#include "client.h"
#include <cstring>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
class api:client
{
    Q_OBJECT
    QString type;
public:
    QJsonObject response;
    QString username;
    api();
    QString EndPoint(QByteArray data);
};

#endif // API_H
