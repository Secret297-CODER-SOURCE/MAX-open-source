#include <QApplication>
#include "mainwindow.h"
#include <QFile>
#include "logger.h"
int main(int argc, char *argv[])
{
    qputenv("QT_ASSUME_STDERR_HAS_CONSOLE", "1");
    if (logger::init()) {
        qInstallMessageHandler(logger::messageHandler);
    } else {

        qCritical() << "Failed to initialize logger file!";
    }
    QApplication app(argc, argv);
    app.setApplicationName("Login");
    MainWindow window;
    window.resize(900, 700);
    window.show();

    return app.exec();
}