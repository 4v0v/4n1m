#include <QApplication>
#include "mw.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    Mw mainwindow;
    mainwindow.show();
    return application.exec();
}
