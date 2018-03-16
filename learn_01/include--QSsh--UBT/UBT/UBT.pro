#-------------------------------------------------
#
# Project created by QtCreator 2017-09-29T14:25:05
#
#-------------------------------------------------

include(.\QSsh\qssh.pro)

QT       += core gui
QT       +=sql
QT       +=network



greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UBT
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    logindialog.cpp \
    agvframe.cpp \
    materialflowframe.cpp \
    logframe.cpp \
    addagv.cpp \
    agv.cpp \
    workthread.cpp \
    workstationthread.cpp \
    station.cpp \
    handler.cpp \
    tablemodel.cpp \
    endstationthread.cpp \
    manualtaskpoint.cpp \
    readystationthread.cpp \
    warndialog.cpp \
    database.cpp \
    workarea.cpp \
    addarea.cpp \
    updatestation.cpp \
    stationframe.cpp \
    mes.cpp \
    mesdialog.cpp \
    mysshconnect.cpp

HEADERS  += mainwindow.h \
    logindialog.h \
    agvframe.h \
    materialflowframe.h \
    logframe.h \
    addagv.h \
    agv.h \
    workthread.h \
    workstationthread.h \
    station.h \
    handler.h \
    tablemodel.h \
    endstationthread.h \
    manualtaskpoint.h \
    readystationthread.h \
    warndialog.h \
    database.h \
    workarea.h \
    addarea.h \
    updatestation.h \
    stationframe.h \
    mes.h \
    mesdialog.h \
    mysshconnect.h

FORMS    += mainwindow.ui \
    logindialog.ui \
    agvframe.ui \
    materialflowframe.ui \
    logframe.ui \
    addagv.ui \
    manualtaskpoint.ui \
    warndialog.ui \
    workarea.ui \
    addarea.ui \
    updatestation.ui \
    stationframe.ui \
    mesdialog.ui

DISTFILES +=

RESOURCES += \
    myresources.qrc



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/QSsh/lib/ -lQSsh
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/QSsh/lib/ -lQSshd
else:unix: LIBS += -L$$PWD/QSsh/lib/ -lQSsh

INCLUDEPATH += $$PWD/QSsh/lib
DEPENDPATH += $$PWD/QSsh/lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/QSsh/lib/ -lQSsh
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/QSsh/lib/ -lQSshd
else:unix:!macx: LIBS += -L$$PWD/QSsh/lib/ -lQSsh

INCLUDEPATH += $$PWD/QSsh/include
DEPENDPATH += $$PWD/QSsh/include
