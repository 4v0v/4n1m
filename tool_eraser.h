#pragma once

#include "mw.h"

class Tool_eraser
{
public:
    Tool_eraser();

    void press(QMouseEvent*);
    void move(QMouseEvent*);
    void release(QMouseEvent*);
    QImage* preview();

    QImage preview_image;
    QPolygon stroke;
    QPen eraser_tool = QPen(Qt::green, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
};
