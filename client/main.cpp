#include "singleapplication.h"
#include "mainwindow.h"
#include <QFile>
#include "logger.h"
#include <iostream>

int main(int argc, char *argv[])
{
    QString appGuid = "messenger.backend.sigmov_messenger";

    SingleApplication app(argc, argv, appGuid);
    app.setApplicationName("Login");

    if (app.isRunning()) {
        return 0;
    }

    if (!logger::init()) {
        std::cerr << "CRITICAL: Failed to initialize logger file!" << std::endl;
        return -1;
    }

    qInstallMessageHandler(logger::messageHandler);
    qInfo() << "Application started successfully.";

    MainWindow window;
    window.resize(900, 700);
    window.show();


    QObject::connect(&app, &SingleApplication::instanceStarted, &window, [&window]() {

        window.setWindowState((window.windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        window.raise();
        window.activateWindow();
    });

    return app.exec();
}