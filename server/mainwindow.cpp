#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("ServerHost — Control Panel");
    setMinimumSize(960, 600);

    QWidget *central = new QWidget(this);
    central->setObjectName("centralBg");
    setCentralWidget(central);

    QHBoxLayout *rootLayout = new QHBoxLayout(central);
    rootLayout->setContentsMargins(16, 16, 16, 16);
    rootLayout->setSpacing(14);

    QWidget *leftPanel = new QWidget;
    leftPanel->setObjectName("sidePanel");
    leftPanel->setFixedWidth(260);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(12, 12, 12, 12);
    leftLayout->setSpacing(10);

    QLabel *titleLabel = new QLabel("SERVER HOST");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);
    leftLayout->addWidget(titleLabel);

    QWidget *statusRow = new QWidget;
    QHBoxLayout *statusLayout = new QHBoxLayout(statusRow);
    statusLayout->setContentsMargins(0, 0, 0, 0);
    statusLayout->setSpacing(8);
    lblStatus = new QLabel("● OFFLINE");
    lblStatus->setObjectName("statusOffline");
    statusLayout->addWidget(lblStatus);
    statusLayout->addStretch();
    lblClientCount = new QLabel("0 clients");
    lblClientCount->setObjectName("clientCountLabel");
    statusLayout->addWidget(lblClientCount);
    leftLayout->addWidget(statusRow);

    btnToggle = new QPushButton("START SERVER");
    btnToggle->setObjectName("btnStart");
    btnToggle->setFixedHeight(44);
    leftLayout->addWidget(btnToggle);

    QFrame *sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    sep->setObjectName("separator");
    leftLayout->addWidget(sep);

    QLabel *clientsHeader = new QLabel("CONNECTED CLIENTS");
    clientsHeader->setObjectName("sectionHeader");
    leftLayout->addWidget(clientsHeader);

    clientListWidget = new QListWidget;
    clientListWidget->setObjectName("clientList");
    clientListWidget->setSelectionMode(QAbstractItemView::NoSelection);
    leftLayout->addWidget(clientListWidget, 1);

    QWidget *rightPanel = new QWidget;
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(10);

    QWidget *tabBar = new QWidget;
    tabBar->setObjectName("tabBar");
    QHBoxLayout *tabLayout = new QHBoxLayout(tabBar);
    tabLayout->setContentsMargins(0, 0, 0, 0);
    tabLayout->setSpacing(6);

    const QString tabStyle =
        "QPushButton { background:transparent; color:#6e7090; border:1px solid transparent;"
        "              border-radius:6px; padding:3px 12px; margin:0 4px 0 0;"
        "              font-family:'Segoe UI','SF Pro Display',sans-serif;"
        "              font-weight:500; font-size:11px; letter-spacing:0.4px;"
        "              min-width:0; }"
        "QPushButton:hover   { color:#cdd2ff; background:#1f2033; }"
        "QPushButton:checked { color:#ffffff; background:#262844;"
        "                      border:1px solid #3a3d68; font-weight:600; }";

    tabLogsBtn = new QPushButton("LOGS");
    tabLogsBtn->setCheckable(true);
    tabLogsBtn->setChecked(true);
    tabLogsBtn->setCursor(Qt::PointingHandCursor);
    tabLogsBtn->setStyleSheet(tabStyle);

    tabApiBtn = new QPushButton("API");
    tabApiBtn->setCheckable(true);
    tabApiBtn->setCursor(Qt::PointingHandCursor);
    tabApiBtn->setStyleSheet(tabStyle);

    QFrame *tabUnderline = new QFrame;
    tabUnderline->setFixedHeight(1);
    tabUnderline->setStyleSheet("background:#22243a; border:none;");

    tabLayout->addWidget(tabLogsBtn);
    tabLayout->addWidget(tabApiBtn);
    tabLayout->addStretch();

    QWidget *tabContainer = new QWidget;
    QVBoxLayout *tabContainerLayout = new QVBoxLayout(tabContainer);
    tabContainerLayout->setContentsMargins(0, 0, 0, 0);
    tabContainerLayout->setSpacing(6);
    tabContainerLayout->addWidget(tabBar);
    tabContainerLayout->addWidget(tabUnderline);

    rightLayout->addWidget(tabContainer);

    rightStack = new QStackedWidget;

    logView = new QTextEdit;
    logView->setObjectName("logView");
    logView->setReadOnly(true);
    logView->setLineWrapMode(QTextEdit::WidgetWidth);

    apiView = new QTextEdit;
    apiView->setObjectName("apiView");
    apiView->setReadOnly(true);
    apiView->setLineWrapMode(QTextEdit::WidgetWidth);
    apiView->setStyleSheet(
        "QTextEdit { background:#171826; color:#cdd2e8; border:none;"
        "            border-radius:8px; padding:18px 22px; }"
        "QScrollBar:vertical { background:transparent; width:8px; margin:4px; }"
        "QScrollBar::handle:vertical { background:#3a3b5c; border-radius:4px; min-height:30px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }"
    );

    rightStack->addWidget(logView);
    rightStack->addWidget(apiView);

    rightLayout->addWidget(rightStack, 1);

    rootLayout->addWidget(leftPanel);
    rootLayout->addWidget(rightPanel, 1);

    connect(btnToggle, &QPushButton::clicked,        this, &MainWindow::onToggleServer);
    connect(&server, &ServerHost::clientListChanged, this, &MainWindow::refreshClientList);
    connect(&server, &ServerHost::serverLogMessage,  this, &MainWindow::appendLog);
    connect(tabLogsBtn, &QPushButton::clicked, this, &MainWindow::showLogsTab);
    connect(tabApiBtn,  &QPushButton::clicked, this, &MainWindow::showApiTab);

    buildApiDoc();
    appendLog("[SYSTEM] Server ready. Press START to begin listening on :8888");
}

void MainWindow::showLogsTab()
{
    rightStack->setCurrentIndex(0);
    tabLogsBtn->setChecked(true);
    tabApiBtn->setChecked(false);
}

void MainWindow::showApiTab()
{
    rightStack->setCurrentIndex(1);
    tabLogsBtn->setChecked(false);
    tabApiBtn->setChecked(true);
}

static QString endpoint(const QString &method, const QString &methodColor,
                        const QString &name, const QString &summary)
{
    return QString(
        "<div style='margin:14px 0 6px 0;'>"
        "<span style='background:%1; color:#fff; padding:2px 9px; border-radius:4px;"
        "             font-family:Menlo,Consolas,monospace; font-size:11px; font-weight:700;"
        "             letter-spacing:1px;'>%2</span>"
        "&nbsp;&nbsp;<span style='color:#e8eaff; font-weight:600; font-size:14px;"
        "                        font-family:Menlo,Consolas,monospace;'>%3</span>"
        "&nbsp;&nbsp;<span style='color:#8088b0; font-size:12px;'>%4</span>"
        "</div>"
    ).arg(methodColor, method, name.toHtmlEscaped(), summary.toHtmlEscaped());
}

static QString codeBlock(const QString &json)
{
    QString esc = json.toHtmlEscaped();
    esc.replace(QRegularExpression("\"([^\"]+)\":"),
                "<span style='color:#7cdcfe;'>\"\\1\"</span>:");
    esc.replace(QRegularExpression(":\\s*\"([^\"]*)\""),
                ": <span style='color:#c5e478;'>\"\\1\"</span>");
    esc.replace(QRegularExpression(":\\s*(\\d+)"),
                ": <span style='color:#f8a677;'>\\1</span>");
    esc.replace(QRegularExpression(":\\s*(true|false)"),
                ": <span style='color:#f8a677;'>\\1</span>");
    return QString(
        "<div style='background:#0e0f1a; border-left:3px solid #7c8df0;"
        "            border-radius:6px; padding:10px 14px; margin:4px 0 12px 0;"
        "            font-family:Menlo,Consolas,monospace; font-size:12px;"
        "            color:#cdd2e8; white-space:pre;'>%1</div>"
    ).arg(esc);
}

static QString sectionTitle(const QString &t)
{
    return QString(
        "<div style='color:#7c8df0; font-size:11px; font-weight:700; letter-spacing:2px;"
        "            margin:24px 0 8px 0; border-bottom:1px solid #2a2b40;"
        "            padding-bottom:6px; font-family:Segoe UI,SF Pro Display,sans-serif;'>%1</div>"
    ).arg(t);
}

static QString fieldRow(const QString &name, const QString &type, const QString &desc)
{
    return QString(
        "<tr>"
        "<td style='padding:4px 14px 4px 0; color:#7cdcfe; font-family:Menlo,monospace;"
        "           font-size:12px; vertical-align:top;'>%1</td>"
        "<td style='padding:4px 14px 4px 0; color:#c5e478; font-family:Menlo,monospace;"
        "           font-size:12px; vertical-align:top;'>%2</td>"
        "<td style='padding:4px 0; color:#9aa0c8; font-size:12px; vertical-align:top;'>%3</td>"
        "</tr>"
    ).arg(name, type, desc);
}

static QString fieldsTable(const QString &rows)
{
    return QString(
        "<table cellspacing='0' cellpadding='0' style='margin:6px 0 14px 0;"
        "       border-left:1px solid #2a2b40; padding-left:12px;'>"
        "<tr><td style='padding-bottom:4px; color:#5a5f80; font-size:10px;"
        "               letter-spacing:1.5px;'>FIELD</td>"
        "    <td style='padding-bottom:4px; color:#5a5f80; font-size:10px;"
        "               letter-spacing:1.5px;'>TYPE</td>"
        "    <td style='padding-bottom:4px; color:#5a5f80; font-size:10px;"
        "               letter-spacing:1.5px;'>DESCRIPTION</td></tr>"
        "%1</table>"
    ).arg(rows);
}

void MainWindow::buildApiDoc()
{
    QString html;
    html += "<div style='font-family:\"Segoe UI\",\"SF Pro Display\",sans-serif;"
            "            color:#cdd2e8; line-height:1.55;'>";

    html += "<div style='font-size:22px; font-weight:600; color:#ffffff;'>"
            "MAX Messenger Server API</div>";
    html += "<div style='color:#8088b0; font-size:13px; margin-top:4px;'>"
            "Version <span style='color:#c5e478;'>1.0.0</span> "
            "&middot; OpenAPI 3.0.3 &middot; "
            "<span style='font-family:Menlo,monospace; color:#7cdcfe;'>tcp://0.0.0.0:8888</span></div>";
    html += "<div style='color:#9aa0c8; font-size:13px; margin-top:10px;'>"
            "JSON-over-TCP protocol used by the MAX messenger. "
            "Each frame is a single-line JSON object terminated by <code "
            "style='background:#0e0f1a; padding:1px 6px; border-radius:3px; color:#f8a677;'>\\n</code>. "
            "Every request and response is identified by the "
            "<code style='background:#0e0f1a; padding:1px 6px; border-radius:3px; color:#7cdcfe;'>type</code> field."
            "</div>";

    html += sectionTitle("CLIENT → SERVER REQUESTS");

    html += endpoint("REQ", "#3b82f6", "REG", "Register a new user");
    html += fieldsTable(
        fieldRow("type", "string", "Always <b>REG</b>") +
        fieldRow("name", "string", "Desired username, must be non-empty")
    );
    html += codeBlock("{\"type\":\"REG\",\"name\":\"alice\"}");

    html += endpoint("REQ", "#3b82f6", "AUTH", "Authenticate by user id");
    html += fieldsTable(
        fieldRow("type",   "string",  "Always <b>AUTH</b>") +
        fieldRow("userid", "integer", "Existing user id from previous registration")
    );
    html += codeBlock("{\"type\":\"AUTH\",\"userid\":12345678}");

    html += endpoint("REQ", "#3b82f6", "MSG", "Send a direct message");
    html += fieldsTable(
        fieldRow("type",       "string",  "Always <b>MSG</b>") +
        fieldRow("senderName", "string",  "Sender's username") +
        fieldRow("receiverId", "integer", "Target user id") +
        fieldRow("content",    "string",  "Message body, non-empty")
    );
    html += codeBlock("{\"type\":\"MSG\",\"senderName\":\"alice\",\"receiverId\":87654321,\"content\":\"hi\"}");

    html += endpoint("REQ", "#3b82f6", "EXIT", "Disconnect a client");
    html += fieldsTable(
        fieldRow("type", "string", "Always <b>EXIT</b>") +
        fieldRow("name", "string", "Username of the disconnecting client")
    );
    html += codeBlock("{\"type\":\"EXIT\",\"name\":\"alice\"}");

    html += endpoint("REQ", "#3b82f6", "LIST", "Get list of all connected clients");
    html += fieldsTable(fieldRow("type", "string", "Always <b>LIST</b>"));
    html += codeBlock("{\"type\":\"LIST\"}");

    html += endpoint("REQ", "#3b82f6", "USERINFO", "Check whether a user exists and get the username");
    html += fieldsTable(
        fieldRow("type", "string",  "Always <b>USERINFO</b>") +
        fieldRow("id",   "integer", "User id to look up in the database")
    );
    html += codeBlock("{\"type\":\"USERINFO\",\"id\":12345678}");

    html += sectionTitle("SERVER → CLIENT EVENTS");

    html += endpoint("EVT", "#a855f7", "WELCOME", "Sent right after the TCP connection is accepted");
    html += fieldsTable(
        fieldRow("type",   "string",  "Always <b>WELCOME</b>") +
        fieldRow("tempid", "integer", "Temporary id assigned to this connection")
    );
    html += codeBlock("{\"type\":\"WELCOME\",\"tempid\":12345678}");

    html += endpoint("EVT", "#a855f7", "AUTH_OK", "Registration or authentication succeeded");
    html += fieldsTable(
        fieldRow("type", "string",  "Always <b>AUTH_OK</b>") +
        fieldRow("id",   "integer", "Authenticated user id")
    );
    html += codeBlock("{\"type\":\"AUTH_OK\",\"id\":12345678}");

    html += endpoint("EVT", "#ef4444", "AUTH_FAIL", "User id not found in the database");
    html += fieldsTable(fieldRow("type", "string", "Always <b>AUTH_FAIL</b>"));
    html += codeBlock("{\"type\":\"AUTH_FAIL\"}");

    html += endpoint("EVT", "#a855f7", "MSG", "Incoming direct message");
    html += fieldsTable(
        fieldRow("type",       "string", "Always <b>MSG</b>") +
        fieldRow("senderName", "string", "Sender's username") +
        fieldRow("content",    "string", "Message body")
    );
    html += codeBlock("{\"type\":\"MSG\",\"senderName\":\"alice\",\"content\":\"hi\"}");

    html += endpoint("EVT", "#a855f7", "LIST", "Response to a LIST request");
    html += fieldsTable(
        fieldRow("type",    "string",  "Always <b>LIST</b>") +
        fieldRow("clients", "array",   "Array of <i>ClientEntry</i> objects")
    );
    html += codeBlock("{\"type\":\"LIST\",\"clients\":[{\"id\":12345678,\"username\":\"alice\"}]}");

    html += endpoint("EVT", "#a855f7", "USERINFO", "Response to a USERINFO request");
    html += fieldsTable(
        fieldRow("type",     "string",  "Always <b>USERINFO</b>") +
        fieldRow("id",       "integer", "Requested user id") +
        fieldRow("exists",   "boolean", "True if user is registered") +
        fieldRow("username", "string",  "Username, empty when <i>exists</i> is false")
    );
    html += codeBlock("{\"type\":\"USERINFO\",\"id\":12345678,\"exists\":true,\"username\":\"alice\"}");

    html += sectionTitle("SCHEMAS");
    html += "<div style='color:#e8eaff; font-weight:600; margin-top:8px;'>ClientEntry</div>";
    html += fieldsTable(
        fieldRow("id",       "integer", "Unique user identifier") +
        fieldRow("username", "string",  "Display name of the user")
    );
    html += codeBlock("{\"id\":12345678,\"username\":\"alice\"}");

    html += "</div>";

    apiView->setHtml(html);
}


void MainWindow::onToggleServer()
{
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

void MainWindow::refreshClientList()
{
    clientListWidget->clear();
    char** list  = server.getClientList();
    int    count = 0;

    for (int i = 0; list[i] != nullptr; i++) {
        QString raw = QString::fromUtf8(list[i]);
        QStringList parts = raw.split(" | ");
        QString display;
        if (parts.size() >= 3)
            display = QString("ID:%1\n%2\n%3")
                          .arg(parts[0].trimmed(), parts[1].trimmed(), parts[2].trimmed());
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

void MainWindow::appendLog(const QString& msg)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    logView->append(
        QString("<span style='color:#555e7a;'>[%1]</span> %2").arg(timestamp, msg));
}

MainWindow::~MainWindow()
{
    delete ui;
}
