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

#ifndef BREAKPAD_HTTP_SENDER_H
#define BREAKPAD_HTTP_SENDER_H

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
class QString;
class QUrl;
class QFile;

namespace BreakpadQt
{

class HttpSender : public QObject
{
	Q_OBJECT
public:
	HttpSender(const QUrl& url);
	~HttpSender();

	//TODO(AlekSi) proxy, ssl

	void uploadDump(const QString& fileName);

signals:
	void finished(QNetworkReply::NetworkError);

private slots:
	void onUploadProgress(qint64 sent, qint64 total);
	void onUploadFinished();

private:
	QNetworkAccessManager m_manager;
	QNetworkRequest m_request;
	QPointer<QNetworkReply> m_reply;
	QFile* m_file;
};

}	// namespace

#endif	// BREAKPAD_HTTP_SENDER_H
