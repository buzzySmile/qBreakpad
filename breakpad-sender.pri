## breakpad-qt
INCLUDEPATH += $$PWD/sender
HEADERS += $$PWD/sender/BreakpadSender.h
SOURCES += $$PWD/sender/BreakpadSender.cpp

## google-breakpad
BREAKPAD_PATH = $$PWD/third-party/google-breakpad/src
INCLUDEPATH += $$BREAKPAD_PATH

# every *nix
unix {
}

# mac os x
mac { 
}

# other *nix
unix:!mac {
    SOURCES += $$BREAKPAD_PATH/common/linux/http_upload.cc
}

win32 {
}
