#-------------------------------------------------
#
# Project created by QtCreator 2016-08-14T18:04:14
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = InventarIO
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    item.cpp \
    computer.cpp \
    soft.cpp \
    event.cpp \
    item_dialog.cpp \
    sql.cpp \
    data.cpp \
    specs.cpp \
    event_dialog.cpp \
    cv_tree_widget.cpp \
    algorythm.cpp

HEADERS  += mainwindow.h \
    item.h \
    computer.h \
    soft.h \
    event.h \
    item_dialog.h \
    sql.h \
    data.h \
    specs.h \
    event_dialog.h \
    cv_tree_widget.h \
    algorythm.h

FORMS    += mainwindow.ui \
    item_dialog.ui \
    event_dialog.ui \
    cv_tree_widget.ui

RESOURCES += \
    res.qrc
