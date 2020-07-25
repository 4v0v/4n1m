#include "toolbar.h"
#include "colorwheel.h"
#include "editor.h"

Toolbar::Toolbar(Mw* mw): QWidget(mw)
{
    QVBoxLayout* vlayout    = new QVBoxLayout;
    ColorWheel* color_wheel = new ColorWheel(this);
    QWidget* widget         = new QWidget(this);

    widget->setStyleSheet("background-color: black;");
    vlayout->setSpacing(0);
    vlayout->setMargin(0);
    vlayout->addWidget(color_wheel, 1);
    vlayout->addWidget(widget, 5);
    setLayout(vlayout);

    connect(color_wheel, &ColorWheel::select_color, this, [color_wheel](){
            Mw::editor->set_pen_color(color_wheel->color());
            Mw::editor->set_brush_color(color_wheel->color());
    });
}
