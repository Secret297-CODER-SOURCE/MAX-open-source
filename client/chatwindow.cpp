#include "chatwindow.h"
#include <QFile>
#include <QDebug>

ChatWindow::ChatWindow(clientSocket *client, QWidget *parent)
    : QMainWindow(parent)
{
    qDebug() << "Initializing ChatWindow instance...";
    setAttribute(Qt::WA_DeleteOnClose);
    m_client = client;
    setWindowTitle("Messenger");
    setFixedSize(WIN_W, WIN_H);
    m_myName = m_client->GetName();

    setupUi();

    connect(m_client, &clientSocket::idReceived, this, [this](int realId) {
        qDebug() << "ChatWindow received real ID via signal:" << realId;
        m_myId = QString::number(realId);
        if (m_infoIp != nullptr) {
            m_infoIp->setText("id:      " + m_myId);
        }
    });

    connect(m_client, &clientSocket::disconnected, this, [this]() {
        qWarning() << "Client disconnected signal caught in ChatWindow. Closing window.";
        this->close();
    });
    connect(m_client, &clientSocket::messageReceived,
            this, [this](const QString &from, const QString &content) {
                appendMessage(from, content, false);
            });
}

void ChatWindow::loadStyleSheet()
{
    QFile file(":/styles/server_style.qss");

    if (file.open(QFile::ReadOnly | QFile::Text)) {
        this->setStyleSheet(file.readAll());
        qDebug() << "ChatWindow stylesheet loaded successfully.";
    } else {
        qWarning() << "Failed to load ChatWindow stylesheet from path: :/styles/server_style.qss";
    }
}

void ChatWindow::setupUi()
{
    qDebug() << "Setting up ChatWindow UI components...";
    loadStyleSheet();
    QWidget *central = new QWidget(this);
    central->setObjectName("centralWidget");
    setCentralWidget(central);

    QHBoxLayout *root = new QHBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    buildLeftPanel();
    buildRightPanel();

    root->addWidget(m_leftPanel);
    root->addWidget(m_rightPanel, 1);
    qDebug() << "ChatWindow UI layout assembly complete.";
}

void ChatWindow::onDisconnectClicked() {
    qDebug() << "Disconnect clicked from menu button.";
    m_client->DisconnectClient();
}

void ChatWindow::buildLeftPanel()
{
    qDebug() << "Building left control panel...";
    m_leftPanel = new QWidget();
    m_leftPanel->setObjectName("leftPanel");
    m_leftPanel->setFixedWidth(230);

    QVBoxLayout *layout = new QVBoxLayout(m_leftPanel);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    //search
    QWidget *topBar = new QWidget();
    topBar->setObjectName("topBar");
    topBar->setFixedHeight(48);

    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(8, 4, 8, 4);
    topLayout->setSpacing(8);

    m_menuBtn = new QPushButton("☰");
    m_menuBtn->setObjectName("menuBtn");
    m_menuBtn->setFixedSize(36, 36);
    m_menuBtn->setCursor(Qt::PointingHandCursor);

    m_searchEdit = new QLineEdit();
    m_searchEdit->setObjectName("searchEdit");
    m_searchEdit->setPlaceholderText("search...");
    m_searchEdit->setFixedHeight(32);

    topLayout->addWidget(m_menuBtn);
    topLayout->addWidget(m_searchEdit, 1);

    buildMenuPanel();

    m_chatsContainer = new QWidget();
    QVBoxLayout *chatsOuter = new QVBoxLayout(m_chatsContainer);
    chatsOuter->setContentsMargins(0, 0, 0, 0);

    m_noChatsLabel = new QLabel("There are no chats yet");
    m_noChatsLabel->setObjectName("noChatsLabel");
    m_noChatsLabel->setAlignment(Qt::AlignCenter);

    m_chatsLayout = new QVBoxLayout();
    m_chatsLayout->setSpacing(2);
    m_chatsLayout->addStretch();

    chatsOuter->addWidget(m_noChatsLabel, 0, Qt::AlignCenter);
    chatsOuter->addLayout(m_chatsLayout);
    chatsOuter->addStretch();

    layout->addWidget(topBar);
    layout->addWidget(m_menuFrame);
    layout->addWidget(m_chatsContainer, 1);

    connect(m_menuBtn, &QPushButton::clicked, this, &ChatWindow::toggleMenu);
}

void ChatWindow::buildMenuPanel()
{
    qDebug() << "Building menu stacked widgets and panels...";
    m_menuFrame = new QFrame();
    m_menuFrame->setObjectName("menuFrame");
    m_menuFrame->setVisible(false);

    QVBoxLayout *frameLayout = new QVBoxLayout(m_menuFrame);
    frameLayout->setContentsMargins(0, 0, 0, 0);
    frameLayout->setSpacing(0);

    m_menuStack = new QStackedWidget();
    frameLayout->addWidget(m_menuStack);

    QWidget *mainMenu = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(mainMenu);
    mainLayout->setContentsMargins(12, 10, 12, 10);
    mainLayout->setSpacing(2);

    auto makeMenuItem = [&](const QString &text) -> QPushButton* {
        QPushButton *btn = new QPushButton(text);
        btn->setObjectName("menuItem");
        btn->setFixedHeight(32);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFlat(true);
        return btn;
    };

    QPushButton *yourInfoBtn    = makeMenuItem("Your info");
    QPushButton *findPartnerBtn = makeMenuItem("Find new partner");
    QPushButton *endConvBtn     = makeMenuItem("End the conversation");
    QPushButton *rebootBtn      = makeMenuItem("Exit");
    QPushButton *changeThemeBtn = makeMenuItem("Change the theme");

    mainLayout->addWidget(yourInfoBtn);
    mainLayout->addWidget(findPartnerBtn);
    mainLayout->addWidget(endConvBtn);
    mainLayout->addWidget(rebootBtn);
    mainLayout->addWidget(changeThemeBtn);

    m_menuStack->addWidget(mainMenu);

    //yourinfo
    QWidget *infoPage = new QWidget();
    QVBoxLayout *infoLayout = new QVBoxLayout(infoPage);
    infoLayout->setContentsMargins(12, 10, 12, 10);
    infoLayout->setSpacing(4);

    QPushButton *infoBack = new QPushButton("← Your info");
    infoBack->setObjectName("menuItem");
    infoBack->setFlat(true);
    infoBack->setCursor(Qt::PointingHandCursor);

    m_infoName = new QLabel("name:  " + m_myName);
    m_infoName->setObjectName("infoLabel");
    m_infoIp   = new QLabel("id:      " + m_myId);
    m_infoIp->setObjectName("infoLabel");

    infoLayout->addWidget(infoBack);
    infoLayout->addSpacing(4);
    infoLayout->addWidget(m_infoName);
    infoLayout->addWidget(m_infoIp);

    m_menuStack->addWidget(infoPage);

    //findpartner
    QWidget *findPage = new QWidget();
    QVBoxLayout *findLayout = new QVBoxLayout(findPage);
    findLayout->setContentsMargins(12, 10, 12, 10);
    findLayout->setSpacing(6);

    QPushButton *findBack = new QPushButton("← Find new partner");
    findBack->setObjectName("menuItem");
    findBack->setFlat(true);
    findBack->setCursor(Qt::PointingHandCursor);

    auto makeField = [&](const QString &labelText, QLineEdit *&edit) {
        QLabel *lbl = new QLabel(labelText);
        lbl->setObjectName("fieldSmallLabel");
        edit = new QLineEdit();
        edit->setObjectName("fieldSmallEdit");
        edit->setFixedHeight(26);
        findLayout->addWidget(lbl);
        findLayout->addWidget(edit);
    };

    findLayout->addWidget(findBack);
    makeField("name:", m_partnerNameEdit);
    makeField("ip:",   m_partnerIpEdit);
    makeField("port:", m_partnerPortEdit);

    QPushButton *connectBtn = new QPushButton("Connect");
    connectBtn->setObjectName("connectSmallBtn");
    connectBtn->setFixedHeight(28);
    connectBtn->setCursor(Qt::PointingHandCursor);
    findLayout->addWidget(connectBtn, 0, Qt::AlignHCenter);

    m_menuStack->addWidget(findPage);

    //changetheme
    QWidget *themePage = new QWidget();
    QVBoxLayout *themeLayout = new QVBoxLayout(themePage);
    themeLayout->setContentsMargins(12, 10, 12, 10);
    themeLayout->setSpacing(8);

    QPushButton *themeBack = new QPushButton("← Change the theme");
    themeBack->setObjectName("menuItem");
    themeBack->setFlat(true);
    themeBack->setCursor(Qt::PointingHandCursor);

    themeLayout->addWidget(themeBack);

    QHBoxLayout *colorsLayout = new QHBoxLayout();
    colorsLayout->setSpacing(6);

    QStringList colors = {"#ffffff","#ff6b9d","#7c6af7","#4caf50",
                          "#f0c040","#e53935","#64b5f6","#333333"};
    for (const QString &color : colors) {
        QPushButton *circle = new QPushButton();
        circle->setFixedSize(22, 22);
        circle->setCursor(Qt::PointingHandCursor);
        circle->setStyleSheet(QString(
                                  "QPushButton { background-color: %1; border-radius: 11px; border: none; }"
                                  "QPushButton:hover { border: 2px solid white; }"
                                  ).arg(color));
        colorsLayout->addWidget(circle);
    }
    colorsLayout->addStretch();
    themeLayout->addLayout(colorsLayout);

    m_menuStack->addWidget(themePage);

    connect(yourInfoBtn,    &QPushButton::clicked, this, &ChatWindow::showYourInfo);
    connect(findPartnerBtn, &QPushButton::clicked, this, &ChatWindow::showFindPartner);
    connect(changeThemeBtn, &QPushButton::clicked, this, &ChatWindow::showChangeTheme);
    connect(endConvBtn,     &QPushButton::clicked, this, &ChatWindow::hideMenu);
    connect(rebootBtn,      &QPushButton::clicked, this, &ChatWindow::onDisconnectClicked);

    connect(infoBack,  &QPushButton::clicked, [=](){ m_menuStack->setCurrentIndex(0); });
    connect(findBack,  &QPushButton::clicked, [=](){ m_menuStack->setCurrentIndex(0); });
    connect(themeBack, &QPushButton::clicked, [=](){ m_menuStack->setCurrentIndex(0); });
    connect(connectBtn, &QPushButton::clicked, this, &ChatWindow::onConnectClicked);
}

void ChatWindow::buildRightPanel()
{
    qDebug() << "Building right chat display panel...";
    m_rightPanel = new QWidget();
    m_rightPanel->setObjectName("rightPanel");

    QVBoxLayout *layout = new QVBoxLayout(m_rightPanel);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QFrame *header = new QFrame();
    header->setObjectName("chatHeader");
    header->setFixedHeight(48);

    QHBoxLayout *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(16, 0, 16, 0);

    m_chatHeader = new QLabel();
    m_chatHeader->setObjectName("chatHeaderLabel");

    headerLayout->addWidget(m_chatHeader);
    headerLayout->addStretch();

    m_messagesArea = new QScrollArea();
    m_messagesArea->setObjectName("messagesArea");
    m_messagesArea->setWidgetResizable(true);
    m_messagesArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *messagesContainer = new QWidget();
    messagesContainer->setObjectName("messagesContainer");

    m_messagesLayout = new QVBoxLayout(messagesContainer);
    m_messagesLayout->setContentsMargins(16, 16, 16, 16);
    m_messagesLayout->setSpacing(8);
    m_messagesLayout->addStretch();

    m_messagesArea->setWidget(messagesContainer);

    //input
    QWidget *inputBar = new QWidget();
    inputBar->setObjectName("inputBar");
    inputBar->setFixedHeight(56);

    QHBoxLayout *inputLayout = new QHBoxLayout(inputBar);
    inputLayout->setContentsMargins(12, 8, 12, 8);
    inputLayout->setSpacing(8);

    m_messageInput = new QLineEdit();
    m_messageInput->setObjectName("messageInput");
    m_messageInput->setPlaceholderText("Enter text...");
    m_messageInput->setFixedHeight(38);

    m_sendBtn = new QPushButton("➜");
    m_sendBtn->setObjectName("sendBtn");
    m_sendBtn->setFixedSize(38, 38);
    m_sendBtn->setCursor(Qt::PointingHandCursor);
    auto doSend = [this]() {
        QString text = m_messageInput->text().trimmed();
        if (text.isEmpty() || m_partnerId == -1) return;
        appendMessage(m_myName, text, true);
        m_client->SendDirectMessage(m_partnerId, text);
        m_messageInput->clear();
    };
    connect(m_sendBtn,       &QPushButton::clicked,    this, doSend);
    connect(m_messageInput,  &QLineEdit::returnPressed, this, doSend);

    inputLayout->addWidget(m_messageInput, 1);
    inputLayout->addWidget(m_sendBtn);

    layout->addWidget(header);
    layout->addWidget(m_messagesArea, 1);
    layout->addWidget(inputBar);
}

void ChatWindow::toggleMenu()
{
    bool visible = m_menuFrame->isVisible();
    qDebug() << "Toggling menu panel visibility from" << visible << "to" << !visible;
    m_menuFrame->setVisible(!visible);
    if (!visible)
        m_menuStack->setCurrentIndex(0);
}

void ChatWindow::hideMenu()
{
    qDebug() << "Hiding menu frame.";
    m_menuFrame->setVisible(false);
}

void ChatWindow::showYourInfo()
{
    qDebug() << "Navigating menu stack to 'Your Info' page.";
    m_infoName->setText("name:  " + m_myName);
    m_infoIp->setText("id:      " + m_myId);
    m_menuStack->setCurrentIndex(1);
}

void ChatWindow::showFindPartner()
{
    qDebug() << "Navigating menu stack to 'Find Partner' page.";
    m_menuStack->setCurrentIndex(2);
}

void ChatWindow::showChangeTheme()
{
    qDebug() << "Navigating menu stack to 'Change Theme' page.";
    m_menuStack->setCurrentIndex(3);
}

void ChatWindow::onConnectClicked() {
    bool ok;
    int id = m_partnerIpEdit->text().trimmed().toInt(&ok);
    if (!ok || id <= 0) return;
    m_partnerId   = id;
    m_partnerName = m_partnerNameEdit->text().trimmed();
    m_chatHeader->setText(m_partnerName.isEmpty()
                              ? "ID: " + QString::number(id)
                              : m_partnerName);
    hideMenu();
}
void ChatWindow::appendMessage(const QString &from,const QString &text,bool isMine)
{
    QWidget *row = new QWidget();
    QHBoxLayout *rowLayout = new QHBoxLayout(row);
    rowLayout->setContentsMargins(0, 2, 0, 2);

    QWidget *bubble = new QWidget();
    bubble->setObjectName(isMine ? "bubbleMine" : "bubbleTheirs");
    bubble->setMaximumWidth(500);

    QVBoxLayout *bl = new QVBoxLayout(bubble);
    bl->setContentsMargins(12, 8, 12, 6);
    bl->setSpacing(2);

    if (!isMine) {
        QLabel *nameLabel = new QLabel(from);
        nameLabel->setObjectName("bubbleSender");
        bl->addWidget(nameLabel);
    }
    QLabel *textLabel = new QLabel(text);
    textLabel->setObjectName("bubbleText");
    textLabel->setWordWrap(true);
    bl->addWidget(textLabel);

    QLabel *timeLabel = new QLabel(QTime::currentTime().toString("hh:mm"));
    timeLabel->setObjectName("bubbleTime");
    timeLabel->setAlignment(Qt::AlignRight);
    bl->addWidget(timeLabel);

    if (isMine) { rowLayout->addStretch(); rowLayout->addWidget(bubble); }
    else        { rowLayout->addWidget(bubble); rowLayout->addStretch(); }

    m_messagesLayout->insertWidget(m_messagesLayout->count() - 1, row);
    QApplication::processEvents();
    m_messagesArea->verticalScrollBar()->setValue(
        m_messagesArea->verticalScrollBar()->maximum());
}