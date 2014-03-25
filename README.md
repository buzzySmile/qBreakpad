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
* Use ```BreakpadHandler``` singleton class to enable automatic crash dumps generation on any failure;
* Read Google Breakpad documentation to know further workflow

Getting started with Google Breakpad
----------------
http://code.google.com/p/google-breakpad/wiki/GettingStartedWithBreakpad
