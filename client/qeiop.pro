QT += widgets network

CONFIG += c++17

TARGET = untitled
TEMPLATE = app

SOURCES += \
    aidialog.cpp \
    chatwindow.cpp \
    clientsocket.cpp \
    fsystem.cpp \
    jsonclient.cpp \
    logger.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    aidialog.h \
    chatwindow.h \
    clientsocket.h \
    fsystem.h \
    jsonclient.h \
    logger.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    mainwindow.qrc

DISTFILES += \
    README.md
