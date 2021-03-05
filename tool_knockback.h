#pragma once

#include "mw.h"

class Tool_knockback
{
public:
    Tool_knockback(){};;

    void press(QMouseEvent*);
    void move(QMouseEvent*);
    void release(QMouseEvent*);
    void preview(QImage*);

    int knockback_amount = 255/3;
};
