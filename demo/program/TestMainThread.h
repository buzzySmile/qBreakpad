#ifndef TESTMAINTHREAD_H
#define TESTMAINTHREAD_H

#include <QObject>
#include <QBreakpadHandler.h>

class TestMainThread : public QObject
{
    Q_OBJECT
public:
    explicit TestMainThread(QObject *parent = nullptr);

private slots:
    void crash() {
        QBreakpadHandler::toCrash();
    }
signals:

};

#endif // TESTMAINTHREAD_H
