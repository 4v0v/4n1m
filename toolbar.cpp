#include "toolbar.h"
#include "colorwheel.h"
#include "editor.h"
#include "preview.h"

Toolbar::Toolbar(): QWidget(nullptr)
{
    setGeometry(0, 0, 150, 350);

    ColorWheel* color_wheel = new ColorWheel(this);

    QCheckBox* preview_button = new QCheckBox("Preview", this);
    preview_button->setChecked(Mw::preview->is_visible);

    QCheckBox* copy_previous_button = new QCheckBox("Copy previous frame", this);
    copy_previous_button->setChecked(Mw::editor->is_copy_prev_frame);

    QCheckBox* onion_button = new QCheckBox("Onion skin", this);
    onion_button->setChecked(Mw::editor->is_os_enabled);

    QCheckBox* onion_next   = new QCheckBox("Onion next", this);
    onion_next->setChecked(Mw::editor->is_os_next_enabled);

    QCheckBox* onion_prev   = new QCheckBox("Onion prev", this);
    onion_prev->setChecked(Mw::editor->is_os_prev_enabled);

    QCheckBox* onion_loop   = new QCheckBox("Onion loop", this);
    onion_loop->setChecked(Mw::editor->is_os_loop_enabled);

    QRadioButton *pen_radiobutton = new QRadioButton("PEN", this);
    pen_radiobutton->toggle();

    QRadioButton *lasso_radiobutton = new QRadioButton("LASSO FILL", this);

    QSlider* pen_width_slider = new QSlider();
    pen_width_slider->setRange(1, 10);
    pen_width_slider->setValue(3);
    pen_width_slider->setOrientation(Qt::Horizontal);

    QPushButton* save_file = new QPushButton("Save");
    QPushButton* load_file = new QPushButton("Load");
    QPushButton* export_file = new QPushButton("Export");

    QVBoxLayout* vlayout  = new QVBoxLayout;
    vlayout->setSpacing(0);
    vlayout->setMargin(0);
    vlayout->addWidget(color_wheel);
    vlayout->addWidget(pen_width_slider);
    vlayout->addWidget(pen_radiobutton);
    vlayout->addWidget(lasso_radiobutton);
    vlayout->addWidget(preview_button);
    vlayout->addWidget(copy_previous_button);
    vlayout->addWidget(onion_button);
    vlayout->addWidget(onion_next);
    vlayout->addWidget(onion_prev);
    vlayout->addWidget(onion_loop);
    vlayout->addWidget(save_file);
    vlayout->addWidget(load_file);
    vlayout->addWidget(export_file);
    setLayout(vlayout);

    connect(pen_radiobutton, &QRadioButton::pressed, this, [] { Mw::editor->set_tool(PEN); });
    connect(lasso_radiobutton, &QRadioButton::pressed, this, [] { Mw::editor->set_tool(LASSOFILL); });
    connect(preview_button, &QCheckBox::pressed, this, []() { Mw::preview->toggle_visibility(); });
    connect(onion_button, &QCheckBox::pressed, this, []() { Mw::editor->toggle_onion_skin(); });
    connect(copy_previous_button, &QCheckBox::pressed, this, []() { Mw::editor->toggle_copy_prev_frame(); });
    connect(onion_next, &QCheckBox::pressed, this, []() { Mw::editor->toggle_onion_skin_next(); });
    connect(onion_prev, &QCheckBox::pressed, this, []() { Mw::editor->toggle_onion_skin_prev(); });
    connect(onion_loop, &QCheckBox::pressed, this, []() { Mw::editor->toggle_onion_skin_loop(); });
    connect(pen_width_slider, &QAbstractSlider::valueChanged, this, [pen_width_slider] { Mw::editor->set_pen_size(pen_width_slider->value()); });
    connect(color_wheel, &ColorWheel::select_color, this, [color_wheel]() {
        Mw::editor->set_pen_color(color_wheel->color());
        Mw::editor->set_brush_color(color_wheel->color());
    });

    connect(save_file, &QPushButton::pressed, this, [] {
       auto saved_file = QFileDialog::getSaveFileName(0, ("Save animation"), QDir::currentPath());
       auto fileinfo = QFileInfo(saved_file);

       qDebug() << fileinfo.baseName();
       qDebug() << fileinfo.suffix();
    });

    connect(load_file, &QPushButton::pressed, this, [] {
        auto loaded_file = QFileDialog::getOpenFileName(0, ("Load animation"), QDir::currentPath(), "*.4n1m");
        auto fileinfo = QFileInfo(loaded_file);


//        Mw::animation->load_animation(loaded_file);
    });

    connect(export_file, &QPushButton::pressed, this, [] {
        auto exported_folder = QFileDialog::getExistingDirectory(0, ("Select Output Folder"), QDir::currentPath());

        Mw::animation->export_animation(exported_folder);
    });
}

void Toolbar::paintEvent(QPaintEvent*) {
    widget_painter.begin(this);

    widget_painter.setBrush(Qt::gray);
    widget_painter.drawRect(rect());

    widget_painter.end();
}
