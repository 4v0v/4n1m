#ifndef TITLEBAR_H
#define TITLEBAR_H

#include "mainwindow.h"

class Titlebar : public QWidget
{
    Q_OBJECT

public:
    Titlebar(MainWindow*);

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

    QPoint p;
    int isDown = false;
};

#endif
