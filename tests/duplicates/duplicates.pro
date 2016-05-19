TEMPLATE = app
TARGET = duplicates_test
QT -= gui
CONFIG -= app_bundle
CONFIG += debug_and_release warn_off console
CONFIG += thread exceptions rtti stl

SOURCES += main.cpp

include($$PWD/../../qBreakpad-handler.pri)
QMAKE_LIBDIR += $$OUT_PWD/../../handler
LIBS += -lqBreakpad-handler

OBJECTS_DIR = _build/obj
MOC_DIR = _build
