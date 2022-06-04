message("BREAKPAD_crash_handler_attached")

INCLUDEPATH += $$PWD/handler/

HEADERS += \
    $$PWD/handler/QBreakpadHandler.h \
    $$PWD/handler/QBreakpadHttpUploader.h

LIBS += \
    -L$$PWD/handler -lqBreakpad

# ---- A typical configuration of adding debug information in release build ----
# Test environment:
# Windows: qt5.6.2 mingw4.9.2-32bit and qt5.6.2 msvc2015-32bit
# Linux: qt5.6.2 gcc5.3.1
# Mac: do not test

*-g++ {
#message($$QMAKE_CXXFLAGS_DEBUG)  # => -g
#message($$QMAKE_CXXFLAGS_RELEASE)  # => -O2
#message($$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO)  # => -O2 -g
#message($$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO)  # => linux: -O2 -g; mingw: nothing
#message($$QMAKE_LFLAGS_DEBUG)  # => nothing
#message($$QMAKE_LFLAGS_RELEASE)  # => linux: -Wl,-O1; mingw: -Wl,-s

# Add debug information, delete compilation optimization
QMAKE_CFLAGS_RELEASE += -g  # QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO is '-O2 -g', but don't add '-O2'
QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -g  # QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_CXXFLAGS_RELEASE -= -O2

QMAKE_LFLAGS_RELEASE -= -Wl,-s  # Delete the default parameter '-Wl,-s'(delete debug information in linking£©
#QMAKE_LFLAGS_RELEASE -= -Wl,-O1  # In linking's -O1, only -fmerge-constants valid, but it don't affect debug info. So don't delete -O1
}
else {
#message($$QMAKE_CXXFLAGS_DEBUG)  # => -Zi -MDd
#message($$QMAKE_CXXFLAGS_RELEASE)  # => -O2 -MD
#message($$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO)  # => -O2 -Zi -MD
#message($$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO)  # => -O2 -Zi -MD
#message($$QMAKE_LFLAGS_DEBUG)  # => nothing
#message($$QMAKE_LFLAGS_RELEASE)  # => /INCREMENTAL:NO

# Add debug information, delete compilation optimization
QMAKE_CFLAGS_RELEASE += -Zi /Od
QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -Zi /Od
QMAKE_CXXFLAGS_RELEASE -= -O2

QMAKE_LFLAGS_RELEASE += /DEBUG  # This disable /INCREMENTAL:NO implicitly
}
# ---- A typical configuration of adding debug information in release build ----
