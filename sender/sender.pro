TEMPLATE = lib
TARGET = $$PWD/../breakpad-qt-sender
VERSION = 0.3.0

include($$PWD/../conf.pri)

CONFIG += warn_on
QT -= gui
QT += network

OBJECTS_DIR = _build/obj
MOC_DIR = _build

DEFINES += QT_NO_CAST_TO_ASCII
DEFINES += QT_NO_CAST_FROM_ASCII

HEADERS += $$PWD/BreakpadHttpSender.h
SOURCES += $$PWD/BreakpadHttpSender.cpp
