TEMPLATE = app
TARGET = test
QT -= gui
CONFIG -= app_bundle
CONFIG += debug_and_release warn_off console

HEADERS += TestThread.h
SOURCES += TestThread.cpp

SOURCES += main.cpp

include($$PWD/../../breakpad-handler.pri)

OBJECTS_DIR = _build/obj
MOC_DIR = _build