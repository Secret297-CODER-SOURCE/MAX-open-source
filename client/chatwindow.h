#ifndef CHATWINDOW_H
#define CHATWINDOW_H
#include "clientsocket.h"
#include <QMainWindow>
#include <QPushButton>
#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QScrollArea>
#include <QFrame>
#include <QStackedWidget>
#include <QFile>
#include <QNetworkInterface>
#include <QList>
#include <QHostAddress>
#include <QDebug>

static constexpr int WIN_W = 1109;
static constexpr int WIN_H = 670;

class ChatWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChatWindow(clientSocket *client, QWidget *parent = nullptr);
private slots:
    void toggleMenu();
    void showYourInfo();
    void showFindPartner();
    void showChangeTheme();

    void onDisconnectClicked();
    void hideMenu();
signals:
    void closedByRequest();
private:
    clientSocket *m_client;
    void setupUi();
    void buildLeftPanel();
    void buildRightPanel();
    void buildMenuPanel();
    void loadStyleSheet();
    void onConnectClicked();
    QString printLocalIPAddresses();
    QWidget        *m_leftPanel;
    QPushButton    *m_menuBtn;
    QLineEdit      *m_searchEdit;
    QLabel         *m_noChatsLabel;
    QWidget        *m_chatsContainer;
    QVBoxLayout    *m_chatsLayout;

    QFrame         *m_menuFrame;
    QStackedWidget *m_menuStack;

    QLabel         *m_infoName;
    QLabel         *m_infoIp;

    QLineEdit      *m_partnerNameEdit;
    QLineEdit      *m_partnerIpEdit;
    QLineEdit      *m_partnerPortEdit;

    QWidget        *m_rightPanel;
    QLabel         *m_chatHeader;
    QScrollArea    *m_messagesArea;
    QVBoxLayout    *m_messagesLayout;
    QLineEdit      *m_messageInput;
    QPushButton    *m_sendBtn;

    QString        m_myName;
    QString        m_myId;
    QString        m_partnerName;
    QString        m_partnerIp;

};

#endif // CHATWINDOW_H
