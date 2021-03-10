#pragma once

#include "mw.h"

class Tool_selection
{
public:
    Tool_selection();

    void press(QMouseEvent*);
    void move(QMouseEvent*);
    void release(QMouseEvent*);
    void reset();
    QImage* preview();

    QImage preview_image;
    QRect selected_zone            = QRect(0, 0, 1, 1);;
    QRect initial_selected_zone    = QRect(0, 0, 1, 1);;
    QPoint selected_zone_delta_pos = QPoint(0, 0);
    bool mirrored_h = false;
    bool mirrored_v = false;
    QImage selected_image          = QImage(1, 1, QImage::Format_ARGB32);
    State state                    = SELECTION_EMPTY;
};
