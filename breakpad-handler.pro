TEMPLATE = lib
TARGET = $$PWD/breakpad-handler
VERSION = 0.3.0

CONFIG += static debug_and_release warn_on
QT -= gui

OBJECTS_DIR = _build/obj
MOC_DIR = _build

include($$PWD/breakpad-handler.pri)

DEFINES += QT_NO_CAST_TO_ASCII
DEFINES += QT_NO_CAST_FROM_ASCII
