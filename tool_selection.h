#pragma once

#include "mw.h"

class Tool_selection
{
public:
    Tool_selection();

    void press(QMouseEvent*);
    void move(QMouseEvent*);
    void release(QMouseEvent*);
    QImage* preview();

    QImage preview_image;

    QRect selection_zone;
    QRect initial_selection_zone;

    QPoint selection_zone_delta_pos;
    QImage selected_image = QImage(1, 1, QImage::Format_ARGB32);
    State state = SELECTION_EMPTY;


//    Tool subtool = RECTANGLE;
//    QPolygon initialPolyZone;
//    QRect initialRectZone = QRect(0,0,1,1);
//    QRect deltaRectZone = QRect(0,0,1,1);
//    QImage deltaImage = QImage(1, 1, QImage::Format_ARGB32);
//    QPoint deltaPosition = QPoint(0, 0);
//    bool pasted = false;
};
