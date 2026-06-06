#include <QString>
#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include <QMutex>
struct LogMessage {
    QString msg;
    QString type;
    QString timestamp;
    int line;
};
class logger {
public:

    static bool init();


    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

private:
    static QFile logFile;
    static QMutex mutex;

};