#include "jsonclient.h"

QByteArray JSONClient::PackLoginForm(const QString &name){
    QJsonObject obj;
    obj["type"] = "AUTH";
    obj["name"] = name;
    return QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n";

}
QByteArray JSONClient::PackMessageForm(const QString &name,const QString &message, const int roomId){
    QJsonObject obj;
    obj["type"] = "MSG";
    obj["name"] = name;
    obj["message"] = message;
    obj["roomid"] = roomId;
    return QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n";
}

QByteArray JSONClient::PackExitForm(const QString &name){
    QJsonObject obj;
    obj["type"] = "EXIT";
    obj["name"] = name;
    return QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n";
}

QByteArray JSONClient::PackChatCrtForm(const QString &chatname, const QList<int> &ids){
    QJsonObject obj;
    obj["type"] = "CHATCRT";
    obj["chatname"] = chatname;
    QJsonArray usersArray;
    for (int id : ids) {
        usersArray.append(id);
    }
    obj["ids"] = usersArray;
    return QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n";
}
