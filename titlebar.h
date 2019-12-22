#ifndef TITLEBAR_H
#define TITLEBAR_H

#include "mainwindow.h"

class Titlebar : public QWidget
{
    Q_OBJECT

public:
    Titlebar(QWidget* = nullptr);

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void paintEvent(QPaintEvent*) override;

private:
    QPoint p;
    int isDown = false;
};

#endif
