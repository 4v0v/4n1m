#include "timeline.h"

Timeline::Timeline(Mw* mw): QWidget(mw)
{
    setMinimumHeight(110);
    setMaximumHeight(110);
    QVBoxLayout* vlayout = new QVBoxLayout();
    for (int i = 0; i < 3; i++) {
       TimelineLayer* t = new TimelineLayer(i);
       vlayout->addWidget(t);
       layers.insert(i, t);
    };
    vlayout->addStretch(1);

    QWidget* main_widget = new QWidget();
    main_widget->setLayout( vlayout );

    timelineScroll = new QScrollArea(this);
    timelineScroll->setWidget(main_widget);
    timelineScroll->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn);
    timelineScroll->setStyleSheet("background-color: black; border:0px");
}

void Timeline::update_all_frames()
{
    foreach(TimelineLayer* layer, layers)
    {
        foreach(TimelineFrame* frame, layer->frames)
        {
            frame->is_key = Mw::animation->is_frame_at(layer->position, frame->position);
            frame->is_current = Mw::editor->frame_pos == frame->position && Mw::editor->layer_pos == layer->position;
            frame->update();
        }
    };
};

void Timeline::wheelEvent(QWheelEvent* e)
{
    static bool doubleWheelEventBug = false;
    doubleWheelEventBug = !doubleWheelEventBug;

    if (!doubleWheelEventBug) return;
    if (e->angleDelta().y() < 0) Mw::editor->goto_next_pos();
    else if (e->angleDelta().y() > 0) Mw::editor->goto_prev_pos();
};

void Timeline::resizeEvent(QResizeEvent* e)
{
    timelineScroll->setGeometry(0, 0, e->size().width(), e->size().height());
};

//////////

TimelineLayer::TimelineLayer(int p): QWidget()
{
    position = p;
    setMinimumHeight(20);
    setMaximumHeight(20);

    QHBoxLayout* hlayout = new QHBoxLayout();

    QSlider* s = new QSlider();
    s->setRange(0, 100);
    s->setValue(100);
    connect(s, &QAbstractSlider::valueChanged, this, [this, s]{
        Mw::animation->set_layer_opacity(this->position, s->value());
        Mw::update_editor_and_timeline();
    });
    hlayout->addWidget(s);

    for (int i = 0; i < 500; i++) {
        TimelineFrame* t = new TimelineFrame(position, i);
        hlayout->addWidget(t);
        frames.insert(i, t);
    };

    hlayout->addStretch(1);
    hlayout->setMargin(0);
    setLayout( hlayout );
};

void TimelineLayer::paintEvent(QPaintEvent*)
{
    widget_painter.begin(this);
    Mw::set_painter_colors(&widget_painter, Qt::black);
    widget_painter.drawRect(rect());
    widget_painter.end();
};

//////////

TimelineFrame::TimelineFrame(int l, int p): QWidget()
{
    layer_position = l;
    position = p;

    setMinimumWidth(20);
    setMaximumWidth(20);
};

void TimelineFrame::paintEvent(QPaintEvent*)
{
    widget_painter.begin(this);
    if (is_key) Mw::set_painter_colors(&widget_painter, Qt::darkGray);
    else Mw::set_painter_colors(&widget_painter, Qt::white);
    widget_painter.drawRect(rect());

    if (is_current)
    {
        Mw::set_painter_colors(&widget_painter, Qt::green, Qt::transparent);
        widget_painter.setPen(QPen(Qt::red, 10));
    }
    widget_painter.drawRect(rect());
    widget_painter.end();
};

void TimelineFrame::mousePressEvent(QMouseEvent*)
{
    Mw::editor->goto_pos(layer_position, position);
    update();
};

