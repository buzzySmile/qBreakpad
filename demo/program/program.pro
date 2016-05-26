TEMPLATE = app
TARGET = test
QT -= gui
QT += network
CONFIG -= app_bundle
CONFIG += debug_and_release warn_off
CONFIG += thread exceptions rtti stl

HEADERS += TestThread.h
SOURCES += TestThread.cpp

SOURCES += main.cpp

include($$PWD/../../qBreakpad.pri)
QMAKE_LIBDIR += $$OUT_PWD/../../handler
LIBS += -lqBreakpad

OBJECTS_DIR = _build/obj
MOC_DIR = _build
