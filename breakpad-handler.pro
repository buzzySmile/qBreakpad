TEMPLATE = lib
TARGET = $$PWD/breakpad-qt
VERSION = 0.2.0

CONFIG += static release warn_on
CONFIG -= debug
QT -= gui

OBJECTS_DIR = _build/obj
MOC_DIR = _build

include($$PWD/breakpad-handler.pri)

DEFINES += QT_NO_CAST_TO_ASCII
DEFINES += QT_NO_CAST_FROM_ASCII
