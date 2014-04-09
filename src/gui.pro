#-------------------------------------------------
#
# Project created by QtCreator 2014-01-13T21:35:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gui
TEMPLATE = app

include(qextserialport/src/qextserialport.pri)

SOURCES += main.cpp\
        mainwindow.cpp \
    conflict_core.cpp

HEADERS  += mainwindow.h \
    conflict_core.h \
    typedef.h

FORMS    += mainwindow.ui
