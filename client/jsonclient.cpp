#include "jsonclient.h"
#include <QDebug>

QByteArray JSONClient::PackLoginForm(const int &userid){
    qDebug() << "Packing AUTH form for userid:" << userid;
    QJsonObject obj;
    obj["type"] = "AUTH";
    obj["userid"] = userid;
    return QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n";
}

QByteArray JSONClient::PackMessageForm(const QString &name,const QString &message, const int &roomId, const QDateTime &currentTime){
    qDebug() << "Packing MSG form. From:" << name << "Room:" << roomId;
    QJsonObject obj;
    obj["type"] = "MSG";
    obj["name"] = name;
    obj["message"] = message;
    obj["roomid"] = roomId;
    obj["time"] = currentTime.toString(Qt::ISODate);
    return QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n";
}

QByteArray JSONClient::PackRegisterForm(const QString &name){
    qDebug() << "Packing REG form for name:" << name;
    QJsonObject obj;
    obj["type"] = "REG";
    obj["name"] = name;
    return QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n";
}

QByteArray JSONClient::PackExitForm(const QString &name){
    qDebug() << "Packing EXIT form for name:" << name;
    QJsonObject obj;
    obj["type"] = "EXIT";
    obj["name"] = name;
    return QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n";
}

QByteArray JSONClient::PackChatCrtForm(const QString &chatname, const QList<int> &ids){
    qDebug() << "Packing CHATCRT form. ChatName:" << chatname << "Participants count:" << ids.size();
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
    qDebug() << "Packing MGET form. Room:" << roomId << "From:" << FromValue << "To:" << ToValue;
    QJsonObject obj;
    obj["type"] = "MGET";
    obj["id"] = roomId;
    obj["from"] = FromValue;
    obj["to"] = ToValue;
    return QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n";
}

void JSONClient::OnGetInfo(const QJsonObject &obj, const QString &type){
    qDebug() << "Processing incoming JSON packet info. Type:" << type;

    if(type == "AUTH_OK"){
        int id = obj["id"].toInt();
        qDebug() << "Received AUTH_OK. Parsing ID:" << id;
        emit authOkReceived(id);
    }
    else if(type == "AUTH_FAIL"){
        qWarning() << "Received AUTH_FAIL from server.";
        emit AuthFailed();
    }
    else{
        qWarning() << "Received unknown or invalid request type:" << type;
    }
}