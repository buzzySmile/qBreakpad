This is Qt library to use google-breakpad crash reporting facilities. Supports Windows (MSVC only!), Linux and MacOSX.

How to use
=================
* Include "breakpad-qt-handler.pri" to your QtCreator project;
* Setup linking with "breakpad-qt-handler" library;
* Use ```BreakpadHandler``` singleton class to enable automatic crash dumps generation on any failure;
* Read Google Breakpad documentation to know further workflow

About Google Breakpad
=================
http://code.google.com/p/google-breakpad/wiki/GettingStartedWithBreakpad
