#pragma once

#include "mw.h"
#include "colorwheel.h"

class Toolbar : public QWidget
{
    Q_OBJECT
public:
    Toolbar();

    virtual void paintEvent(QPaintEvent* e);

    QPainter widget_painter;
    ColorWheel* color_wheel;
};
