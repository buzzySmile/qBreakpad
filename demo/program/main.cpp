/* This file is in public domain. */

#include <QCoreApplication>
#include <QTimer>
#include <QDateTime>

#include <BreakpadHandler.h>

#include "TestThread.h"

int main(int argc, char* argv[])
{
	QCoreApplication app(argc, argv);
	BreakpadQt::GlobalHandler handler(app.applicationDirPath() + "/crashes");

	qsrand(QDateTime::currentDateTime().toTime_t());
	TestThread t1(false, qrand());
	TestThread t2(true, qrand());

	t1.start();
	t2.start();

	QTimer::singleShot(3000, qApp, SLOT(quit()));
	BreakpadQt::GlobalHandler::instance()->writeMinidump();
	return app.exec();
}
