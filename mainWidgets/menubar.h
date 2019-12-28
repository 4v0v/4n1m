#ifndef MENUBAR_H
#define MENUBAR_H

#include "mainwindow.h"

class Menubar : public QMenuBar
{
    Q_OBJECT

public:
    Menubar(MainWindow*);

protected:

private:
    MainWindow* mainwindow;

};

#endif
