#pragma once

#include "mw.h"

class Tool_move
{
public:
    Tool_move(){};;

    void press(QMouseEvent*);
    void move(QMouseEvent*);
    void release(QMouseEvent*);
    void preview(QImage*);

    QPoint moving_offset_delta;
    QPoint moving_offset;
};
