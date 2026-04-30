#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QWidget>
#include "serverhost.h"
#include <QFrame>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QList>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void LogConnectionBlock();
    Ui::MainWindow *ui;
    ServerHost server;
    QVBoxLayout *layout;
    QList<QLabel*> iplabels;
};
#endif // MAINWINDOW_H
