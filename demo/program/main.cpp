/* This file is in public domain. */

#include "TestThread.h"

#include <BreakpadHandler.h>

#include <QCoreApplication>
#include <QTimer>
#include <QDateTime>

int main(int argc, char* argv[])
{
	QCoreApplication app(argc, argv);
	BreakpadQt::GlobalHandler handler(app.applicationDirPath() + QLatin1String("/crashes"),
										app.applicationDirPath() + QLatin1String("/../reporter/reporter"));

	qsrand(QDateTime::currentDateTime().toTime_t());
	TestThread t1(false, qrand());
	TestThread t2(true, qrand());

	t1.start();
	t2.start();

	QTimer::singleShot(3000, qApp, SLOT(quit()));
	return app.exec();
}
