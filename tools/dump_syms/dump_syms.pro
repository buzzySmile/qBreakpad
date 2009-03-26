TEMPLATE = app
TARGET = $$PWD/../dump_symbols
QT -= gui
CONFIG -= app_bundle
CONFIG += debug_and_release warn_on console
CONFIG += thread exceptions rtti stl

SOURCES += main.cpp

OBJECTS_DIR = _build/obj
MOC_DIR = _build

# google-breakpad
include($$PWD/../../google-breakpad.pri)

# other *nix
unix:!mac {
        SOURCES += $$BREAKPAD_PATH/common/linux/dump_symbols.cc \
                $$BREAKPAD_PATH/common/linux/file_id.cc \
                $$BREAKPAD_PATH/common/md5.c
}
