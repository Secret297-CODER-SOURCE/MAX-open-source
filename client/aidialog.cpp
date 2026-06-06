#include "aidialog.h"
#include <QHBoxLayout>
#include <QScrollBar>
#include <QFrame>
#include <QSettings>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

static const char *OPENAI_URL   = "https://api.openai.com/v1/chat/completions";
static const char *OPENAI_MODEL = "gpt-4o-mini";

AiDialog::AiDialog(QWidget *parent)
    : QDialog(parent), m_net(new QNetworkAccessManager(this))
{
    setWindowTitle("AI Assistant");
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(400, 520);

    QFrame *card = new QFrame(this);
    card->setObjectName("aiCard");
    card->setStyleSheet(
        "#aiCard { background:#1a1a1a; border-radius:18px; border:1px solid #2a2a2a; }"
        "QLabel { color:#e0e0e0; font-family:'Segoe UI','SF Pro Display',sans-serif; }"
        "QLabel#aiTitle { color:#ececec; font-size:16px; font-weight:500; padding:2px 4px; }"
        "QLabel#aiHint  { color:#8e8e8e; font-size:13px; }"
        "QLabel#aiError { color:#d97070; font-size:12px; }"
        "QLineEdit { background:#111111; color:#e8e8e8; border:1px solid #1e1e1e;"
        "            border-radius:12px; padding:8px 14px; font-size:14px; }"
        "QLineEdit:focus { border:1px solid #3a3a3a; background:#0e0e0e; }"
        "QPushButton#aiPrimary { background:#161616; color:#e0e0e0; border:1px solid #2a2a2a;"
        "                       border-radius:12px; padding:8px 12px; font-size:14px; }"
        "QPushButton#aiPrimary:hover { background:#1f1f1f; border:1px solid #3a3a3a; color:#fff; }"
        "QPushButton#aiSend { background:#242424; color:#e0e0e0; border:1px solid #2a2a2a;"
        "                     border-radius:12px; font-size:15px; font-weight:500; }"
        "QPushButton#aiSend:hover { background:#2e2e2e; border:1px solid #3a3a3a; color:#fff; }"
        "QPushButton#aiSend:disabled { color:#5a5a5a; background:#1a1a1a; }"
        "QPushButton#aiIcon { background:transparent; color:#8e8e8e; border:none; font-size:13px; font-weight:500; }"
        "QPushButton#aiIcon:hover { color:#fff; }"
        "QPushButton#aiClose { background:transparent; color:#8e8e8e; border:none; font-size:16px; }"
        "QPushButton#aiClose:hover { color:#fff; }"
        "QScrollArea { border:none; background:transparent; }"
        "QWidget#aiBubbleMine { background:#242424; border:1px solid #2a2a2a; border-radius:12px; }"
        "QWidget#aiBubbleAi   { background:#141414; border:1px solid #1f1f1f; border-radius:12px; }"
        "QWidget#aiBubbleErr  { background:#2a1a1a; border:1px solid #3a2222; border-radius:12px; }"
        "QLabel#aiBubbleText { color:#e8e8e8; font-size:13px; background:transparent; }"
    );

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->addWidget(card);

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(12);

    QHBoxLayout *header = new QHBoxLayout();
    QLabel *title = new QLabel("AI Assistant");
    title->setObjectName("aiTitle");
    m_keyBtn = new QPushButton("TOKEN");
    m_keyBtn->setObjectName("aiIcon");
    m_keyBtn->setFixedHeight(26);
    m_keyBtn->setCursor(Qt::PointingHandCursor);
    m_keyBtn->setToolTip("Change API token");
    QPushButton *closeBtn = new QPushButton("×");
    closeBtn->setObjectName("aiClose");
    closeBtn->setFixedSize(26, 26);
    closeBtn->setCursor(Qt::PointingHandCursor);
    header->addWidget(title);
    header->addStretch();
    header->addWidget(m_keyBtn);
    header->addWidget(closeBtn);
    layout->addLayout(header);

    m_stack = new QStackedWidget();
    layout->addWidget(m_stack, 1);
    buildTokenPage();
    buildChatPage();

    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_keyBtn, &QPushButton::clicked, this, &AiDialog::onChangeToken);

    m_token = loadToken();
    if (m_token.isEmpty()) {
        m_stack->setCurrentIndex(0);
        m_keyBtn->setVisible(false);
    } else {
        m_stack->setCurrentIndex(1);
        appendBubble("Hi! I'm connected to ChatGPT. Ask me anything.", false);
    }
}

void AiDialog::buildTokenPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(page);
    l->setContentsMargins(2, 2, 2, 2);
    l->setSpacing(10);

    QLabel *info = new QLabel("Paste your OpenAI API token to start.");
    info->setObjectName("aiHint");
    info->setWordWrap(true);

    QLabel *hint = new QLabel("Get one at platform.openai.com/api-keys");
    hint->setObjectName("aiHint");
    hint->setWordWrap(true);

    m_tokenEdit = new QLineEdit();
    m_tokenEdit->setPlaceholderText("sk-...");
    m_tokenEdit->setEchoMode(QLineEdit::Password);
    m_tokenEdit->setFixedHeight(40);

    m_tokenError = new QLabel();
    m_tokenError->setObjectName("aiError");
    m_tokenError->setWordWrap(true);
    m_tokenError->setVisible(false);

    m_tokenSaveBtn = new QPushButton("Save and continue");
    m_tokenSaveBtn->setObjectName("aiPrimary");
    m_tokenSaveBtn->setFixedHeight(40);
    m_tokenSaveBtn->setCursor(Qt::PointingHandCursor);

    l->addWidget(info);
    l->addWidget(hint);
    l->addSpacing(4);
    l->addWidget(m_tokenEdit);
    l->addWidget(m_tokenError);
    l->addWidget(m_tokenSaveBtn);
    l->addStretch();

    connect(m_tokenSaveBtn, &QPushButton::clicked,    this, &AiDialog::onSaveToken);
    connect(m_tokenEdit,    &QLineEdit::returnPressed, this, &AiDialog::onSaveToken);

    m_stack->addWidget(page);
}

void AiDialog::buildChatPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(page);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(10);

    m_area = new QScrollArea();
    m_area->setWidgetResizable(true);
    m_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *container = new QWidget();
    container->setStyleSheet("background:transparent;");
    m_messagesLayout = new QVBoxLayout(container);
    m_messagesLayout->setContentsMargins(4, 4, 4, 4);
    m_messagesLayout->setSpacing(8);
    m_messagesLayout->addStretch();
    m_area->setWidget(container);
    l->addWidget(m_area, 1);

    QHBoxLayout *inputRow = new QHBoxLayout();
    inputRow->setSpacing(8);
    m_input = new QLineEdit();
    m_input->setPlaceholderText("Ask AI...");
    m_input->setFixedHeight(40);
    m_sendBtn = new QPushButton("Send");
    m_sendBtn->setObjectName("aiSend");
    m_sendBtn->setFixedSize(70, 40);
    m_sendBtn->setCursor(Qt::PointingHandCursor);
    inputRow->addWidget(m_input, 1);
    inputRow->addWidget(m_sendBtn);
    l->addLayout(inputRow);

    connect(m_sendBtn, &QPushButton::clicked,    this, &AiDialog::onSend);
    connect(m_input,   &QLineEdit::returnPressed, this, &AiDialog::onSend);

    m_stack->addWidget(page);
}

QString AiDialog::loadToken() const
{
    QSettings s("MAX-open-source", "client");
    return s.value("openai/token").toString();
}

void AiDialog::saveToken(const QString &token)
{
    QSettings s("MAX-open-source", "client");
    s.setValue("openai/token", token);
}

void AiDialog::onSaveToken()
{
    QString t = m_tokenEdit->text().trimmed();
    if (!t.startsWith("sk-") || t.size() < 20) {
        m_tokenError->setText("Token must start with 'sk-' and be valid.");
        m_tokenError->setVisible(true);
        return;
    }
    m_tokenError->setVisible(false);
    m_token = t;
    saveToken(t);
    m_tokenEdit->clear();
    m_keyBtn->setVisible(true);
    m_stack->setCurrentIndex(1);
    if (m_messagesLayout->count() <= 1)
        appendBubble("Token saved. Ask me anything.", false);
}

void AiDialog::onChangeToken()
{
    m_tokenEdit->setText(m_token);
    m_tokenError->setVisible(false);
    m_stack->setCurrentIndex(0);
}

void AiDialog::onSend()
{
    QString text = m_input->text().trimmed();
    if (text.isEmpty()) return;
    if (m_token.isEmpty()) {
        m_stack->setCurrentIndex(0);
        return;
    }
    appendBubble(text, true);
    m_input->clear();
    sendToOpenAI(text);
}

void AiDialog::sendToOpenAI(const QString &userMessage)
{
    QJsonObject userMsg;
    userMsg["role"]    = "user";
    userMsg["content"] = userMessage;
    m_history.append(userMsg);

    QJsonObject body;
    body["model"]    = OPENAI_MODEL;
    body["messages"] = m_history;

    QNetworkRequest req{QUrl(OPENAI_URL)};
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", ("Bearer " + m_token).toUtf8());

    m_sendBtn->setEnabled(false);
    m_input->setEnabled(false);

    QNetworkReply *reply = m_net->post(req, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        m_sendBtn->setEnabled(true);
        m_input->setEnabled(true);
        m_input->setFocus();

        QByteArray data = reply->readAll();
        int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        reply->deleteLater();

        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject obj = doc.object();

        if (status != 200) {
            QString err = obj.value("error").toObject().value("message").toString();
            if (err.isEmpty()) err = "HTTP " + QString::number(status);
            appendBubble("Error: " + err, false, true);
            if (!m_history.isEmpty()) m_history.removeLast();
            return;
        }

        QJsonArray choices = obj.value("choices").toArray();
        if (choices.isEmpty()) {
            appendBubble("Error: empty response", false, true);
            return;
        }
        QString answer = choices.first().toObject()
                             .value("message").toObject()
                             .value("content").toString();

        QJsonObject assistantMsg;
        assistantMsg["role"]    = "assistant";
        assistantMsg["content"] = answer;
        m_history.append(assistantMsg);

        appendBubble(answer, false);
    });
}

void AiDialog::appendBubble(const QString &text, bool isMine, bool isError)
{
    QWidget *row = new QWidget();
    row->setStyleSheet("background:transparent;");
    QHBoxLayout *rowLayout = new QHBoxLayout(row);
    rowLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *bubble = new QWidget();
    bubble->setObjectName(isError ? "aiBubbleErr"
                                  : (isMine ? "aiBubbleMine" : "aiBubbleAi"));
    bubble->setMaximumWidth(280);

    QVBoxLayout *bl = new QVBoxLayout(bubble);
    bl->setContentsMargins(12, 8, 12, 8);

    QLabel *lbl = new QLabel(text);
    lbl->setObjectName("aiBubbleText");
    lbl->setWordWrap(true);
    lbl->setTextInteractionFlags(Qt::TextSelectableByMouse);
    bl->addWidget(lbl);

    if (isMine) { rowLayout->addStretch(); rowLayout->addWidget(bubble); }
    else        { rowLayout->addWidget(bubble); rowLayout->addStretch(); }

    m_messagesLayout->insertWidget(m_messagesLayout->count() - 1, row);

    QScrollBar *bar = m_area->verticalScrollBar();
    bar->setValue(bar->maximum());
}
