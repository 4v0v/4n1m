#include "tool_colorpicker.h"
#include "editor.h"
#include "animation.h"

Tool_colorpicker::Tool_colorpicker() {
    preview_image = QImage(QSize(Mw::animation->dimensions.width()+1, Mw::animation->dimensions.height() +1), QImage::Format_ARGB32);
}

void Tool_colorpicker::press(QMouseEvent* e)
{
    Mw::editor->state = SCRIBBLING;

    QPixmap pixmap = Mw::editor->grab();
    QImage image(pixmap.toImage());

    QColor color = image.pixelColor(e->pos());

    Mw::toolbar->color_wheel->set_color(color);
    Mw::editor->set_pen_color(color);
    Mw::editor->set_brush_color(color);

    Mw::editor  ->update();
    Mw::timeline->update();
}

void Tool_colorpicker::move(QMouseEvent* e)
{
    QPixmap pixmap = Mw::editor->grab();
    QImage image(pixmap.toImage());

    // TODO: only check when e->pos is inside image to avoid error out of range
    QColor color = image.pixelColor(e->pos());

    Mw::toolbar->color_wheel->set_color(color);
    Mw::editor->set_pen_color(color);
    Mw::editor->set_brush_color(color);
};

void Tool_colorpicker::release(QMouseEvent*)
{
    Mw::editor->state = IDLE;
    Mw::editor->update();
};

QImage* Tool_colorpicker::preview()
{
    return &preview_image;
};
