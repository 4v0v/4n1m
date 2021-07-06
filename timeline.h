#pragma once

#include "mw.h"
#include "animation.h"
#include "editor.h"

class Timeline : public QWidget
{
    Q_OBJECT
public:
    Timeline();

    virtual void wheelEvent (QWheelEvent*  e);
    virtual void resizeEvent(QResizeEvent* e);

    void update_all_frames();

    QList<TimelineLayer*> layers;
    QScrollArea*          timelineScroll;
};

class TimelineLayer : public QWidget
{
    Q_OBJECT
public:
    TimelineLayer(int p);

    virtual void paintEvent(QPaintEvent *e);

    QList<TimelineFrame*> frames;
    int                   position;
};

class TimelineFrame : public QWidget
{
    Q_OBJECT
public:
    TimelineFrame(int l, int p);

    virtual void paintEvent     (QPaintEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);

    int  position;
    int  layer_position;
    bool is_key     = false;
    bool is_current = false;
};



