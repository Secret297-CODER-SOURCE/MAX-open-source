#ifndef CHATWINDOW_H
#define CHATWINDOW_H
#include "clientsocket.h"
#include <QMainWindow>
#include <QPushButton>
#include <QApplication>
class ChatWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChatWindow(clientSocket *client, QWidget *parent = nullptr);

private slots:
    void onDisconnectClicked();
signals:
    void closedByRequest();
private:
    QPushButton *disconnectBtn;
    clientSocket *m_client;
    void initUI();
    void loadStyleSheet();
};

#endif // CHATWINDOW_H
