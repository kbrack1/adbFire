#-------------------------------------------------
#
# Project created by QtCreator 2014-05-21T01:43:07
#
#-------------------------------------------------

QT       += core gui sql network


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = adbFire
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    dialog2.cpp \
    helpdialog.cpp \
    uninstalldialog.cpp \
    preferencesdialog.cpp \
    usbfiledialog.cpp \
    llamadialog.cpp \
    creditsdialog.cpp \
    adblogdialog.cpp


RESOURCES = adbFire.qrc

HEADERS  += mainwindow.h \
    dialog2.h \
    helpdialog.h \
    uninstalldialog.h \
    preferencesdialog.h \
    usbfiledialog.h \
    llamadialog.h \
    creditsdialog.h \
    adblogdialog.h \

FORMS    += mainwindow.ui \
    dialog2.ui \
    helpdialog.ui \
    uninstalldialog.ui \
    preferencesdialog.ui \
    usbfiledialog.ui \
    llamadialog.ui \
    creditsdialog.ui \
    adblogdialog.ui

RC_FILE = adbFire.rc

ICON = adbfire.icns

OTHER_FILES +=
