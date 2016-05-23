TEMPLATE = app
TARGET = test
QT -= gui
QT += network
CONFIG -= app_bundle
CONFIG += debug_and_release warn_off console
CONFIG += thread exceptions rtti stl

HEADERS += TestThread.h
SOURCES += TestThread.cpp

SOURCES += main.cpp

include($$PWD/../../qBreakpad-handler.pri)
QMAKE_LIBDIR += $$OUT_PWD/../../handler
LIBS += -lqBreakpad-handler

OBJECTS_DIR = _build/obj
MOC_DIR = _build

DEFINES += QT_NO_CAST_TO_ASCII
DEFINES += QT_NO_CAST_FROM_ASCII
