#include "timeline.h"

Timeline::Timeline(): QWidget(nullptr)
{
    setMinimumHeight(110);
    setMaximumHeight(110);

    QVBoxLayout* vlayout = new QVBoxLayout();

    for (int i = 0; i < 3; i++)
    {
       TimelineLayer* t = new TimelineLayer(i);
       vlayout->addWidget(t);
       layers.insert(i, t);
    };

    vlayout->addStretch(1);

    QWidget* main_widget = new QWidget();
    main_widget->setLayout( vlayout );

    timelineScroll = new QScrollArea(this);
    timelineScroll->setWidget(main_widget);
    timelineScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    timelineScroll->setStyleSheet("background-color: black; border:0px");
}

void Timeline::update_all_frames()
{
    foreach(auto layer, layers)
    {
        foreach(auto frame, layer->frames)
        {
            frame->is_key     = Mw::animation->has_frame_at(layer->position, frame->position);
            frame->is_current = Mw::editor->frame_pos == frame->position && Mw::editor->layer_pos == layer->position;
            frame->update();
        }
    };
};

void Timeline::wheelEvent(QWheelEvent* e)
{
    static bool can_scroll = false;

    can_scroll = !can_scroll;

    if (!can_scroll) return;

    if      (e->angleDelta().y() < 0) Mw::editor->goto_next_pos();
    else if (e->angleDelta().y() > 0) Mw::editor->goto_prev_pos();
};

void Timeline::resizeEvent(QResizeEvent* e)
{
    timelineScroll->setGeometry(0, 0, e->size().width(), e->size().height());
};

TimelineLayer::TimelineLayer(int p): QWidget()
{
    position = p;

    setMinimumHeight(20);
    setMaximumHeight(20);

    QHBoxLayout* hlayout = new QHBoxLayout();

    QSlider* opacity_slider = new QSlider();
    opacity_slider->setRange(0, 100);
    opacity_slider->setValue(100);

    connect(opacity_slider, &QAbstractSlider::valueChanged, this, [this, opacity_slider] {
        Mw::animation->set_layer_opacity(this->position, opacity_slider->value());
        Mw::update_all();
    });

    hlayout->addWidget(opacity_slider);

    for (int i = 0; i < 500; i++) {
        TimelineFrame* t = new TimelineFrame(position, i);

        hlayout->addWidget(t);
        frames.insert(i, t);
    };

    hlayout->addStretch(1);
    hlayout->setMargin(0);

    setLayout(hlayout);
};

void TimelineLayer::paintEvent(QPaintEvent*)
{
    static QPainter painter;

    painter.begin(this);
    painter.setPen  (Qt::black);
    painter.setBrush(Qt::black);

    painter.drawRect(rect());
    painter.end();
};

TimelineFrame::TimelineFrame(int l, int p): QWidget()
{
    layer_position = l;
    position       = p;

    setMinimumWidth(10);
    setMaximumWidth(10);
};

void TimelineFrame::paintEvent(QPaintEvent*)
{
    static QPainter painter;

    painter.begin(this);

    painter.setPen  (is_key ? Qt::darkGray : Qt::white);
    painter.setBrush(is_key ? Qt::darkGray : Qt::white);

    painter.drawRect(rect());

    if (is_current)
    {
        static QPen pen(Qt::red, 10);

        painter.setPen  (pen);
        painter.setBrush(Qt::transparent);
    }

    painter.drawRect(rect());

    painter.end();
};

void TimelineFrame::mousePressEvent(QMouseEvent*)
{
    Mw::editor->goto_pos(layer_position, position);

    update();
};

