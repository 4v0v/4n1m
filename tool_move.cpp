#include "tool_move.h"
#include "editor.h"
#include "animation.h"

Tool_move::Tool_move()
{
    preview_image = QImage(QSize(Mw::animation->dimensions.width() + 1, Mw::animation->dimensions.height() + 1), QImage::Format_ARGB32);
}

void Tool_move::press(QMouseEvent *e)
{
    Mw::editor->state = SCRIBBLING;

    moving_offset       = e->pos();
    moving_offset_delta = e->pos();
}

void Tool_move::move(QMouseEvent *e)
{
    moving_offset_delta = e->pos();

    Mw::editor->update();
};

void Tool_move::release(QMouseEvent *)
{
    Mw::editor->offset += moving_offset_delta - moving_offset;
    moving_offset       = moving_offset_delta;

    Mw::editor->state = IDLE;
    Mw::editor->update();
};

QImage* Tool_move::preview()
{
    return &preview_image;
};
