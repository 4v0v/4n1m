#include "toolbar.h"
#include "colorwheel.h"
#include "editor.h"

Toolbar::Toolbar(Mw* mw): QWidget(mw)
{
    QVBoxLayout* vlayout    = new QVBoxLayout;
    QWidget* widget         = new QWidget(this);
    ColorWheel* color_wheel = new ColorWheel(this);
    QPushButton* pen_button = new QPushButton("Pen");
    QPushButton* lasso_button = new QPushButton("Lasso fill");
    QSlider* pen_width_slider = new QSlider();
    pen_width_slider->setRange(1, 10);
    pen_width_slider->setValue(3);
    pen_width_slider->setOrientation(Qt::Horizontal);

    widget->setStyleSheet("background-color: black;");
    vlayout->setSpacing(0);
    vlayout->setMargin(0);
    vlayout->addWidget(color_wheel, 1);
    vlayout->addWidget(pen_width_slider);
    vlayout->addWidget(pen_button);
    vlayout->addWidget(lasso_button);
    vlayout->addWidget(widget, 5);
    setLayout(vlayout);

    connect(pen_button, &QPushButton::pressed, this, []{
            Mw::editor->set_tool(PEN);
    });

    connect(lasso_button, &QPushButton::pressed, this, []{
            Mw::editor->set_tool(LASSOFILL);
    });

    connect(pen_width_slider, &QAbstractSlider::valueChanged, this, [pen_width_slider]{
        Mw::editor->set_pen_size(pen_width_slider->value());
    });

    connect(color_wheel, &ColorWheel::select_color, this, [color_wheel](){
            Mw::editor->set_pen_color(color_wheel->color());
            Mw::editor->set_brush_color(color_wheel->color());
    });
}
