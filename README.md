Breakpad-Qt
================
Breakpad-Qt is Qt library to use google-breakpad crash reporting facilities. Supports
* Windows (but crash dump decoding will not work with MinGW compiler)
* Linux
* MacOS X

How to use
----------------
* Include "breakpad-qt-handler.pri" to your QtCreator project;
* Setup linking with "breakpad-qt-handler" library;
Linking example (breakpad added as git submodule):
```c++
QMAKE_LIBDIR += $$OUT_PWD/submodules/breakpad/handler
LIBS += -lbreakpad-qt-handler
```
* Use ```BreakpadHandler``` singleton class to enable automatic crash dumps generation on any failure;
Usage example:
```c++
BreakpadQt::GlobalHandler::instance()->setDumpPath(QLatin1String("crashes"));
```
* Read Google Breakpad documentation to know further workflow

Getting started with Google Breakpad
----------------
http://code.google.com/p/google-breakpad/wiki/GettingStartedWithBreakpad
