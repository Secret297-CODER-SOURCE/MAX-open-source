#ifndef FSYSTEM_H
#define FSYSTEM_H
#include <QObject>
#include <QFile>
#include <QTextStream>
class Fsystem
{

public:
    static void saveId(int userId);
    static int loadSavedId();
    static bool hasFile();
    static void saveName(const QString &name);
    static QString loadSavedName();
};
#endif // FSYSTEM_H
