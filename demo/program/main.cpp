/* This file is in public domain. */

#include "TestThread.h"

#include <BreakpadHandler.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtCore/QDateTime>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName(QLatin1String("AppName"));
    app.setApplicationVersion(QLatin1String("1.0"));
    app.setOrganizationName(QLatin1String("OrgName"));
    app.setOrganizationDomain(QLatin1String("name.org"));

    BreakpadQt::GlobalHandler::instance()->setDumpPath(QLatin1String("crashes"));

    qsrand(QDateTime::currentDateTime().toTime_t());
    TestThread t1(false, qrand());
    TestThread t2(true, qrand());

    t1.start();
    t2.start();

    QTimer::singleShot(3000, qApp, SLOT(quit()));
    return app.exec();
}
