#include "fsystem.h"
#include <QDebug>

int Fsystem::loadSavedId()
{
    QFile idFile("id.txt");

    if (!idFile.exists()) {
        qDebug() << "File 'id.txt' does not exist.";
        return 0;
    }

    if (idFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        bool valid = false;
        QString line = QTextStream(&idFile).readLine().trimmed();
        int id = line.toInt(&valid);

        if (valid) {
            qDebug() << "Successfully loaded ID from file:" << id;
            return id;
        } else {
            qWarning() << "Failed to parse ID from file. Content was:" << line;
        }
    } else {
        qCritical() << "Failed to open 'id.txt' for reading.";
    }

    return 0;
}

void Fsystem::saveId(int userId)
{
    QFile idFile("id.txt");
    if (idFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&idFile);
        out << userId;
        idFile.close();
        qDebug() << "Successfully saved ID to file:" << userId;
    } else {
        qCritical() << "Failed to open 'id.txt' for writing. Cannot save ID:" << userId;
    }
}

bool Fsystem::hasFile()
{
    QFile idFile("id.txt");
    bool exists = idFile.exists();
    qDebug() << "Checking if 'id.txt' exists:" << exists;
    return exists;
}
void Fsystem::saveName(const QString &name)
{
    QFile f("name.txt");
    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&f);
        out << name;
    }
}

QString Fsystem::loadSavedName()
{
    QFile f("name.txt");
    if (f.open(QIODevice::ReadOnly | QIODevice::Text))
        return QTextStream(&f).readAll().trimmed();
    return "";
}
