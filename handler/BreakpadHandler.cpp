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

#include <QDir>
#include <QProcess>
#include <QCoreApplication>

#if defined(Q_OS_MAC)
#include "client/mac/handler/exception_handler.h"
#elif defined(Q_OS_LINUX)
#include "client/linux/handler/exception_handler.h"
#elif defined(Q_OS_WIN32)
#include "client/windows/handler/exception_handler.h"
#endif

namespace BreakpadQt
{

static QString reporterFullFileName_;

/*	From exception_handler.h:

	A callback function to run after the minidump has been written.
	minidump_id is a unique id for the dump, so the minidump
	file is <dump_path>\<minidump_id>.dmp.  context is the parameter supplied
	by the user as callback_context when the handler was created.  succeeded
	indicates whether a minidump file was successfully written.

	If an exception occurred and the callback returns true, Breakpad will
	treat the exception as fully-handled, suppressing any other handlers from
	being notified of the exception.  If the callback returns false, Breakpad
	will treat the exception as unhandled, and allow another handler to handle
	it. If there are no other handlers, Breakpad will report the exception to
	the system as unhandled, allowing a debugger or native crash dialog the
	opportunity to handle the exception.  Most callback implementations
	should normally return the value of |succeeded|, or when they wish to
	not report an exception of handled, false.  Callbacks will rarely want to
	return true directly (unless |succeeded| is true).
*/
#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
bool MDCallback(const char* _dump_dir,
				const char* _minidump_id,
				void *context, bool success)
#elif defined(Q_OS_WIN32)
bool MDCallback(const wchar_t* _dump_dir,
				const wchar_t* _minidump_id,
				void* context,
				EXCEPTION_POINTERS* exinfo,
				MDRawAssertionInfo* assertion,
				bool success)
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

	QProcess::startDetached(reporterFullFileName_);
	return success;
}

google_breakpad::ExceptionHandler* GlobalHandler::handler_ = 0;
GlobalHandler* GlobalHandler::instance_ = 0;

GlobalHandler::GlobalHandler(const QString& minidumpPath, const QString& reporter)
{
	if(instance_) {
		qWarning("BreakpadQt: GlobalHandler already exists!");
		return;
	}

	// path to crash reporter
	if(reporter.isNull()) {
#	if defined(Q_OS_MAC)
		// TODO what if we are not inside bundle?
		reporterFullFileName_ = QDir::cleanPath(qApp->applicationDirPath() + QLatin1String("/../Resources/crashreporter"));
#	elif defined(Q_OS_LINUX)
		// MAYBE better default place? libexec? or what?
		reporterFullFileName_ = qApp->applicationDirPath() + QLatin1String("/crashreporter");
#	elif defined(Q_OS_WIN32)
		reporterFullFileName_ = qApp->applicationDirPath() + QLatin1String("/crashreporter.exe");
#	else
		What is this?!
#	endif
	} else {
		reporterFullFileName_ = reporter;
		if(!QDir::isAbsolutePath(reporterFullFileName_)) {
			reporterFullFileName_ = QDir::cleanPath(qApp->applicationDirPath() + QLatin1String("/") + reporter);
		}
	}
	Q_ASSERT(QDir::isAbsolutePath(reporterFullFileName_));
	Q_ASSERT(QDir().exists(reporterFullFileName_));

	// path to minidumps
	Q_ASSERT(QDir::isAbsolutePath(minidumpPath));
	QDir().mkpath(minidumpPath);
	Q_ASSERT(QDir().exists(minidumpPath));

	handler_ = new google_breakpad::ExceptionHandler(
#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
		minidumpPath.toUtf8().data(),
#elif defined(Q_OS_WIN32)
		minidumpPath.toStdWString(),
#endif
		/*FilterCallback*/ 0, MDCallback, /*context*/ 0, true);

	instance_ = this;
}

GlobalHandler::~GlobalHandler()
{
	if(this == instance_) {
		delete handler_;
		handler_ = 0;

		reporterFullFileName_.clear();

		instance_ = 0;
	}
}

bool GlobalHandler::writeMinidump()
{
	bool res = handler_->WriteMinidump();
	if (res) {
		qDebug("BreakpadQt: writeMinidump() successed.");
	} else {
		qWarning("BreakpadQt: writeMinidump() failed.");
	}
	return res;
}

}	// namespace
