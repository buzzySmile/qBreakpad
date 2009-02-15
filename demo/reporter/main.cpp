/* This file is in public domain. */

#include <BreakpadSender.h>

#include <QCoreApplication>
#include <QTimer>
#include <QString>
#include <QUrl>
#include <QDebug>

int main(int argc, char* argv[])
{
	QCoreApplication app(argc, argv);
	BreakpadQt::Sender sender(QUrl(QLatin1String("http://localhost:8080/breakpad-test/receiver")));
	sender.addParameter(QLatin1String("param1"), QString::fromLatin1("value1"));
	sender.setFile(qApp->applicationFilePath());
	app.connect(&sender, SIGNAL(done(bool)), SLOT(quit()));
	sender.sendRequest();
	int res = app.exec();
	sender.wait();
	return res;
}
