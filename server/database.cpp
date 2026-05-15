#include "database.h"

database::database() {}

database::~database()
{
    close();
}

void database::open()
{
    if (m_db.isOpen())
        m_db.close();

    m_db = QSqlDatabase();

    if (QSqlDatabase::contains(connectionName()))
        QSqlDatabase::removeDatabase(connectionName());

    m_db = QSqlDatabase::addDatabase("QSQLITE", connectionName());
    m_db.setDatabaseName("BAZA.db");
    m_db.open();
    createTables();
}

void database::close()
{
    if (m_db.isOpen())
        m_db.close();
}

bool database::isOpen() const
{
    return m_db.isOpen();
}

void database::createTables()
{
    QSqlQuery q(m_db);
    q.exec(R"(
        CREATE TABLE IF NOT EXISTS users (
            id       INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
        )
    )");
    q.exec(R"(
        CREATE TABLE IF NOT EXISTS messages (
            id            INTEGER PRIMARY KEY AUTOINCREMENT,
            sender_id     INTEGER NOT NULL,
            receiver_id   INTEGER NOT NULL,
            content       TEXT    NOT NULL,
            timestamp     TEXT    DEFAULT (datetime('now'))
        )
    )");
}

void database::registerUser(const QString& username)
{
    if (userExists(username))
        return;
    QSqlQuery q(m_db);
    q.prepare("INSERT OR IGNORE INTO users (username) VALUES (:u)");
    q.bindValue(":u", username);
    q.exec();
}

bool database::userExists(const QString& username)
{
    QSqlQuery q(m_db);
    q.prepare("SELECT 1 FROM users WHERE username = :u");
    q.bindValue(":u", username);
    q.exec();
    return q.next();
}

int database::getUserId(const QString& username)
{
    QSqlQuery q(m_db);
    q.prepare("SELECT id FROM users WHERE username = :u");
    q.bindValue(":u", username);
    q.exec();
    if (q.next())
        return q.value(0).toInt();
    return -1;
}

void database::saveMessage(const int senderId, const int receiverId, const QString& content)
{
    QSqlQuery q(m_db);
    q.prepare(R"(
        INSERT INTO messages (sender_id, receiver_id, content)
        VALUES (:sid, :rid, :content)
    )");
    q.bindValue(":sid", senderId);
    q.bindValue(":rid", receiverId);
    q.bindValue(":content", content);
    q.exec();
}

QList<MessageRecord> database::getHistory(int userId1, int userId2)
{
    QList<MessageRecord> result;
    QSqlQuery q(m_db);
    q.prepare(R"(
        SELECT id, sender_id, receiver_id, content, timestamp
        FROM messages
        WHERE (sender_id = :u1 AND receiver_id = :u2)
           OR (sender_id = :u2b AND receiver_id = :u1b)
        ORDER BY id ASC
    )");
    q.bindValue(":u1",  userId1);
    q.bindValue(":u2",  userId2);
    q.bindValue(":u1b", userId1);
    q.bindValue(":u2b", userId2);
    q.exec();

    while (q.next()) {
        MessageRecord r;
        r.id = q.value(0).toInt();
        r.senderId=q.value(1).toInt();
        r.receiverId=q.value(2).toInt();
        r.content =q.value(3).toString();
        r.timestamp= q.value(4).toString();
        result.append(r);
    }
    return result;
}

QList<MessageRecord> database::getAllMessages(int userId)
{
    QList<MessageRecord> result;
    QSqlQuery q(m_db);
    q.prepare(R"(
        SELECT id, sender_id, receiver_id, content, timestamp
        FROM messages
        WHERE sender_id = :u OR receiver_id = :u2
        ORDER BY id ASC
    )");
    q.bindValue(":u",  userId);
    q.bindValue(":u2", userId);
    q.exec();

    while (q.next()) {
        MessageRecord r;
        r.id = q.value(0).toInt();
        r.senderId=q.value(1).toInt();
        r.receiverId=q.value(2).toInt();
        r.content =q.value(3).toString();
        r.timestamp= q.value(4).toString();
        result.append(r);
    }
    return result;
}
