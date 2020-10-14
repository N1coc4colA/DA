#-------------------------------------------------
#
# Project created by QtCreator 2020-08-20T18:00:00
#
#-------------------------------------------------

QT       += core gui widgets dbus x11extras core-private

greaterThan(QT_MAJOR_VERSION, 4) {
  QT += widgets widgets-private
  # Qt >= 5.8
  greaterThan(QT_MAJOR_VERSION, 5)|greaterThan(QT_MINOR_VERSION, 7): QT += gui-private
  else: QT += platformsupport-private
}

TARGET = libDA
TEMPLATE = lib

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

CONFIG += link_pkgconfig
PKGCONFIG = dtkwidget dtkcore

SOURCES += \
    notificationmanager.cpp \
    diconic.cpp \
    daddonapplication.cpp \
    daddonbutton.cpp \
    daddonsplittedwindow.cpp \
    daddonsplittedbar.cpp \
    elementlist.cpp \
    element.cpp \
    proxypainter.cpp \
    emptyitemdelegate.cpp \
    libraryban.cpp \
    effectwidget.cpp \
    graphictools.cpp \
    imagepopup.cpp \
    dsplitedbar.cpp \
    dsplitedwindow.cpp \
    multicolors.cpp

HEADERS += \
        libda_global.h \ 
    class_decl.h \
    daddonbutton.h \
    diconic.h \
    daddonapplication.h \
    daddonsplittedbar.h \
    daddonsplittedwindow.h \
    notificationmanager.h \
    elementlist.h \
    element.h \
    proxypainter.h \
    emptyitemdelegate.h \
    libraryban.h \
    effectwidget.h \
    graphictools.h \
    effectwidget_p.h \
    imagepopup.h \
    dsplitedbar.h \
    dsplitedwindow.h \
    multicolors.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

RESOURCES += \
    main.qrc
