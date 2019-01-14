#-------------------------------------------------
#
# Project created by QtCreator 2018-12-24T12:55:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mdi
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

CONFIG(debug, debug|release) {
    DESTDIR = $$OUT_PWD/debug
} else {
    DESTDIR = $$OUT_PWD/release
}
win32: {
CONFIG(debug, debug|release) {
QMAKE_POST_LINK = $$(QTDIR)/bin/windeployqt $$OUT_PWD/debug
} else{
QMAKE_POST_LINK = $$(QTDIR)/bin/windeployqt $$OUT_PWD/release
}
}

MOC_DIR=moc
RCC_DIR=rcc
UI_DIR=ui
OBJECTS_DIR=obj

SOURCES += \
        main.cpp \
        mainwin.cpp \
        editor.cpp \
    syntaxhiglighter.cpp

HEADERS += \
        mainwin.h \
    editor.h \
    syntaxhiglighter.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
#LIBS += -lqscintilla2

win32: LIBS += -lqscintilla2_qt5
