TEMPLATE = lib
TARGET = breakpad-qt-handler
VERSION = 0.4.0

include($$PWD/../conf.pri)

CONFIG += warn_on thread exceptions rtti stl
QT -= gui
QT += network

OBJECTS_DIR = _build/obj
MOC_DIR = _build
win32 {
    DESTDIR = $$OUT_PWD
}

DEFINES += QT_NO_CAST_TO_ASCII
DEFINES += QT_NO_CAST_FROM_ASCII

## breakpad-qt
include($$PWD/../breakpad-qt-handler.pri)

## google-breakpad
include($$PWD/../third_party/breakpad.pri)

SOURCES += \
    $$PWD/QBreakpadHandler.cpp
