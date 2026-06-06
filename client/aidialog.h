#ifndef AIDIALOG_H
#define AIDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QJsonArray>
#include <QNetworkAccessManager>

class AiDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AiDialog(QWidget *parent = nullptr);

private slots:
    void onSend();
    void onSaveToken();
    void onChangeToken();

private:
    void buildTokenPage();
    void buildChatPage();
    void appendBubble(const QString &text, bool isMine, bool isError = false);
    void sendToOpenAI(const QString &userMessage);
    QString loadToken() const;
    void saveToken(const QString &token);

    QStackedWidget *m_stack;

    // Token page
    QLineEdit *m_tokenEdit;
    QPushButton *m_tokenSaveBtn;
    QLabel *m_tokenError;

    // Chat page
    QScrollArea *m_area;
    QVBoxLayout *m_messagesLayout;
    QLineEdit   *m_input;
    QPushButton *m_sendBtn;
    QPushButton *m_keyBtn;

    QNetworkAccessManager *m_net;
    QJsonArray m_history;
    QString m_token;
};

#endif // AIDIALOG_H
