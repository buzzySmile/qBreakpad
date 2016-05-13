<p align="center">
    <a href="https://travis-ci.org/pixraider/breakpad-qt">
        <img src="https://travis-ci.org/pixraider/breakpad-qt.svg?branch=master"
             alt="Build Status">
    </a>
</p>    
Breakpad-Qt
================
Breakpad-Qt is Qt library to use google-breakpad crash reporting facilities. Supports
* Windows (but crash dump decoding will not work with MinGW compiler)
* Linux
* MacOS X

How to use
----------------
* Build breakpad-qt static library (breakpad-qt/handler/)
* Include "breakpad-qt-handler.pri" to your target Qt project
```c++
include(libs/breakpad-qt/breakpad-qt-handler.pri)
```
* Setup linking with "breakpad-qt-handler" library
```c++
QMAKE_LIBDIR += $$OUT_PWD/submodules/breakpad/handler
LIBS += -lbreakpad-qt-handler
```
* Use ```BreakpadHandler``` singleton class to enable automatic crash dumps generation on any failure; example:
```c++
BreakpadQt::GlobalHandler::instance()->setDumpPath(QLatin1String("crashes"));
```
* Read Google Breakpad documentation to know further workflow

Getting started with Google Breakpad
----------------
https://chromium.googlesource.com/breakpad/breakpad/+/master/docs/getting_started_with_breakpad.md

How to decode minidumps
----------------
* Build dump_syms

 Mac example:
```sh
#check SDK versions
$> xcodebuild -sdk -version
$ ...
$ MacOSX10.10.sdk - OS X 10.10(macosx10.10)
$ SDKVersion: 10.10
$ ...
$> cd $BREAKPAD_SOURCE_TREE
#use proper SDKVersion = 10.10
$> xcodebuild -sdk macosx10.10 -project src/tools/mac/dump_syms/dump_syms.xcodeproj -configuration Release -target dump_syms ARCHS=x86_64 ONLY_ACTIVE_ARCH=YES MACOSX_DEPLOYMENT_TARGET=10.10 GCC_VERSION=com.apple.compilers.llvm.clang.1_0
```
* Generate symbol file for your app
	
	Mac example:
	
First generate applocation version binary with proper function names with source file line numbers.
Go to non-stripped binary (e.g. ```application```) and use dsymutil util
```sh
$> dsymutil ./application.app/Content/MacOS/application
```
now you have a DWARF debugging format version of you ```application``` at "application.dSYM/Contents/Resources/DWARF/application" near original binary
	
Next  generate symbol file:
```sh
#generate .sym file
$> dump_syms ./application.app/Content/MacOS/application.dSYM/Contents/Resources/DWARF/application > ./application.app/Content/MacOS/application.sym
			 
#store sym file in the correct location
#this step is necessary. Without that minidump_stackwalk tool doesn't work
			 
$> head -n1 application.sym
$ MODULE mac x86_64 C5CAE9D1E51536DF95C4F0CB5CCDAB270 application
			 
$> mkdir -p ./symbols/application/C5CAE9D1E51536DF95C4F0CB5CCDAB270
$> mv application.sym ./symbols/application/C5CAE9D1E51536DF95C4F0CB5CCDAB270
```
	 
* Show stack trace 

Use minidump_stackwalk tool with symbol file for show stack with full convinience	
```sh
$> minidump_stackwalk ./crash.dmp ./symbols 2>&1 | grep my_symbols
```
