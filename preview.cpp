#include "preview.h"

Preview::Preview(): QWidget(nullptr)
{
    setGeometry(0, 0, 500, 500);
    setStyleSheet("background-color: red; border:0px");
    playing_timer->connect(playing_timer, &QTimer::timeout, this, [this]{ this->play_step();});
}

void Preview::mousePressEvent(QMouseEvent* e)
{
    if (state != IDLE) return;
    state = MOVING;
    delta_pos = e->pos();
}

void Preview::mouseMoveEvent(QMouseEvent* e)
{
    if (state != MOVING) return;
    move(QWidget::mapToParent(e->pos() - delta_pos));
}

void Preview::mouseReleaseEvent(QMouseEvent*)
{
    state = IDLE;
}

void Preview::paintEvent(QPaintEvent*)
{
    if (!is_visible) return;
    widget_painter.begin(this);

    //background
    Mw::set_painter_colors(&widget_painter, bg_color);
    widget_painter.drawRect(rect());

    //transform editor
    widget_painter.scale(
        1/((float)Mw::animation->dimensions.width()/ width()),
        1/((float)Mw::animation->dimensions.height()/ height())
    );

    //visible frame
    auto layer_keys = Mw::animation->layers.keys();
    auto ri = layer_keys.crbegin();
    while(ri != layer_keys.crend()) {
        widget_painter.setOpacity(Mw::animation->get_layer_at(*ri).opacity/100.0);
        auto frame = Mw::animation->is_frame_at(*ri, frame_pos)?
            Mw::animation->get_frame_at(*ri, frame_pos):
            Mw::animation->get_prev_frame_at(*ri, frame_pos);
        widget_painter.drawImage(frame.dimensions.topLeft(), frame.image);
        ++ri;
    }

    //reset painter
    widget_painter.setOpacity(1);
    widget_painter.resetTransform();

    widget_painter.end();
};

void Preview::play_step()
{
    if (frame_pos + 1 <= Mw::animation->get_last_anim_pos())
        frame_pos += 1;
     else
        frame_pos = 0;

    update();
}

void Preview::toggle_play()
{
    if (!is_visible) return;

    if (!playing_timer->isActive()) {
        frame_pos = 0;
        playing_timer->start(1000/Mw::animation->FPS);
     } else
        playing_timer->stop();
}

