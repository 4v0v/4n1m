#pragma once

#include "mw.h"

class Tool_colorpicker
{
public:
    Tool_colorpicker();

    void press(QMouseEvent*);
    void move(QMouseEvent*);
    void release(QMouseEvent*);
    QImage* preview();

    QImage preview_image;
};
