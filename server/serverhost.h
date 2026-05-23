#ifndef SERVERHOST_H
#define SERVERHOST_H
#include <QCoreApplication>
#include <QThread>
#include <QDebug>
#include <QMap>
#include <QRandomGenerator>
#include "api.h"


class ServerHost : api {

public:
    static const QMetaObject staticMetaObject;
    virtual const QMetaObject *metaObject() const;
    virtual void *qt_metacast(const char *);
    virtual int qt_metacall(QMetaObject ::Call, int, void **);
    static inline QString tr(const char *s, const char *c = nullptr, int n = -1) {
        return staticMetaObject.tr(s, c, n);
    }
    __attribute__((__deprecated__)) static inline QString
    trUtf8(const char *s, const char *c = nullptr, int n = -1) {
        return staticMetaObject.tr(s, c, n);
    }

private:
    __attribute__((visibility("hidden"))) static void
    qt_static_metacall(QObject *, QMetaObject ::Call, int, void **);
    struct QPrivateSignal {};

public:
    ServerHost();
    ~ServerHost();
    void removeClient(QTcpSocket* socket);
    char** getClientList();
    int generateId();
    int addClient(QTcpSocket* socket);

    void resizeArray();
    void start();
    void stop();
    void newConnections();



    //Client EndPoint
    void AuthClient(QJsonObject response,QTcpSocket* socket,QString username);
    void ExitClient(QString username);
signals:
    void clientListChanged();
    void serverLogMessage(const QString& msg);
};

#endif // SERVERHOST_H
