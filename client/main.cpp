#include <QApplication>
#include "mainwindow.h"
#include <QFile>

int main(int argc, char *argv[])
{
    qputenv("QT_ASSUME_STDERR_HAS_CONSOLE", "1");

    QApplication app(argc, argv);
    app.setApplicationName("Login");
    MainWindow window;
    window.resize(900, 700);
    window.show();

    return app.exec();
}