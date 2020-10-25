#pragma once

#include "mainwindow.h"
#include "animation.h"

class Preview : public QWidget
{
    Q_OBJECT
public:
    Preview();
    virtual void paintEvent(QPaintEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);

    QPainter widget_painter;

    void play_step();
    void play_from(int begin, bool loop);
    void stop();

    int frame_pos = 0;

    QTimer* playing_timer = new QTimer();
    State state = IDLE;
};


