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

#include "BreakpadHttpSender.h"

#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtNetwork/QNetworkReply>

namespace BreakpadQt
{

HttpSender::HttpSender(const QUrl& url)
	: m_file(0)
{
	m_request.setUrl(url);
}

HttpSender::~HttpSender()
{
	if(m_reply) {
		qWarning("m_reply is not NULL");
		m_reply->deleteLater();
	}

	delete m_file;
}

void HttpSender::uploadDump(const QString& fileName)
{
	Q_ASSERT(!m_file);
	Q_ASSERT(!m_reply);
	Q_ASSERT(QDir().exists(fileName));

	m_file = new QFile(fileName);
	m_file->open(QIODevice::ReadOnly);
	m_reply = m_manager.post(m_request, m_file);
	connect(m_reply, SIGNAL(uploadProgress(qint64, qint64)), SLOT(onUploadProgress(qint64,qint64)));
	connect(m_reply, SIGNAL(finished()), SLOT(onUploadFinished()));
}

void HttpSender::onUploadProgress(qint64 sent, qint64 total)
{
	qDebug("upload: %lld/%lld", sent, total);
}

void HttpSender::onUploadFinished()
{
	if(m_reply->error() != QNetworkReply::NoError) {
		qWarning("upload error: %d - %s", m_reply->error(), qPrintable(m_reply->errorString()));
	} else {
		qDebug("upload complete");
	}
	emit finished(m_reply->error());

	m_reply->close();
	m_reply->deleteLater();
	m_reply = 0;

	delete m_file;
	m_file = 0;
}

}	// namespace
