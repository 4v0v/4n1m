#ifndef TOOLBAR_H
#define TOOLBAR_H

#include "mainwindow.h"

class Toolbar : public QWidget
{
    Q_OBJECT
public:
    Toolbar(MainWindow*, QWidget*);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    MainWindow* mainwindow;
    QWidget* parent;

};

#endif
