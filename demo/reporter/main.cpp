/* This file is in public domain. */

#include <BreakpadSender.h>

#include <QCoreApplication>
#include <QTimer>
#include <QString>

int main(int argc, char* argv[])
{
	QCoreApplication app(argc, argv);
	BreakpadQt::Sender sender(QLatin1String(""));

	QTimer::singleShot(0, qApp, SLOT(quit()));
	return app.exec();
}
