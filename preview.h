#pragma once

#include "mw.h"
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

    void toggle_visibility();
    void toggle_play();
    void play_step();

    int frame_pos = 0;
    bool is_visible = true;
    QTimer* playing_timer = new QTimer();
    State state = IDLE;
    QPainter painter;
    QPoint delta_pos;
    QColor bg_color = Qt::gray;
};
