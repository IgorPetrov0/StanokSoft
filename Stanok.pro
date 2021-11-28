#-------------------------------------------------
#
# Project created by QtCreator 2020-09-03T06:23:04
#
#-------------------------------------------------

QT       += core gui
 QT += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Stanok
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        apperture.cpp \
        connectionindicator.cpp \
        drawcommand.cpp \
        gerberconverter.cpp \
        gpath.cpp \
        main.cpp \
        mainwindow.cpp \
        pad.cpp \
        parsergcode.cpp \
        stanokobject.cpp \
        usbconnection.cpp \
        widgets/consoletextwidget.cpp \
        widgets/directmanagewidget.cpp \
        widgets/gcodeeditwidget.cpp \
        widgets/gframe.cpp \
        widgets/gpushbutton.cpp \
        widgets/progressdialor.cpp \
        widgets/serialportsdialog.cpp \
        widgets/viewwidget.cpp

HEADERS += \
        apperture.h \
        connectionindicator.h \
        drawcommand.h \
        gerberconverter.h \
        gpath.h \
        mainwindow.h \
        pad.h \
        parsergcode.h \
        stanokobject.h \
        usbconnection.h \
        widgets/consoletextwidget.h \
        widgets/directmanagewidget.h \
        widgets/gcodeeditwidget.h \
        widgets/gframe.h \
        widgets/gpushbutton.h \
        widgets/progressdialor.h \
        widgets/serialportsdialog.h \
        widgets/viewwidget.h

FORMS += \
        mainwindow.ui \
        widgets/directmanagewidget.ui \
        widgets/progressdialor.ui \
        widgets/serialportsdialog.ui \
        widgets/viewwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
