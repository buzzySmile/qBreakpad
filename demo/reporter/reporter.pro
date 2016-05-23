TEMPLATE = app
TARGET = reporter
QT += widgets

CONFIG -= app_bundle
CONFIG += warn_on debug_and_release
CONFIG += thread exceptions rtti stl

QT += network
include($$PWD/../../qBreakpad-handler.pri)
QMAKE_LIBDIR += $$OUT_PWD/../../handler/
LIBS += -lqBreakpad-handler

# Define the source code
HEADERS += \
    $$PWD/../program/TestThread.h \
    $$PWD/reporter.h

SOURCES += \
    $$PWD/../program/TestThread.cpp \
    $$PWD/reporter.cpp

FORMS += \
    $$PWD/reporter.ui

OBJECTS_DIR = _build/obj
MOC_DIR = _build
