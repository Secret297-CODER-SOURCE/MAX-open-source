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
    QByteArray PackExitForm(const int &id);
    QByteArray PackDirectMessage(const QString &senderName, int receiverId, const QString &content);
    QByteArray PackChatCrtForm(const QString &chatname, const QList<int> &ids);
    QByteArray PackMessageGet(const int &roomId, const int &FromValue, const int &ToValue);
    QByteArray PackUserInfo(const int &userid);
    QByteArray PackChatList();

    void OnGetInfo(const QJsonObject &obj, const QString &type);
signals:
    void authOkReceived(int id);
    void UsernameRecieved(int id, QString name);
    void AuthFailed();
    void messageReceived(const QString &senderName, const QString &content);
    void UserNotFound(int id);
};

#endif // JSONCLIENT_H
