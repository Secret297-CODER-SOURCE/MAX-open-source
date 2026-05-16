#ifndef JSONCLIENT_H
#define JSONCLIENT_H
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QByteArray>
class JSONClient: public QObject
{
    Q_OBJECT
public:
    explicit JSONClient(QObject *parent = nullptr) : QObject(parent) {}
    QByteArray PackLoginForm(const QString &name);
    QByteArray PackMessageForm(const QString &name,const QString &message, const int roomId );
    QByteArray PackExitForm(const QString &name);
    QByteArray PackChatCrtForm(const QString &chatname, const QList<int> &ids);
    QByteArray PackMessageGet(const int &roomId, const int &FromValue, const int &ToValue);
    void OnGetInfo(const QJsonObject &obj, const QString &type);
signals:
    void authOkReceived(int id);
};

#endif // JSONCLIENT_H
