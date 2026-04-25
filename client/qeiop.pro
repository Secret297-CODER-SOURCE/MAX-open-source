QT += widgets network

CONFIG += c++17

TARGET = untitled
TEMPLATE = app

SOURCES += \
    clientsocket.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    clientsocket.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    mainwindow.qrc