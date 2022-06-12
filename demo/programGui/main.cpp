#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QBreakpadInstance.setDumpPath("crashes");
    WinVeh::AddVeh();

    MainWindow w;
    w.show();
    return a.exec();
}
