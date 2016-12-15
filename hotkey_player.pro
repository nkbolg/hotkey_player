#-------------------------------------------------
#
# Project created by QtCreator 2016-12-14T23:53:02
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hotkey_player
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h


CONFIG += C++14

win32: LIBS += -lOle32 -luser32

VERSION = 0.1.1

DEFINES += VERSION_STRING=\\\"$${VERSION}\\\"

CONFIG -= debug_and_release debug_and_release_targets

RESOURCES += \
    icons.qrc

