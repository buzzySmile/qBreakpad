TEMPLATE = lib
TARGET = $$PWD/breakpad-sender
VERSION = 0.3.0

CONFIG += static debug_and_release warn_on
CONFIG += thread exceptions rtti stl
QT -= gui

OBJECTS_DIR = _build/obj
MOC_DIR = _build

include($$PWD/breakpad-sender.pri)

DEFINES += QT_NO_CAST_TO_ASCII
DEFINES += QT_NO_CAST_FROM_ASCII
