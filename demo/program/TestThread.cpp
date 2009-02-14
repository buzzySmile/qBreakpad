/* This file is in public domain. */

#include "TestThread.h"

#include <QTimer>

TestThread::TestThread(bool buggy, uint seed)
	: m_buggy(buggy)
{
	qsrand(seed);
}

TestThread::~TestThread()
{
}

void TestThread::crash()
{
	reinterpret_cast<QString*>(1)->toInt();
	throw 1;
}

void TestThread::run()
{
	if(m_buggy) {
		QTimer::singleShot(qrand() % 2000 + 100, this, SLOT(crash()));
	} else {
		QTimer::singleShot(qrand() % 2000 + 100, this, SLOT(quit()));
	}
	exec();
}
