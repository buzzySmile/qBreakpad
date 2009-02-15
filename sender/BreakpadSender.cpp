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

#include "BreakpadSender.h"

#include <QDir>
#include <QByteArray>

#include <string>
#include <map>
using std::string;
using std::wstring;
using std::map;

#if defined(Q_OS_MAC)
#elif defined(Q_OS_LINUX)
#include "common/linux/http_upload.h"
#elif defined(Q_OS_WIN32)
#include "common/windows/http_upload.h"
#endif

namespace BreakpadQt
{

Sender::Sender(const QUrl& reportUrl)
	: m_reportUrl(reportUrl), m_requestSended(false)
{
}

Sender::~Sender()
{
}

void Sender::run()
{
	m_requestSended = false;
	m_responce.clear();
	m_errorString.clear();

#if defined(Q_OS_LINUX)
	string url = m_reportUrl.toString().toStdString();
	map<string, string> parameters;
	string file = m_filename.toStdString();
	string file_part_name("file");
	string responce;
	string error;
#elif defined(Q_OS_WIN32)
	wstring url = m_reportUrl.toString().toStdWString();
	map<wstring, wstring> parameters;
	wstring file = m_filename.toStdWString();
	wstring file_part_name; // FIXME (AlekSi) ("file");
	wstring responce;
	int error;
#endif

	// TODO (AlekSi) params map

	// TODO (AlekSi) checks of URL parts: username, password, etc. - strip it all
	m_requestSended = google_breakpad::HTTPUpload::SendRequest(url, parameters,
																	file, file_part_name,
#if defined(Q_OS_LINUX)
																	/* proxy */ string(), /* proxy_user_pwd */ string(),
#endif
																	&responce, &error);
#if defined(Q_OS_LINUX)
	m_responce = QString::fromStdString(responce);
	m_errorString = QString::fromStdString(error);
#elif defined(Q_OS_WIN32)
	m_responce = QString::fromStdWString(responce);
	m_errorString = QString::number(error);
#endif

	emit done(!m_requestSended);
}

void Sender::addParameter(const QLatin1String& key, const QString& value)
{
	Q_ASSERT(!QString(key).contains(QLatin1Char('"')));
	Q_ASSERT(!qstrcmp(key.latin1(), QString(key).toLatin1().constData()));
	m_params[key] = value;
}

void Sender::setFile(const QString& filename)
{
	Q_ASSERT(QDir::isAbsolutePath(filename));
	Q_ASSERT(QDir().exists(filename));
	m_filename = filename;
}

void Sender::sendRequest()
{
	start();
}

}	// namespace
