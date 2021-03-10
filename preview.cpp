#include "preview.h"
#include "editor.h"

Preview::Preview(): QWidget(nullptr)
{
    setGeometry(0, 0, 500, 500);

    playing_timer->connect(playing_timer, &QTimer::timeout, this, [this]{ play_step();});
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
    painter.begin(this);

    // background
    Mw::set_painter_colors(&painter, bg_color);
    painter.drawRect(rect());

    // transform editor
    painter.scale(
        1/((float)Mw::animation->dimensions.width()/ width()),
        1/((float)Mw::animation->dimensions.height()/ height())
    );

    // visible frame
    auto layer_keys = Mw::animation->layers.keys();
    auto ri = layer_keys.crbegin();
    while(ri != layer_keys.crend()) {
        painter.setOpacity(Mw::animation->get_layer_at(*ri).opacity/100.0);
        auto frame = Mw::animation->is_frame_at(*ri, frame_pos) ? Mw::animation->get_frame_at(*ri, frame_pos): Mw::animation->get_prev_frame_at(*ri, frame_pos);
        painter.drawImage(frame.dimensions.topLeft(), frame.image);
        ++ri;
    }

    // tool preview
    if (frame_pos == Mw::editor->frame_pos) {
        QImage* preview;
        switch (Mw::editor->current_tool)
        {
            case PEN:         preview = Mw::editor->tool_pen->preview();         break;
            case LASSOFILL:   preview = Mw::editor->tool_lassofill->preview();   break;
            case ERASER:      preview = Mw::editor->tool_eraser->preview();      break;
            case KNOCKBACK:   preview = Mw::editor->tool_knockback->preview();   break;
            case COLORPICKER: preview = Mw::editor->tool_colorpicker->preview(); break;
            case MOVE:        preview = Mw::editor->tool_move->preview();        break;
            case SELECTION:   preview = Mw::editor->tool_selection->preview();   break;
        }

        if (preview) painter.drawImage(0,0, *preview);
    }


    // reset painter
    painter.setOpacity(1);
    painter.resetTransform();

    painter.end();
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

void Preview::toggle_visibility() {
    is_visible = !is_visible;

    if (!is_visible) setGeometry(0, 0,   0,   0);
    else             setGeometry(0, 0, 500, 500);

    playing_timer->stop();
    update();
}
