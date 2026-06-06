#include <QApplication>
#include "mainwindow.h"
#include <QFile>
#include "logger.h"
#include <iostream>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("Login");

    if (!logger::init()) {

        std::cerr << "CRITICAL: Failed to initialize logger file!" << std::endl;
        return -1;
    }

    qInstallMessageHandler(logger::messageHandler);

    qInfo() << "Application started successfully.";

    MainWindow window;
    window.resize(900, 700);
    window.show();

    return app.exec();
}