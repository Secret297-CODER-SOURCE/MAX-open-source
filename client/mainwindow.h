#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include "clientsocket.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onConnectClicked();

private:
    void setupUi();
    void loadStyleSheet();
    clientSocket *client;
    QWidget     *m_centralWidget;
    QWidget     *m_card;
    QVBoxLayout *m_cardLayout;

    QLabel      *m_titleLabel;

    QLabel      *m_nameLabel;
    QLineEdit   *m_nameEdit;

    QLabel      *m_ipLabel;
    QLineEdit   *m_ipEdit;

    QLabel      *m_portLabel;
    QLineEdit   *m_portEdit;

    QPushButton *m_connectButton;
};
