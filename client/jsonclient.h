#ifndef JSONCLIENT_H
#define JSONCLIENT_H
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QByteArray>
class JSONClient
{
public:
    static QByteArray PackLoginForm(const QString &name);
    static QByteArray PackMessageForm(const QString &name,const QString &message, const int roomId );
    static QByteArray PackExitForm(const QString &name);
    static QByteArray PackChatCrtForm(const QString &chatname, const QList<int> &ids);
};

#endif // JSONCLIENT_H
