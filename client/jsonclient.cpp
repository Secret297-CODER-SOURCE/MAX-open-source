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

QByteArray JSONClient::PackMessageGet(const int &roomId, const int &FromValue, const int &ToValue){
    QJsonObject obj;
    obj["type"] = "MGET";
    obj["ID"] = roomId;
    obj["From"] = FromValue;
    obj["To"] = ToValue;
    return QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n";
}

void JSONClient::OnGetInfo(const QJsonObject &obj, const QString &type){
    if(type == "AUTH_OK"){
        int id = obj["id"].toInt();
        emit authOkReceived(id);

    }
    else{
        qDebug() << "invalid request";
    }
//додати обробку AUTH_OK!!!!!!!!!!!!!!!!!!!!!!!!!!

}