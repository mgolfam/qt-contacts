#-------------------------------------------------
#
# Project created by QtCreator 2022-08-18T10:43:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = contacts-app
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dialogadduser.cpp \
    contact.cpp \
    phonebook.cpp

HEADERS  += mainwindow.h \
    dialogadduser.h \
    contact.h \
    phonebook.h

FORMS    += mainwindow.ui \
    dialogadduser.ui
