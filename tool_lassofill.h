#pragma once

#include "mw.h"

class Tool_lassofill
{
public:
    Tool_lassofill(){};;

    void press(QMouseEvent*);
    void move(QMouseEvent*);
    void release(QMouseEvent*);
    void preview();

    QPolygon stroke;
    QBrush lassofill_tool = QBrush(Qt::black);
};
