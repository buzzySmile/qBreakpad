TEMPLATE = app
TARGET = $$PWD/../dump_symbols
QT -= gui
CONFIG -= app_bundle
CONFIG += debug_and_release warn_on console
CONFIG += thread exceptions rtti stl

SOURCES += main.cpp

include($$PWD/../../breakpad-qt-handler.pri)

OBJECTS_DIR = _build/obj
MOC_DIR = _build

# google-breakpad
