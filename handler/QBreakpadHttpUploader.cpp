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
 
#include <QString>
#include <QUrl>
#include <QFile>
#include <QDir>
#include <QNetworkReply>
#include <QCoreApplication>

#include "QBreakpadHttpUploader.h"

QBreakpadHttpUploader::QBreakpadHttpUploader(const QUrl& url) :
    m_file(0)
{
	m_request.setUrl(url);
}

QBreakpadHttpUploader::~QBreakpadHttpUploader()
{
	if(m_reply) {
		qWarning("m_reply is not NULL");
		m_reply->deleteLater();
	}

	delete m_file;
}

void QBreakpadHttpUploader::uploadDump(const QString& fileName)
{
    Q_ASSERT(!m_file);
    Q_ASSERT(!m_reply);
    Q_ASSERT(QDir().exists(fileName));

    m_file = new QFile(fileName);
    if(!m_file->open(QIODevice::ReadOnly)) return;

    /*
     * GenerateRequestHeader
     * header = L"Content-Type: multipart/form-data; boundary="
     * +
     * GenerateMultipartBoundary()
     * {
        // The boundary has 27 '-' characters followed by 16 hex digits
        static const wchar_t kBoundaryPrefix[] = L"---------------------------";
        static const int kBoundaryLength = 27 + 16 + 1;

        // Generate some random numbers to fill out the boundary
        int r0 = rand();
        int r1 = rand();

        return QString("%s%08X%08X").arg(kBoundaryPrefix).arg(r0).arg(r1);
    }
    */

    QByteArray data;  //(QString(generateMultipartBoundary() + "\r\n").toLatin1());

    QMap<QString, QString> params;
    params["prod"] = qApp->applicationName();
    params["ver"] = qApp->applicationVersion();

    // Append each of the parameter pairs as a form-data part
    QMapIterator<QString, QString> i(params);
    while (i.hasNext()) {
        i.next();
        data.append(generateMultipartBoundary().toLatin1() + "\r\n");     //according to rfc 1867
        data.append("Content-Disposition: form-data; name=\"" +
                     i.key() + "\"\r\n\r\n" +
                     i.value() + "\r\n");
    }

    data.append(generateMultipartBoundary().toLatin1() + "\r\n");     //according to rfc 1867
    QStringList filePathParts = fileName.split("/");
    data.append("Content-Disposition: form-data; "
                     "name=\"upload_file_minidump\"; "
                     "filename=\"" + filePathParts.at(filePathParts.count() - 1) + "\"\r\n");
    data.append("Content-Type: application/octet-stream\r\n");
    data.append("\r\n");
    // read the file
    data.append(m_file->readAll());
    data.append("\r\n");

    data.append(generateMultipartBoundary().toLatin1() + "--\r\n"); //closing boundary according to rfc 1867

    m_request.setRawHeader(QString("Content-Type").toLatin1(), QString("multipart/form-data; boundary=" + generateMultipartBoundary()).toLatin1());
    m_request.setRawHeader(QString("Content-Length").toLatin1(), QString::number(data.length()).toLatin1());

    qDebug() << data;

    m_reply = m_manager.post(m_request, data);
    connect(m_reply, SIGNAL(uploadProgress(qint64, qint64)),
            this,      SLOT(onUploadProgress(qint64,qint64)));
    connect(m_reply, SIGNAL(finished()),
            this,      SLOT(onUploadFinished()));
}

QString QBreakpadHttpUploader::remoteUrl() const
{
    return m_request.url().toString();
}

void QBreakpadHttpUploader::onUploadProgress(qint64 sent, qint64 total)
{
    qDebug("upload progress: %lld/%lld", sent, total);
}

void QBreakpadHttpUploader::onUploadFinished()
{
	if(m_reply->error() != QNetworkReply::NoError) {
        qWarning("Upload error: %d - %s", m_reply->error(), qPrintable(m_reply->errorString()));
	} else {
        qDebug() << "Upload to " << remoteUrl() << " complete!";
	}
	emit finished(m_reply->error());

	m_reply->close();
	m_reply->deleteLater();
	m_reply = 0;

	delete m_file;
	m_file = 0;
}

QString QBreakpadHttpUploader::generateMultipartBoundary()
{
    // The boundary has 27 '-' characters followed by 16 hex digits
    //QString boundaryPrefix = QLatin1String("---------------------------");
    //int boundaryLength = 27 + 16 + 1;

    // Generate some random numbers to fill out the boundary
    int r0 = qrand();
    int r1 = qrand();

    return QString("%1%2%3").arg("---------------------------").arg(r0, 8).arg(r1, 8);
}
