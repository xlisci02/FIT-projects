#-------------------------------------------------
#
# Project created by QtCreator 2018-04-24T00:10:34
#
#-------------------------------------------------

QT       += core gui widgets 

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = blockeditor
MAKEFILE = gui_makefile
DEPENDPATH += .
INCLUDEPATH += .

TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++14

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    schemeitem.cpp \
    schemescene.cpp \
    link.cpp \
    block.cpp \
    getreal.cpp \
    getimaginary.cpp \
    arithmblock.cpp \
    polarform.cpp \
    port.cpp \
    scheme.cpp \
    adder.cpp \
    multiplier.cpp \
    subtractor.cpp \
    divider.cpp \
    connection.cpp \
    createcomplex.cpp \
    polartocomplex.cpp

HEADERS += \
        mainwindow.h \
    schemeitem.h \
    schemescene.h \
    link.h \
    block.h \
    port.h \
    polarform.h \
    getreal.h \
    getimaginary.h \
    scheme.h \
    connection.h \
    subtractor.h \
    multiplier.h \
    divider.h \
    adder.h \
    arithmblock.h \
    createcomplex.h \
    polartocomplex.h

RESOURCES += \
    schemescene.qrc
