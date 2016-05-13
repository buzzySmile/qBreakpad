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
 
#ifndef QBREAKPAD_HTTP_SENDER_H
#define QBREAKPAD_HTTP_SENDER_H

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

class QString;
class QUrl;
class QFile;

class QBreakpadHttpSender : public QObject
{
	Q_OBJECT
public:
    QBreakpadHttpSender(const QUrl& url);
    ~QBreakpadHttpSender();

    //TODO: proxy, ssl

    void uploadDump(const QString& fileName);

    QString remoteUrl() const;

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

#endif	// QBREAKPAD_HTTP_SENDER_H
