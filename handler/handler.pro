TEMPLATE = lib
TARGET = breakpad-qt-handler
VERSION = 0.3.0

include($$PWD/../conf.pri)

mac {
    CONFIG += c++11
}

CONFIG += warn_on thread exceptions rtti stl
QT -= gui

OBJECTS_DIR = _build/obj
MOC_DIR = _build
win32 {
    DESTDIR = $$OUT_PWD
}

DEFINES += QT_NO_CAST_TO_ASCII
DEFINES += QT_NO_CAST_FROM_ASCII

## breakpad-qt
include($$PWD/../breakpad-qt-handler.pri)
SOURCES += $$PWD/BreakpadHandler.cpp

## google-breakpad
include($$PWD/../third-party/google-breakpad.pri)
