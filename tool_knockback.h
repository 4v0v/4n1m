#pragma once

#include "mw.h"

class Tool_knockback
{
public:
    Tool_knockback();

    void press  (QMouseEvent*);
    void move   (QMouseEvent*);
    void release(QMouseEvent*);

    QImage* preview();

    QImage    preview_image;
    QVector2D position;
    QVector2D delta_position;
    int       knockback_amount;
};
