QT += widgets network

CONFIG += c++17

TARGET = untitled
TEMPLATE = app

SOURCES += \
    chatwindow.cpp \
    clientsocket.cpp \
    jsonclient.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    chatwindow.h \
    clientsocket.h \
    jsonclient.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    mainwindow.qrc