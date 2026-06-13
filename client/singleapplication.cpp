#include "singleapplication.h"
#include <QDebug>

SingleApplication::SingleApplication(int &argc, char **argv, const QString &uniqueKey)
    : QApplication(argc, argv), m_uniqueKey(uniqueKey), m_isRunning(false), m_localServer(nullptr)
{
    QLocalSocket socket;
    socket.connectToServer(m_uniqueKey);


    if (socket.waitForConnected(500)) {
        m_isRunning = true;
        qDebug() << "Another instance is already running. Exiting.";
        return;
    }

    m_localServer = new QLocalServer(this);

    QLocalServer::removeServer(m_uniqueKey);

    if (!m_localServer->listen(m_uniqueKey)) {
        qCritical() << "Server failed to listen:" << m_localServer->errorString();
    } else {
        qDebug() << "Single instance server started successfully. Listening for other copies...";
        connect(m_localServer, &QLocalServer::newConnection, this, &SingleApplication::receiveNewConnection);
    }
}

void SingleApplication::receiveNewConnection()
{
    QLocalSocket *nextInConnection = m_localServer->nextPendingConnection();
    if (nextInConnection) {
        nextInConnection->close();
        nextInConnection->deleteLater();

        qDebug() << "Detected attempt to launch a second instance. Raising main window...";
        emit instanceStarted();
    }
}