#include <QApplication>
#include "mw.h"

Mw* Mw::mainwindow;

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    Mw::mainwindow = new Mw;
    Mw::mainwindow->show();
    return application.exec();
}
