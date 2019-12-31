#ifndef TOOLBAR_H
#define TOOLBAR_H

#include "mainwindow.h"

class Toolbarbutton : public QPushButton
{
    Q_OBJECT
public:
    Toolbarbutton(MainWindow*, QWidget*, QString, int, int, int, int, bool = false);
    void setCurrentStyle();
    void setIscurrent(bool b) { isCurrent = b; }

private:
    MainWindow* mainwindow;
    QWidget* parent;
    QString text;
    bool isCurrent = false;
};

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

class Toolbar : public QWidget
{
    Q_OBJECT
public:
    Toolbar(MainWindow*, QWidget*);
    void checkTool(Tool);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    MainWindow* mainwindow;
    QWidget* parent;

    Toolbarbutton* pen;
    Toolbarbutton* line;
    Toolbarbutton* lassofill;
    Toolbarbutton* eraser;
    Toolbarbutton* other;
    Toolbarbutton* undo;
    Toolbarbutton* redo;
};

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

class Subtoolbar : public QWidget
{
    Q_OBJECT
public:
    Subtoolbar(MainWindow*, QWidget*);

private:
    MainWindow* mainwindow;
    QWidget* parent;

    Toolbarbutton* p1;
    Toolbarbutton* p2;
    Toolbarbutton* p3;
    Toolbarbutton* p4;
};


#endif
