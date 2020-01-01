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
    bool isSub = false;
};

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

class Toolbar : public QWidget
{
    Q_OBJECT
public:
    Toolbar(MainWindow*, QWidget*);
    void setTool(Tool);
    void setSuboolbar(Subtoolbar* s) { subtoolbar = s; }
    Toolbarbutton* pen;
    Toolbarbutton* line;
    Toolbarbutton* lassofill;
    Toolbarbutton* eraser;
    Toolbarbutton* other;
    Toolbarbutton* clearimage;
    Toolbarbutton* knockback;
    Toolbarbutton* undo;
    Toolbarbutton* redo;

protected:
    void paintEvent(QPaintEvent*) override;

private:
    MainWindow* mainwindow;
    QWidget* parent;
    Subtoolbar* subtoolbar;
};

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

class Subtoolbar : public QWidget
{
    Q_OBJECT
public:
    Subtoolbar(MainWindow*, QWidget*);
    void setToolbar(Toolbar* t) { toolbar = t; }
    void clickSubtool(int);
    Toolbarbutton* p1;
    Toolbarbutton* p2;
    Toolbarbutton* p3;
    Toolbarbutton* p4;

private:
    MainWindow* mainwindow;
    QWidget* parent;
    Toolbar* toolbar;
};


#endif
