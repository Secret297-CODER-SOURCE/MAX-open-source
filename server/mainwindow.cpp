#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("ServerHost — Control Panel");
    setMinimumSize(860, 560);

    // ── Central widget ──────────────────────────────────────────────
    QWidget *central = new QWidget(this);
    central->setObjectName("centralBg");
    setCentralWidget(central);

    QHBoxLayout *rootLayout = new QHBoxLayout(central);
    rootLayout->setContentsMargins(16, 16, 16, 16);
    rootLayout->setSpacing(14);

    // ── LEFT PANEL ──────────────────────────────────────────────────
    QWidget *leftPanel = new QWidget;
    leftPanel->setObjectName("sidePanel");
    leftPanel->setFixedWidth(260);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(12, 12, 12, 12);
    leftLayout->setSpacing(10);

    // Server title
    QLabel *titleLabel = new QLabel("SERVER HOST");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);
    leftLayout->addWidget(titleLabel);

    // Status row
    QWidget *statusRow = new QWidget;
    QHBoxLayout *statusLayout = new QHBoxLayout(statusRow);
    statusLayout->setContentsMargins(0,0,0,0);
    statusLayout->setSpacing(8);
    lblStatus = new QLabel("● OFFLINE");
    lblStatus->setObjectName("statusOffline");
    statusLayout->addWidget(lblStatus);
    statusLayout->addStretch();
    lblClientCount = new QLabel("0 clients");
    lblClientCount->setObjectName("clientCountLabel");
    statusLayout->addWidget(lblClientCount);
    leftLayout->addWidget(statusRow);

    // Toggle button
    btnToggle = new QPushButton("START SERVER");
    btnToggle->setObjectName("btnStart");
    btnToggle->setFixedHeight(44);
    leftLayout->addWidget(btnToggle);

    // Separator
    QFrame *sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    sep->setObjectName("separator");
    leftLayout->addWidget(sep);

    // Clients label
    QLabel *clientsHeader = new QLabel("CONNECTED CLIENTS");
    clientsHeader->setObjectName("sectionHeader");
    leftLayout->addWidget(clientsHeader);

    // Client list
    clientListWidget = new QListWidget;
    clientListWidget->setObjectName("clientList");
    clientListWidget->setSelectionMode(QAbstractItemView::NoSelection);
    leftLayout->addWidget(clientListWidget, 1);

    // ── RIGHT PANEL ─────────────────────────────────────────────────
    QWidget *rightPanel = new QWidget;
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(10);

    // Activity log header
    QLabel *logHeader = new QLabel("ACTIVITY LOG");
    logHeader->setObjectName("sectionHeader");
    rightLayout->addWidget(logHeader);

    // Log view
    logView = new QTextEdit;
    logView->setObjectName("logView");
    logView->setReadOnly(true);
    logView->setLineWrapMode(QTextEdit::WidgetWidth);
    rightLayout->addWidget(logView, 1);

    // ── Assemble root ────────────────────────────────────────────────
    rootLayout->addWidget(leftPanel);
    rootLayout->addWidget(rightPanel, 1);

    // ── Connections ──────────────────────────────────────────────────
    connect(btnToggle, &QPushButton::clicked, this, &MainWindow::onToggleServer);
    connect(&server, &ServerHost::clientListChanged, this, &MainWindow::refreshClientList);
    connect(&server, &ServerHost::serverLogMessage, this, &MainWindow::appendLog);

    appendLog("[SYSTEM] Server ready. Press START to begin listening on :8888");
}

void MainWindow::onToggleServer() {
    if (!serverRunning) {
        server.start();
        serverRunning = true;
        btnToggle->setText("STOP SERVER");
        btnToggle->setObjectName("btnStop");
        btnToggle->style()->unpolish(btnToggle);
        btnToggle->style()->polish(btnToggle);
        lblStatus->setText("● ONLINE");
        lblStatus->setObjectName("statusOnline");
        lblStatus->style()->unpolish(lblStatus);
        lblStatus->style()->polish(lblStatus);
        appendLog("[SYSTEM] Server started on port 8888");
    } else {
        server.stop();
        serverRunning = false;
        btnToggle->setText("START SERVER");
        btnToggle->setObjectName("btnStart");
        btnToggle->style()->unpolish(btnToggle);
        btnToggle->style()->polish(btnToggle);
        lblStatus->setText("● OFFLINE");
        lblStatus->setObjectName("statusOffline");
        lblStatus->style()->unpolish(lblStatus);
        lblStatus->style()->polish(lblStatus);
        appendLog("[SYSTEM] Server stopped");
        refreshClientList();
    }
}

void MainWindow::refreshClientList() {
    clientListWidget->clear();
    char** list = server.getClientList();
    int count = 0;
    for (int i = 0; list[i] != nullptr; i++) {
        // Parse "id | username | ip" for nicer display
        QString raw = QString::fromUtf8(list[i]);
        QStringList parts = raw.split(" | ");
        QString display;
        if (parts.size() >= 3)
            display = QString("ID:%1\n%2\n%3").arg(parts[0].trimmed(), parts[1].trimmed(), parts[2].trimmed());
        else
            display = raw;

        QListWidgetItem *item = new QListWidgetItem(display);
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        clientListWidget->addItem(item);
        delete[] list[i];
        count++;
    }
    delete[] list;

    lblClientCount->setText(QString::number(count) + (count == 1 ? " client" : " clients"));
}

void MainWindow::appendLog(const QString& msg) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    logView->append(QString("<span style='color:#555e7a;'>[%1]</span> %2").arg(timestamp, msg));
}

MainWindow::~MainWindow() {
    delete ui;
}
