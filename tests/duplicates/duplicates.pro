TEMPLATE = app
TARGET = $$PWD/../duplicates_test
QT -= gui
CONFIG -= app_bundle
CONFIG += debug_and_release warn_off console
CONFIG += thread exceptions rtti stl

SOURCES += main.cpp

include($$PWD/../../breakpad-qt-handler.pri)
LIBS += -L$$PWD/../../ -lbreakpad-qt-handler

OBJECTS_DIR = _build/obj
MOC_DIR = _build
