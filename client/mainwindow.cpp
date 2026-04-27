#include "mainwindow.h"

#include <QFile>
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    loadStyleSheet();

    setWindowTitle("Log in");
    setMinimumSize(400, 520);
    // Center on screen
    if (auto *screen = QApplication::primaryScreen()) {
        const QRect sg = screen->availableGeometry();
        move(sg.center() - rect().center());
    }
}

void MainWindow::setupUi()
{
    // ── Root ──────────────────────────────────────────────────────────────
    m_centralWidget = new QWidget(this);
    m_centralWidget->setObjectName("centralWidget");
    setCentralWidget(m_centralWidget);

    auto *rootLayout = new QVBoxLayout(m_centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setAlignment(Qt::AlignCenter);

    // ── Card ──────────────────────────────────────────────────────────────
    m_card = new QWidget(m_centralWidget);
    m_card->setObjectName("card");
    m_card->setFixedWidth(520);

    // Soft drop-shadow on the card
    auto *shadow = new QGraphicsDropShadowEffect(m_card);
    shadow->setBlurRadius(48);
    shadow->setOffset(0, 8);
    shadow->setColor(QColor(0, 0, 0, 180));
    m_card->setGraphicsEffect(shadow);

    m_cardLayout = new QVBoxLayout(m_card);
    m_cardLayout->setContentsMargins(52, 48, 52, 52);
    m_cardLayout->setSpacing(0);

    // ── Title ─────────────────────────────────────────────────────────────
    m_titleLabel = new QLabel("Log in", m_card);
    m_titleLabel->setObjectName("titleLabel");
    m_titleLabel->setAlignment(Qt::AlignHCenter);
    m_cardLayout->addWidget(m_titleLabel);
    m_cardLayout->addSpacing(32);

    // ── Name field ────────────────────────────────────────────────────────
    m_nameLabel = new QLabel("name:", m_card);
    m_nameLabel->setObjectName("fieldLabel");
    m_cardLayout->addWidget(m_nameLabel);
    m_cardLayout->addSpacing(6);

    m_nameEdit = new QLineEdit(m_card);
    m_nameEdit->setObjectName("fieldEdit");
    m_nameEdit->setFixedHeight(52);
    m_cardLayout->addWidget(m_nameEdit);
    m_cardLayout->addSpacing(18);

    // ── IP field ──────────────────────────────────────────────────────────
    m_ipLabel = new QLabel("ip:", m_card);
    m_ipLabel->setObjectName("fieldLabel");
    m_cardLayout->addWidget(m_ipLabel);
    m_cardLayout->addSpacing(6);

    m_ipEdit = new QLineEdit(m_card);
    m_ipEdit->setObjectName("fieldEdit");
    m_ipEdit->setFixedHeight(52);
    m_cardLayout->addWidget(m_ipEdit);
    m_cardLayout->addSpacing(18);

    // ── Port field ────────────────────────────────────────────────────────
    m_portLabel = new QLabel("port:", m_card);
    m_portLabel->setObjectName("fieldLabel");
    m_cardLayout->addWidget(m_portLabel);
    m_cardLayout->addSpacing(6);

    m_portEdit = new QLineEdit(m_card);
    m_portEdit->setObjectName("fieldEdit");
    m_portEdit->setFixedHeight(52);
    m_cardLayout->addWidget(m_portEdit);
    m_cardLayout->addSpacing(28);

    // ── Connect button ────────────────────────────────────────────────────
    m_connectButton = new QPushButton("Connect", m_card);
    m_connectButton->setObjectName("connectButton");
    m_connectButton->setFixedHeight(56);
    m_connectButton->setCursor(Qt::PointingHandCursor);
    m_cardLayout->addWidget(m_connectButton);

    rootLayout->addWidget(m_card, 0, Qt::AlignCenter);

    // ── Signals ───────────────────────────────────────────────────────────
    connect(m_connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
}

void MainWindow::loadStyleSheet()
{
    QFile file(":/styles/mainwindow.qss");

    if (file.open(QFile::ReadOnly | QFile::Text)) {
        this->setStyleSheet(file.readAll());
    } else {
        qDebug() << "Failed to load stylesheet";
    }
}
void MainWindow::OnConnected()
{

    ChatWindow *chat = new ChatWindow(client);
    connect(client, &clientSocket::disconnected, this, &MainWindow::show);

    chat->show();
    this->hide();
}
void MainWindow::onConnectClicked()
{
    const QString name = m_nameEdit->text().trimmed();
    const QString ip   = m_ipEdit->text().trimmed();
    const QString port = m_portEdit->text().trimmed();
    if (client) {
        client->deleteLater();
        client = nullptr;
    }
    if (name.isEmpty() || ip.isEmpty() || port.isEmpty()) {
        QMessageBox::warning(this, "Missing fields", "Please fill in all fields before connecting.");
        return;
    }
    client = new clientSocket(name, this);
    connect(client, &clientSocket::connected, this, &MainWindow::OnConnected);
    client->ConnectClient(ip, port.toUInt());
}
