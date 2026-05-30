#ifndef JSONCLIENT_H
#define JSONCLIENT_H
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QByteArray>
#include <QFile>
#include <QDateTime>
class JSONClient: public QObject
{
    Q_OBJECT
public:
    explicit JSONClient(QObject *parent = nullptr) : QObject(parent) {}
    QByteArray PackLoginForm(const int &userid);
    QByteArray PackRegisterForm(const QString &name);
    QByteArray PackMessageForm(const QString &name,const QString &message, const int &roomId, const QDateTime &currentTime );
    QByteArray PackExitForm(const QString &name);
    QByteArray PackChatCrtForm(const QString &chatname, const QList<int> &ids);
    QByteArray PackMessageGet(const int &roomId, const int &FromValue, const int &ToValue);

    void OnGetInfo(const QJsonObject &obj, const QString &type);
signals:
    void authOkReceived(int id);
    void AuthFailed();
};

#endif // JSONCLIENT_H
