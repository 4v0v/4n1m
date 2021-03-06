#include "tool_colorpicker.h"
#include "editor.h"

void Tool_colorpicker::press(QMouseEvent* e)
{
    QPixmap pixmap = Mw::editor->grab();
    QImage image(pixmap.toImage());
    QColor color = image.pixelColor(e->pos());
    Mw::toolbar->color_wheel->set_color(color);
    Mw::editor->set_pen_color(color);
    Mw::editor->set_brush_color(color);
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

};

void Tool_colorpicker::preview(QImage*)
{

};