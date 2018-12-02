#-------------------------------------------------
#
# Project created by QtCreator 2016-11-03T15:33:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Ziarna
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    space.cpp \
    seed.cpp \
    filemanager.cpp

HEADERS  += mainwindow.h \
    space.h \
    seed.h \
    filemanager.h

FORMS    += mainwindow.ui

RC_FILE = grains.rc
