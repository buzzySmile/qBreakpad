/*
 *  Copyright (C) 2009 Aleksey Palazhchenko
 *  Copyright (C) 2014 Sergey Shambir
 *  Copyright (C) 2016 Alexander Makarov
 *
 * This file is a part of Breakpad-qt library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 */

#ifndef QBREAKPAD_HANDLER_H
#define QBREAKPAD_HANDLER_H

#include <QString>
#include <QUrl>
#include "singletone/singleton.h"

namespace google_breakpad {
    class ExceptionHandler;
    class MinidumpDescriptor;
}

class QBreakpadHandlerPrivate;

class QBreakpadHandler: public QObject
{
    Q_OBJECT
public:
    static QString version();

    QBreakpadHandler();
    ~QBreakpadHandler();

    QString uploadUrl() const;
    QString dumpPath() const;
    QStringList dumpFileList() const;

    void setDumpPath(const QString& path);
    void setUploadUrl(const QUrl& url);

    /// Delete dump files but keep \a keepLastestFileNum latest file according file's time
    static void DelOldDumpFiles(int keepLastestFileNum);

    /// To crash. A static test function
    static void toCrash();

#ifdef Q_OS_WIN32
    /// Add a windows veh. Experimental Function
    /// \details Google breakpad do not catch exception after qt app enters main event loop, so add a veh to catch exceptions.
    /// Test environment: qt5.6.2_mingw, qt5.6.2_msvc2015-32bit, qt5.9.9_mingw, qt5.9.9_msvc2015-32bit
    static void AddWindowsVeh();
#endif

public slots:
    void sendDumps();

private:
    QBreakpadHandlerPrivate* d;
};
#define QBreakpadInstance Singleton<QBreakpadHandler>::instance()

#endif	// QBREAKPAD_HANDLER_H
