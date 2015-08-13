#-------------------------------------------------
#
# Project created by QtCreator 2014-05-21T01:43:07
#
#-------------------------------------------------

QT       += core gui sql network multimedia


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

TARGET = adbFire
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    dialog2.cpp \
    helpdialog.cpp \
    uninstalldialog.cpp \
    preferencesdialog.cpp \
    usbfiledialog.cpp \
    uuiddialog.cpp \
    llamadialog.cpp \
    creditsdialog.cpp \
    adblogdialog.cpp \
    cifsdialog.cpp \
    videodialog.cpp \
    kodidialog.cpp \
    unlockdialog.cpp \
    bootmenudialog.cpp \
    unbootdialog.cpp \
    datadialog.cpp \
    mediacenterdialog.cpp 


RESOURCES = adbFire.qrc

HEADERS  += mainwindow.h \
    dialog2.h \
    helpdialog.h \
    uninstalldialog.h \
    preferencesdialog.h \
    usbfiledialog.h \
    uuiddialog.h \
    llamadialog.h \
    creditsdialog.h \
    adblogdialog.h \  
    cifsdialog.h \
    videodialog.h \
    kodidialog.h \
    unlockdialog.h \
    bootmenudialog.h \
    unbootdialog.h \
    datadialog.h \
    mediacenterdialog.h 

FORMS    += mainwindow.ui \
    dialog2.ui \
    helpdialog.ui \
    uninstalldialog.ui \
    preferencesdialog.ui \
    usbfiledialog.ui \
    uuiddialog.ui \
    llamadialog.ui \
    creditsdialog.ui \
    adblogdialog.ui \
    cifsdialog.ui \
    videodialog.ui \
    kodidialog.ui \
    unlockdialog.ui \
    bootmenudialog.ui \
    unbootdialog.ui \
    datadialog.ui \
    mediacenterdialog.ui

RC_FILE = adbFire.rc

ICON = adbFire.icns

OTHER_FILES +=
