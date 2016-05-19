TEMPLATE = app
TARGET = reporter
QT += network
CONFIG -= app_bundle
CONFIG += debug_and_release warn_on console
CONFIG += thread exceptions rtti stl

SOURCES += main.cpp

include($$PWD/../../qBreakpad-sender.pri)
QMAKE_LIBDIR += $$OUT_PWD/../../sender/
LIBS += -lqBreakpad-sender

OBJECTS_DIR = _build/obj
MOC_DIR = _build

DEFINES += QT_NO_CAST_TO_ASCII
DEFINES += QT_NO_CAST_FROM_ASCII
