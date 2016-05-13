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
 
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtNetwork/QNetworkReply>

#include "QBreakpadHttpSender.h"

QBreakpadHttpSender::QBreakpadHttpSender(const QUrl& url) :
    m_file(0)
{
	m_request.setUrl(url);
}

QBreakpadHttpSender::~QBreakpadHttpSender()
{
	if(m_reply) {
		qWarning("m_reply is not NULL");
		m_reply->deleteLater();
	}

	delete m_file;
}

void QBreakpadHttpSender::uploadDump(const QString& fileName)
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

QString QBreakpadHttpSender::remoteUrl() const
{
    return m_request.url().toString();
}

void QBreakpadHttpSender::onUploadProgress(qint64 sent, qint64 total)
{
	qDebug("upload: %lld/%lld", sent, total);
}

void QBreakpadHttpSender::onUploadFinished()
{
	if(m_reply->error() != QNetworkReply::NoError) {
		qWarning("upload error: %d - %s", m_reply->error(), qPrintable(m_reply->errorString()));
	} else {
        qDebug() << "upload to " << remoteUrl() << " complete!";
	}
	emit finished(m_reply->error());

	m_reply->close();
	m_reply->deleteLater();
	m_reply = 0;

	delete m_file;
	m_file = 0;
}
