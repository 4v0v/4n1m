#pragma once

#include "mw.h"

class Tool_selection
{
public:
    Tool_selection();

    void press  (QMouseEvent*);
    void move   (QMouseEvent*);
    void release(QMouseEvent*);

    QImage* preview();
    void init_values();
    void draw_on_frame();

    QImage preview_image;
    QRect  selected_zone;
    QRect  initial_selected_zone;
    QPoint selected_zone_delta_pos;
    bool   is_mirrored_h;
    bool   is_mirrored_v;
    bool   is_copying;
    QImage selected_image;
    State  state;
};
