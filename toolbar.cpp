#include "toolbar.h"
#include "editor.h"
#include "preview.h"

Toolbar::Toolbar(): QWidget(nullptr)
{
    setGeometry(0, 0, 150, 480);

    color_wheel = new ColorWheel(this);

    QCheckBox* btn_show_preview = new QCheckBox("Preview"            , this);
    QCheckBox* btn_copy_prev    = new QCheckBox("Copy previous frame", this);
    QCheckBox* btn_onion_show   = new QCheckBox("Onion skin"         , this);
    QCheckBox* btn_onion_next   = new QCheckBox("Onion next"         , this);
    QCheckBox* btn_onion_prev   = new QCheckBox("Onion prev"         , this);
    QCheckBox* btn_onion_loop   = new QCheckBox("Onion loop"         , this);
    btn_show_preview->setChecked(Mw::preview->is_visible);
    btn_copy_prev   ->setChecked(Mw::editor->is_copy_prev_enabled);
    btn_onion_show  ->setChecked(Mw::editor->is_os_enabled);
    btn_onion_next  ->setChecked(Mw::editor->is_os_next_enabled);
    btn_onion_prev  ->setChecked(Mw::editor->is_os_prev_enabled);
    btn_onion_loop  ->setChecked(Mw::editor->is_os_loop_enabled);

    QRadioButton* rad_pen         = new QRadioButton("PEN"         , this);
    QRadioButton* rad_lasso       = new QRadioButton("LASSO FILL"  , this);
    QRadioButton* rad_eraser      = new QRadioButton("ERASER"      , this);
    QRadioButton* rad_knockback   = new QRadioButton("KNOCKBACK"   , this);
    QRadioButton* rad_colorpicker = new QRadioButton("COLOR PICKER", this);
    QRadioButton* rad_move        = new QRadioButton("MOVE"        , this);
    QRadioButton* rad_selection   = new QRadioButton("SELECTION"   , this);
    rad_pen->toggle();

    QSlider* pen_width_slider = new QSlider();
    pen_width_slider->setRange(1, 10);
    pen_width_slider->setValue(Mw::editor->tool_pen->pen_tool.width());
    pen_width_slider->setOrientation(Qt::Horizontal);

    QSlider* fps_slider = new QSlider();
    fps_slider->setRange(1, 60);
    fps_slider->setValue(Mw::animation->FPS);
    fps_slider->setOrientation(Qt::Horizontal);

    QPushButton* file_save   = new QPushButton("Save"  );
    QPushButton* file_load   = new QPushButton("Load"  );
    QPushButton* file_export = new QPushButton("Export");

    QVBoxLayout* vlayout = new QVBoxLayout;
    vlayout->setSpacing(0);
    vlayout->setMargin(0);

    vlayout->addWidget(color_wheel);
    vlayout->addWidget(pen_width_slider);
    vlayout->addWidget(rad_pen);
    vlayout->addWidget(rad_lasso);
    vlayout->addWidget(rad_eraser);
    vlayout->addWidget(rad_knockback);
    vlayout->addWidget(rad_colorpicker);
    vlayout->addWidget(rad_move);
    vlayout->addWidget(rad_selection);
    vlayout->addWidget(btn_show_preview);
    vlayout->addWidget(btn_copy_prev);
    vlayout->addWidget(btn_onion_show);
    vlayout->addWidget(btn_onion_next);
    vlayout->addWidget(btn_onion_prev);
    vlayout->addWidget(btn_onion_loop);
    vlayout->addWidget(fps_slider);
    vlayout->addWidget(file_save);
    vlayout->addWidget(file_load);
    vlayout->addWidget(file_export);

    setLayout(vlayout);

    connect(btn_show_preview , &QCheckBox   ::pressed, this, [] { Mw::preview->toggle_visibility();     });
    connect(btn_onion_show   , &QCheckBox   ::pressed, this, [] { Mw::editor->toggle_onion_skin();      });
    connect(btn_copy_prev    , &QCheckBox   ::pressed, this, [] { Mw::editor->toggle_copy_prev_frame(); });
    connect(btn_onion_next   , &QCheckBox   ::pressed, this, [] { Mw::editor->toggle_onion_skin_next(); });
    connect(btn_onion_prev   , &QCheckBox   ::pressed, this, [] { Mw::editor->toggle_onion_skin_prev(); });
    connect(btn_onion_loop   , &QCheckBox   ::pressed, this, [] { Mw::editor->toggle_onion_skin_loop(); });
    connect(rad_pen          , &QRadioButton::pressed, this, [] { Mw::editor->set_tool(PEN);            });
    connect(rad_lasso        , &QRadioButton::pressed, this, [] { Mw::editor->set_tool(LASSOFILL);      });
    connect(rad_eraser       , &QRadioButton::pressed, this, [] { Mw::editor->set_tool(ERASER);         });
    connect(rad_knockback    , &QRadioButton::pressed, this, [] { Mw::editor->set_tool(KNOCKBACK);      });
    connect(rad_colorpicker  , &QRadioButton::pressed, this, [] { Mw::editor->set_tool(COLORPICKER);    });
    connect(rad_move         , &QRadioButton::pressed, this, [] { Mw::editor->set_tool(MOVE);           });
    connect(rad_selection    , &QRadioButton::pressed, this, [] { Mw::editor->set_tool(SELECTION);      });

    connect(pen_width_slider, &QAbstractSlider::valueChanged, this, [pen_width_slider] {
        Mw::editor->set_pen_size   (pen_width_slider->value());
        Mw::editor->set_eraser_size(pen_width_slider->value());
    });

    connect(fps_slider, &QAbstractSlider::valueChanged, this, [fps_slider] {
        //TODO: refactor ugly double toggle to reset preview
        Mw::preview->toggle_play();
        Mw::preview->toggle_play();
        Mw::animation->FPS = fps_slider->value();
    });

    connect(file_save, &QPushButton::pressed, this, [] {
       auto saved_file = QFileDialog::getSaveFileName(0, ("Save animation"), QDir::currentPath());
       auto file_info  = QFileInfo(saved_file);

       // TODO:
       qDebug() << file_info.baseName();
       qDebug() << file_info.suffix();
    });

    connect(file_load, &QPushButton::pressed, this, [] {
        auto loaded_file = QFileDialog::getOpenFileName(0, ("Load animation"), QDir::currentPath(), "*.4n1m");
        auto file_info   = QFileInfo(loaded_file);
        // TODO: Mw::animation->load_animation(loaded_file);
    });

    connect(file_export, &QPushButton::pressed, this, [] {
        auto exported_file = QFileDialog::getSaveFileName(0, ("Export animation"), QDir::currentPath());
        auto file_info     = QFileInfo(exported_file);

        Mw::animation->export_animation(exported_file);
    });
}

void Toolbar::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setBrush(Qt::gray);
    painter.drawRect(rect());
}
