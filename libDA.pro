#-------------------------------------------------
#
# Project created by QtCreator 2020-08-20T18:00:00
#
#-------------------------------------------------

QT       += core gui widgets dbus xml script svg network x11extras core-private

greaterThan(QT_MAJOR_VERSION, 4) {
  QT += widgets widgets-private
  # Qt >= 5.8
  greaterThan(QT_MAJOR_VERSION, 5)|greaterThan(QT_MINOR_VERSION, 7): QT += gui-private
  else: QT += platformsupport-private
}

CONFIG += link_pkgconfig debug c++17
PKGCONFIG = dtkwidget dtkcore dtkgui x11 glib-2.0

unix:!macx: LIBS += -lKF5WindowSystem

TARGET = libDA
TEMPLATE = lib
VERSION = 1.0.5

DEFINES += LIBDA_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


unix {
    target.path = /usr/lib
    INSTALLS += target
}

SUBDIRS += \
    libDA.pro \
    libDA.pro

HEADERS += \
    abstractelement.h \
    basicelements.h \
    class_decl.h \
    daddonapplication.h \
    daddonbutton.h \
    daddonsplittedbar.h \
    daddonsplittedwindow.h \
    diconic.h \
    dotsslidetabs.h \
    dynamicsvgparser.h \
    effectwidget_p.h \
    effectwidget.h \
    element.h \
    elementlist.h \
    emptyitemdelegate.h \
    graphictools.h \
    imagepopup.h \
    indraggablewidget.h \
    iod.h \
    libda_global.h \
    libraryban.h \
    macros.h \
    multicolors.h \
    notificationmanager.h \
    proxypainter.h \
    pwl.h \
    sharednam.h \
    stackedslidewidget.h \
    swipinggesture.h \
    touchinterfacing.h \
    touchsystem.h \
    translationengine.h \
    trayicon.h \
    visualstackelement.h \
    wintools.h \
    xwengine.h

SOURCES += \
    abstractelement.cpp \
    basicelements.cpp \
    daddonapplication.cpp \
    daddonbutton.cpp \
    daddonsplittedbar.cpp \
    daddonsplittedwindow.cpp \
    diconic.cpp \
    dotsslidetabs.cpp \
    dynamicsvgparser.cpp \
    effectwidget.cpp \
    element.cpp \
    elementlist.cpp \
    emptyitemdelegate.cpp \
    graphictools.cpp \
    imagepopup.cpp \
    indraggablewidget.cpp \
    iod.cpp \
    libraryban.cpp \
    macros.cpp \
    multicolors.cpp \
    notificationmanager.cpp \
    proxypainter.cpp \
    pwl.cpp \
    sharednam.cpp \
    stackedslidewidget.cpp \
    swipinggesture.cpp \
    touchinterfacing.cpp \
    touchsystem.cpp \
    translationengine.cpp \
    trayicon.cpp \
    visualstackelement.cpp \
    wintools.cpp \
    xwengine.cpp
