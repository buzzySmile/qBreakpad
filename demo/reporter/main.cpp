/*
 *  Copyright (C) 2009 Aleksey Palazhchenko
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

#include <QBreakpadHttpSender.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtNetwork/QNetworkReply>

int main(int argc, char* argv[])
{
	QCoreApplication app(argc, argv);
	if(argc < 2 || argc > 3) {
		qDebug("Usage: reporter <dump file> [host]");
		return 100;
	}

	QString file(app.arguments().at(1));
	QUrl host = (argc == 3 ? app.arguments().at(2) : QString::fromLatin1("http://localhost:8082"));

    QBreakpadHttpSender sender(host);
	app.connect(&sender, SIGNAL(finished(QNetworkReply::NetworkError)), SLOT(quit()));
	sender.uploadDump(file);
	app.exec();
}
