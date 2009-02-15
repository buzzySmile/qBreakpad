TEMPLATE = app
TARGET = reporter
CONFIG -= app_bundle
CONFIG += debug_and_release warn_on

SOURCES += main.cpp

include($$PWD/../../breakpad-sender.pri)

OBJECTS_DIR = _build/obj
MOC_DIR = _build
