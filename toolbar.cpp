#include "toolbar.h"
#include "editor.h"
#include "preview.h"

Toolbar::Toolbar(): QWidget(nullptr)
{
    setGeometry(0, 0, 150, 480);

    color_wheel = new ColorWheel(this);

    QCheckBox* button_preview_show  = new QCheckBox("Preview", this);
    QCheckBox* button_copy_previous = new QCheckBox("Copy previous frame", this);
    QCheckBox* button_onion_show    = new QCheckBox("Onion skin", this);
    QCheckBox* button_onion_next    = new QCheckBox("Onion next", this);
    QCheckBox* button_onion_prev    = new QCheckBox("Onion prev", this);
    QCheckBox* button_onion_loop    = new QCheckBox("Onion loop", this);

    button_preview_show->setChecked(Mw::preview->is_visible);
    button_copy_previous->setChecked(Mw::editor->is_copy_prev_frame);
    button_onion_show->setChecked(Mw::editor->is_os_enabled);
    button_onion_next->setChecked(Mw::editor->is_os_next_enabled);
    button_onion_prev->setChecked(Mw::editor->is_os_prev_enabled);
    button_onion_loop->setChecked(Mw::editor->is_os_loop_enabled);

    QRadioButton *radio_pen         = new QRadioButton("PEN", this);
    QRadioButton *radio_lasso       = new QRadioButton("LASSO FILL", this);
    QRadioButton *radio_eraser      = new QRadioButton("ERASER", this);
    QRadioButton *radio_knockback   = new QRadioButton("KNOCKBACK", this);
    QRadioButton *radio_colorpicker = new QRadioButton("COLOR PICKER", this);
    QRadioButton *radio_move        = new QRadioButton("MOVE", this);
    QRadioButton *radio_selection   = new QRadioButton("SELECTION", this);

    radio_pen->toggle();

    QSlider* pen_width_slider = new QSlider();
    pen_width_slider->setRange(1, 10);
    pen_width_slider->setValue(Mw::editor->tool_pen->pen_tool.width());
    pen_width_slider->setOrientation(Qt::Horizontal);

    QSlider* fps_slider = new QSlider();
    fps_slider->setRange(1, 60);
    fps_slider->setValue(Mw::animation->FPS);
    fps_slider->setOrientation(Qt::Horizontal);

    QPushButton* save_file   = new QPushButton("Save");
    QPushButton* load_file   = new QPushButton("Load");
    QPushButton* export_file = new QPushButton("Export");

    QVBoxLayout* vlayout  = new QVBoxLayout;
    vlayout->setSpacing(0);
    vlayout->setMargin(0);

    vlayout->addWidget(color_wheel);
    vlayout->addWidget(pen_width_slider);
    vlayout->addWidget(radio_pen);
    vlayout->addWidget(radio_lasso);
    vlayout->addWidget(radio_eraser);
    vlayout->addWidget(radio_knockback);
    vlayout->addWidget(radio_colorpicker);
    vlayout->addWidget(radio_move);
    vlayout->addWidget(radio_selection);
    vlayout->addWidget(button_preview_show);
    vlayout->addWidget(button_copy_previous);
    vlayout->addWidget(button_onion_show);
    vlayout->addWidget(button_onion_next);
    vlayout->addWidget(button_onion_prev);
    vlayout->addWidget(button_onion_loop);
    vlayout->addWidget(fps_slider);
    vlayout->addWidget(save_file);
    vlayout->addWidget(load_file);
    vlayout->addWidget(export_file);

    setLayout(vlayout);

    connect(radio_pen,         &QRadioButton::pressed, this, [] { Mw::editor->set_tool(PEN); });
    connect(radio_lasso,       &QRadioButton::pressed, this, [] { Mw::editor->set_tool(LASSOFILL); });
    connect(radio_eraser,      &QRadioButton::pressed, this, [] { Mw::editor->set_tool(ERASER); });
    connect(radio_knockback,   &QRadioButton::pressed, this, [] { Mw::editor->set_tool(KNOCKBACK); });
    connect(radio_colorpicker, &QRadioButton::pressed, this, [] { Mw::editor->set_tool(COLORPICKER); });
    connect(radio_move,        &QRadioButton::pressed, this, [] { Mw::editor->set_tool(MOVE); });
    connect(radio_selection,   &QRadioButton::pressed, this, [] { Mw::editor->set_tool(SELECTION); });

    connect(button_preview_show,  &QCheckBox::pressed, this, []() { Mw::preview->toggle_visibility(); });
    connect(button_onion_show,    &QCheckBox::pressed, this, []() { Mw::editor->toggle_onion_skin(); });
    connect(button_copy_previous, &QCheckBox::pressed, this, []() { Mw::editor->toggle_copy_prev_frame(); });
    connect(button_onion_next,    &QCheckBox::pressed, this, []() { Mw::editor->toggle_onion_skin_next(); });
    connect(button_onion_prev,    &QCheckBox::pressed, this, []() { Mw::editor->toggle_onion_skin_prev(); });
    connect(button_onion_loop,    &QCheckBox::pressed, this, []() { Mw::editor->toggle_onion_skin_loop(); });

    connect(pen_width_slider, &QAbstractSlider::valueChanged, this, [pen_width_slider] {
        Mw::editor->set_pen_size(pen_width_slider->value());
        Mw::editor->set_eraser_size(pen_width_slider->value());
    });

    connect(fps_slider, &QAbstractSlider::valueChanged, this, [fps_slider] {
        Mw::preview->toggle_play();
        Mw::preview->toggle_play();
        Mw::animation->FPS = fps_slider->value();
    });

    connect(color_wheel, &ColorWheel::select_color, this, [this]() {
        color_wheel->set_color(color_wheel->color());
        Mw::editor->set_pen_color(color_wheel->color());
        Mw::editor->set_brush_color(color_wheel->color());
    });

    connect(save_file, &QPushButton::pressed, this, [] {
       auto saved_file = QFileDialog::getSaveFileName(0, ("Save animation"), QDir::currentPath());
       auto fileinfo = QFileInfo(saved_file);

       // TODO:
       qDebug() << fileinfo.baseName();
       qDebug() << fileinfo.suffix();
    });

    connect(load_file, &QPushButton::pressed, this, [] {
        auto loaded_file = QFileDialog::getOpenFileName(0, ("Load animation"), QDir::currentPath(), "*.4n1m");
        auto fileinfo = QFileInfo(loaded_file);
        // TODO: Mw::animation->load_animation(loaded_file);
    });

    connect(export_file, &QPushButton::pressed, this, [] {
        auto exported_file = QFileDialog::getSaveFileName(0, ("Export animation"), QDir::currentPath());
        auto fileinfo = QFileInfo(exported_file);

        Mw::animation->export_animation(exported_file);
    });
}

void Toolbar::paintEvent(QPaintEvent*) {
    widget_painter.begin(this);

    widget_painter.setBrush(Qt::gray);
    widget_painter.drawRect(rect());

    widget_painter.end();
}
