#pragma once

#include "mw.h"

class Toolbar : public QWidget
{
    Q_OBJECT
public:
    Toolbar();

    virtual void paintEvent(QPaintEvent* e);

    QPainter widget_painter;
};
