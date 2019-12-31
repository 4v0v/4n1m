#ifndef TITLEBAR_H
#define TITLEBAR_H

#include "mainwindow.h"

class Menubar : public QMenuBar
{
    Q_OBJECT

public:
    Menubar(MainWindow*);
    QMenu* getOptionsMenu() { return options; }
    QMenu* getToolsMenu() { return tools; }
protected:
    void paintEvent(QPaintEvent*) override;

private:
    MainWindow* mainwindow;

    QMenu* options;
    QMenu* tools;
};

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

class Titlebar : public QWidget
{
    Q_OBJECT

public:
    Titlebar(MainWindow*);
    Menubar* getMenubar() { return menubar; }

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void paintEvent(QPaintEvent*) override;

public slots:
    void closeWindow() { window()->close(); }
    void minimizeWindow() { window()->setWindowState(Qt::WindowState::WindowMinimized);}

private:
    MainWindow* mainwindow;

    Menubar* menubar;
    QPoint p;
    int isDown = false;
};

#endif
