#pragma once

#include "mw.h"

class Tool_move
{
public:
    Tool_move();

    void press  (QMouseEvent*);
    void move   (QMouseEvent*);
    void release(QMouseEvent*);

    QImage* preview();

    QImage preview_image;
    QPoint moving_offset_delta;
    QPoint moving_offset;
};
