#include "api.h"

api::api() {}
// якщо летить багато запитів реалізувати rate-limit та якщо запити сторонні зробити forbidden

QString api::EndPoint(QByteArray data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isNull() && doc.isObject()) {
        QJsonObject obj = doc.object();
        type = obj["type"].toString();
        username = obj["name"].toString();
        if (type=="AUTH"){
            response["type"] = "AUTH_OK";
            response["name"] = username;
        }
    }
    return type;
}

