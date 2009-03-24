/*
	Copyright (c) 2009, Aleksey Palazhchenko
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are
	met:

		* Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.
		* Redistributions in binary form must reproduce the above
	copyright notice, this list of conditions and the following disclaimer
	in the documentation and/or other materials provided with the
	distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "BreakpadHandler.h"

#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QCoreApplication>
#if defined(QT_GUI_LIB)
#	include <QtGui/QDesktopServices>
#endif

#if defined(Q_OS_MAC)
#include "client/mac/handler/exception_handler.h"
#elif defined(Q_OS_LINUX)
#include "client/linux/handler/exception_handler.h"
#elif defined(Q_OS_WIN32)
#include "client/windows/handler/exception_handler.h"
#endif

namespace BreakpadQt
{

class GlobalHandlerPrivate
{
public:
	GlobalHandlerPrivate();
	~GlobalHandlerPrivate();

public:
	static QString reporter_;
	static google_breakpad::ExceptionHandler* handler_;
};

QString GlobalHandlerPrivate::reporter_ = QString();
google_breakpad::ExceptionHandler* GlobalHandlerPrivate::handler_ = 0;


#if defined(Q_OS_WIN32)
bool DumpCallback(const wchar_t* _dump_dir,
				const wchar_t* _minidump_id,
				void* context,
				EXCEPTION_POINTERS* exinfo,
				MDRawAssertionInfo* assertion,
				bool success)
#else
bool DumpCallback(const char* _dump_dir,
				const char* _minidump_id,
				void *context, bool success)
#endif
{
	Q_UNUSED(_dump_dir);
	Q_UNUSED(_minidump_id);
	Q_UNUSED(context);
#if defined(Q_OS_WIN32)
	Q_UNUSED(assertion);
	Q_UNUSED(exinfo);
#endif

	/*
		NO STACK USE, NO HEAP USE THERE !!!
		Creating QString's, using qDebug, etc. - everything is crash-unfriendly.
	*/

	if(!GlobalHandlerPrivate::reporter_.isEmpty()) {
		QProcess::startDetached(GlobalHandlerPrivate::reporter_);	// very likely we will die there
	}
	return success;
}


GlobalHandlerPrivate::GlobalHandlerPrivate()
{
	handler_ = new google_breakpad::ExceptionHandler(/*DumpPath*/ "", /*FilterCallback*/ 0, DumpCallback, /*context*/ 0, true);
	reporter_.clear();
}

GlobalHandlerPrivate::~GlobalHandlerPrivate()
{
	delete handler_;
	handler_ = 0;
}


GlobalHandler* GlobalHandler::instance()
{
	static GlobalHandler globalHandler;
	return &globalHandler;
}

GlobalHandler::GlobalHandler()
{
	d = new GlobalHandlerPrivate();
}

GlobalHandler::~GlobalHandler()
{
	delete d;
	d = 0;
}

void GlobalHandler::setDumpPath(const QString& path)
{
	QString absPath = path;
	if(!QDir::isAbsolutePath(absPath)) {
		// If program uses QtGui module, we can use QDesktopServices
		//FIXME (AlekSi) this doesn't works anymore - it's a library without QtGui
#		if defined(QT_GUI_LIB)
			// this should be set for storageLocation
			Q_ASSERT(!qApp->applicationName().isEmpty());
			Q_ASSERT(!qApp->organizationName().isEmpty());
			qDebug("BreakpadQt: %s", qPrintable(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QLatin1String("/") + path));
			absPath = QDir::cleanPath(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QLatin1String("/") + path);
#		else
			absPath = QDir::cleanPath(qApp->applicationDirPath() + QLatin1String("/") + path);
#		endif

		qDebug("BreakpadQt: setDumpPath: %s -> %s", qPrintable(path), qPrintable(absPath));
	}
	Q_ASSERT(QDir::isAbsolutePath(absPath));

	QDir().mkpath(absPath);
	Q_ASSERT(QDir().exists(absPath));

#	if defined(Q_OS_WIN32)
		d->handler_->set_dump_path(absPath.toStdWString());
#	else
		d->handler_->set_dump_path(absPath.toStdString());
#	endif
}

void GlobalHandler::setReporter(const QString& reporter)
{
	d->reporter_ = reporter;

	if(!QDir::isAbsolutePath(d->reporter_)) {
#		if defined(Q_OS_MAC)
			// TODO(AlekSi) What to do if we are not inside bundle?
			d->reporter_ = QDir::cleanPath(qApp->applicationDirPath() + QLatin1String("/../Resources/") + d->reporter_);
#		elif defined(Q_OS_LINUX) || defined(Q_OS_WIN32)
			// MAYBE(AlekSi) Better place for Linux? libexec? or what?
			d->reporter_ = QDir::cleanPath(qApp->applicationDirPath() + QLatin1String("/") + d->reporter_);
#		else
			What is this?!
#		endif

		qDebug("BreakpadQt: setReporter: %s -> %s", qPrintable(reporter), qPrintable(d->reporter_));
	}
	Q_ASSERT(QDir::isAbsolutePath(d->reporter_));

	// add .exe for Windows if needed
#	if defined(Q_OS_WIN32)
		if(!QDir().exists(impl_->reporter_)) {
			d->reporter_ += QLatin1String(".exe");
		}
#	endif
	Q_ASSERT(QDir().exists(d->reporter_));
}

bool GlobalHandler::writeMinidump()
{
	bool res = d->handler_->WriteMinidump();
	if (res) {
		qDebug("BreakpadQt: writeMinidump() successed.");
	} else {
		qWarning("BreakpadQt: writeMinidump() failed.");
	}
	return res;
}

}	// namespace
