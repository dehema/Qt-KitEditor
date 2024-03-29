#-------------------------------------------------
#
# Project created by QtCreator 2024-01-08T14:42:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KitEditor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARN
QMAKE_CXXFLAGS_RELEASE = -Od -ZI -MD
QMAKE_LFLAGS_RELEASE = /DEBUG /INCREMENTAL:NOINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    main.cpp \
    kitmgr.cpp \
    kiteditorview.cpp \
    kitcreatedialog.cpp \
    kitchoosedialog.cpp \
    kitcolordialog.cpp

HEADERS += \
    kitmgr.h \
    kitmodel.h \
    kiteditorview.h \
    kitcreatedialog.h \
    kitchoosedialog.h \
    kitcolordialog.h

FORMS += \
    kiteditorview.ui \
    kitcreatedialog.ui \
    kitchoosedialog.ui \
    kitcolordialog.ui
