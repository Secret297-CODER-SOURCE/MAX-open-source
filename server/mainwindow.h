#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include "serverhost.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QTextEdit>
#include <QFrame>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void refreshClientList();
    void appendLog(const QString& msg);
    void onToggleServer();

private:
    Ui::MainWindow *ui;
    ServerHost server;
    bool serverRunning = false;

    QPushButton *btnToggle;
    QLabel      *lblStatus;
    QLabel      *lblClientCount;
    QListWidget *clientListWidget;
    QTextEdit   *logView;
};

#endif // MAINWINDOW_H
