#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile file(":/styles/server_style.qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = file.readAll();
        a.setStyleSheet(styleSheet);
        file.close();
    } else {
        qDebug() << "Failed to open stylesheet!";
    }

    MainWindow w;
    w.show();

    return a.exec();
}
