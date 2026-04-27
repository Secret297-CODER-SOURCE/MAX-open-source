#include "chatwindow.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QDebug>
#include <QFile>
ChatWindow::ChatWindow(clientSocket *client, QWidget *parent)
    : QMainWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    m_client = client;
    initUI();
    loadStyleSheet();
    connect(m_client, &clientSocket::disconnected, this, &ChatWindow::close);
    connect(disconnectBtn, &QPushButton::clicked,
            this, &ChatWindow::onDisconnectClicked);


}
void ChatWindow::loadStyleSheet()
{
    QFile file(":/styles/chatwindow.qss");

    if (file.open(QFile::ReadOnly | QFile::Text)) {
        this->setStyleSheet(file.readAll());
    } else {
        qDebug() << "Failed to load stylesheet";
    }
}
void ChatWindow::onDisconnectClicked() {
    m_client->DisconnectClient();
    qDebug() << "Disconnected.";
}
void ChatWindow::initUI() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    disconnectBtn = new QPushButton("Disconnect", this);
    disconnectBtn->setObjectName("disconnectBtn");
    disconnectBtn->setFixedHeight(40);

    mainLayout->addStretch();
    mainLayout->addWidget(disconnectBtn);
}
