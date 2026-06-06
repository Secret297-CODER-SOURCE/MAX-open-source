#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql>
#include <QDebug>

struct MessageRecord {
    int id;
    int senderId;
    int receiverId;
    QString content;
    QString timestamp;
};

class database
{
public:
    database();
    ~database();

    void open();
    void close();
    void registerUser(const QString& username, int customId)
    {
        if (userExists(username)) return;
        QSqlQuery q(m_db);
        q.prepare("INSERT OR IGNORE INTO users (id, username) VALUES (:id, :u)");
        q.bindValue(":id", customId);
        q.bindValue(":u",  username);
        q.exec();
    }
    bool userExists(const QString& username);
    int getUserId(const QString& username);
    QString getUsernameById(int userId);
    void saveMessage(const int senderId,const int receiverId,const QString& content);
    QList<MessageRecord> getHistory(int userId1, int userId2);
    QList<MessageRecord> getAllMessages(int userId);
    bool isOpen() const;
private:
    QSqlDatabase m_db;
    void createTables();
    QString connectionName() const { return "ServerDB"; }
};

#endif
