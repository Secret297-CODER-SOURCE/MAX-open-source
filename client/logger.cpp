#include "logger.h"
#include <iostream>
QFile logger::logFile;
bool logger::init(){
    logFile.setFileName("logger.txt");
    return logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
}
void logger::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg){
    LogMessage log;
    log.msg = msg;
    log.timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    log.line = context.line;
    switch (type) {
        case QtDebugMsg:    log.type = "DEBUG";    break;
        case QtInfoMsg:     log.type = "INFO";     break;
        case QtWarningMsg:  log.type = "WARNING";  break;
        case QtCriticalMsg: log.type = "CRITICAL"; break;
        case QtFatalMsg:    log.type = "FATAL";    break;
    }
    if(logFile.isOpen()){
         QTextStream out(&logFile);
        out << QString("[%1] [%2] (%3) -> %4\n")
                    .arg(log.timestamp, log.type, QString::number(log.line), log.msg);
        out.flush();
    }
    std::cout << QString("[%1] [%2] %3\n").arg(log.timestamp, log.type, log.msg).toStdString();

}