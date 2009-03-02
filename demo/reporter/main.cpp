/* This file is in public domain. */

#include <BreakpadHttpSender.h>

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

	BreakpadQt::HttpSender sender(host);
	app.connect(&sender, SIGNAL(finished(QNetworkReply::NetworkError)), SLOT(quit()));
	sender.uploadDump(file);
	app.exec();
}
