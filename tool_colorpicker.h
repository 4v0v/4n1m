#pragma once

#include "mw.h"

class Tool_colorpicker
{
public:
    Tool_colorpicker(){};;

    void press(QMouseEvent*);
    void move(QMouseEvent*);
    void release(QMouseEvent*);
    void preview(QImage*);
};
