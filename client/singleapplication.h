#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

#include <QApplication>
#include <QLocalServer>
#include <QLocalSocket>

class SingleApplication : public QApplication
{
    Q_OBJECT
public:
    SingleApplication(int &argc, char **argv, const QString &uniqueKey);

    bool isRunning() const { return m_isRunning; }

signals:

    void instanceStarted();

private slots:
    void receiveNewConnection();

private:
    bool m_isRunning;
    QString m_uniqueKey;
    QLocalServer *m_localServer;
};

#endif