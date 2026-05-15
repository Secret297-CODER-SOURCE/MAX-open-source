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
    void registerUser(const QString& username);
    bool userExists(const QString& username);
    int getUserId(const QString& username);
    void saveMessage(const int senderId,const int receiverId,const QString& content);
    QList<MessageRecord> getHistory(int userId1, int userId2);
    QList<MessageRecord> getAllMessages(int userId);
    bool isOpen() const;
private:
    QSqlDatabase m_db;
    QString      m_lastError;

    void createTables();
    QString connectionName() const { return "ServerDB"; }
};

#endif // DATABASE_H
