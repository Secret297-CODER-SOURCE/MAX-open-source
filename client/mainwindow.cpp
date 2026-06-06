#include "mainwindow.h"

#include <QFile>
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    loadStyleSheet();

    setWindowTitle("Log in");
    setMinimumSize(400, 520);

    if (auto *screen = QApplication::primaryScreen()) {
        const QRect sg = screen->availableGeometry();
        move(sg.center() - rect().center());
    }
}

void MainWindow::refreshUI()
{
    if (m_isLoginMode)
    {
        m_titleLabel->setText("Log in");
        m_nameLabel->setText("ID:");

        m_nameEdit->clear();
        m_nameEdit->setEnabled(true);
        m_nameEdit->setPlaceholderText("Enter your ID");

        if (Fsystem::hasFile())
        {
            int id = Fsystem::loadSavedId();
            m_nameEdit->setText(QString::number(id));
        }

        m_connectButton->setText("Connect");

        m_loginModeButton->setEnabled(false);
        m_registerModeButton->setEnabled(true);
    }
    else
    {
        m_titleLabel->setText("Register");
        m_nameLabel->setText("Nickname:");

        m_nameEdit->clear();
        m_nameEdit->setEnabled(true);
        m_nameEdit->setPlaceholderText("Enter your nickname");

        m_connectButton->setText("Register & Connect");

        m_loginModeButton->setEnabled(true);
        m_registerModeButton->setEnabled(false);
    }
}

void MainWindow::setupUi()
{
    m_centralWidget = new QWidget(this);
    m_centralWidget->setObjectName("centralWidget");
    setCentralWidget(m_centralWidget);
    auto *rootLayout = new QVBoxLayout(m_centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setAlignment(Qt::AlignCenter);

    m_card = new QWidget(m_centralWidget);
    m_card->setObjectName("card");
    m_card->setFixedWidth(520);

    auto *shadow = new QGraphicsDropShadowEffect(m_card);
    shadow->setBlurRadius(48);
    shadow->setOffset(0, 8);
    shadow->setColor(QColor(0, 0, 0, 180));
    m_card->setGraphicsEffect(shadow);

    m_cardLayout = new QVBoxLayout(m_card);
    m_cardLayout->setContentsMargins(52, 48, 52, 52);
    m_cardLayout->setSpacing(0);

    auto *modeLayout = new QHBoxLayout();
    m_loginModeButton = new QPushButton("Login", m_card);
    m_registerModeButton = new QPushButton("Register", m_card);

    m_loginModeButton->setObjectName("modeButton");
    m_registerModeButton->setObjectName("modeButton");
    m_loginModeButton->setCursor(Qt::PointingHandCursor);
    m_registerModeButton->setCursor(Qt::PointingHandCursor);

    modeLayout->addWidget(m_loginModeButton);
    modeLayout->addWidget(m_registerModeButton);
    m_cardLayout->addLayout(modeLayout);
    m_cardLayout->addSpacing(20);

    connect(m_loginModeButton, &QPushButton::clicked, this, [this]() {
        m_isLoginMode = true;
        refreshUI();
    });

    connect(m_registerModeButton, &QPushButton::clicked, this, [this]() {
        m_isLoginMode = false;
        refreshUI();
    });

    m_titleLabel = new QLabel(m_card);
    m_titleLabel->setObjectName("titleLabel");
    m_titleLabel->setAlignment(Qt::AlignHCenter);
    m_cardLayout->addWidget(m_titleLabel);

    m_nameLabel = new QLabel("name:", m_card);
    m_nameLabel->setObjectName("fieldLabel");
    m_cardLayout->addWidget(m_nameLabel);
    m_cardLayout->addSpacing(6);

    m_nameEdit = new QLineEdit(m_card);
    m_nameEdit->setObjectName("fieldEdit");
    m_nameEdit->setFixedHeight(52);
    m_cardLayout->addWidget(m_nameEdit);

    m_ipLabel = new QLabel("ip:", m_card);
    m_ipLabel->setObjectName("fieldLabel");
    m_cardLayout->addWidget(m_ipLabel);
    m_cardLayout->addSpacing(6);

    m_ipEdit = new QLineEdit(m_card);
    m_ipEdit->setObjectName("fieldEdit");
    m_ipEdit->setFixedHeight(52);
    m_cardLayout->addWidget(m_ipEdit);
    m_cardLayout->addSpacing(18);

    m_portLabel = new QLabel("port:", m_card);
    m_portLabel->setObjectName("fieldLabel");
    m_cardLayout->addWidget(m_portLabel);
    m_cardLayout->addSpacing(6);

    m_portEdit = new QLineEdit(m_card);
    m_portEdit->setObjectName("fieldEdit");
    m_portEdit->setFixedHeight(52);
    m_cardLayout->addWidget(m_portEdit);
    m_cardLayout->addSpacing(28);

    m_connectButton = new QPushButton("Connect", m_card);
    m_connectButton->setObjectName("connectButton");
    m_connectButton->setFixedHeight(56);
    m_connectButton->setCursor(Qt::PointingHandCursor);
    m_cardLayout->addWidget(m_connectButton);

    rootLayout->addWidget(m_card, 0, Qt::AlignCenter);

    refreshUI();

    connect(m_connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
}

void MainWindow::loadStyleSheet()
{
    QFile file(":/styles/mainwindow.qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        this->setStyleSheet(file.readAll());
    } else {
        qWarning() << "Failed to load stylesheet from path: :/styles/mainwindow.qss";
    }
}

void MainWindow::OnConnected()
{

    ChatWindow *chat = new ChatWindow(client);
    client->setParent(chat);
    client = nullptr;

    chat->setAttribute(Qt::WA_DeleteOnClose);

    connect(chat, &QObject::destroyed, this, [this]() {
        this->show();
        this->refreshUI();
    });

    chat->show();
    this->hide();
}

void MainWindow::onConnectClicked()
{
    const QString ip = m_ipEdit->text().trimmed();
    const QString portStr = m_portEdit->text().trimmed();
    const QString value = m_nameEdit->text().trimmed();

    if (ip.isEmpty() || portStr.isEmpty() || value.isEmpty())
    {
        QMessageBox::warning(this, "Missing fields", "Please fill all fields.");
        return;
    }

    bool portOk;
    quint16 port = portStr.toUShort(&portOk);
    if (!portOk || port == 0)
    {
        QMessageBox::warning(this, "Invalid Port", "Please enter a valid port number (1-65535).");
        return;
    }
    if (client)
    {
        client->disconnect(this);
        client->deleteLater();
        client = nullptr;
    }

    if (m_isLoginMode)
    {
        bool idOk;
        int id = value.toInt(&idOk);
        if (!idOk)
        {
            QMessageBox::warning(this, "Invalid ID", "Please enter a valid numeric ID.");
            return;
        }
        client = new clientSocket(id, this);
    }
    else
    {
        client = new clientSocket(value, this);
    }

    connect(client, &clientSocket::ConnectError, this, [this]() {
        QMessageBox::critical(this, "Error", "Server unavailable");
        // Очищаем client при ошибке подключения
        if (client) {
            client->deleteLater();
            client = nullptr;
        }
    });

    connect(client, &clientSocket::connected, this, &MainWindow::OnConnected);

    client->ConnectClient(ip, port);
}