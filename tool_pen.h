#pragma once

#include "mw.h"

class Tool_pen
{
public:
    Tool_pen(){};;

    void press(QMouseEvent*);
    void move(QMouseEvent*);
    void release(QMouseEvent*);
    void preview(QImage*);

    QPolygon stroke;
    QPen pen_tool = QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
};
