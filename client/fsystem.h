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
};

#endif // FSYSTEM_H
