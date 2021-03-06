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
    rad_pen->toggle();

    QSlider* slider_width = new QSlider();
    slider_width->setRange(1, 10);
    slider_width->setValue(Mw::editor->tool_pen->pen_tool.width());
    slider_width->setOrientation(Qt::Horizontal);

    QSlider* slider_fps = new QSlider();
    slider_fps->setRange(1, 60);
    slider_fps->setValue(Mw::animation->FPS);
    slider_fps->setOrientation(Qt::Horizontal);

    QPushButton* file_save   = new QPushButton("Save"  );
    QPushButton* file_load   = new QPushButton("Load"  );
    QPushButton* file_export = new QPushButton("Export");

    QVBoxLayout* vlayout = new QVBoxLayout;
    vlayout->setSpacing(0);
    vlayout->setMargin (0);

    vlayout->addWidget(color_wheel     );
    vlayout->addWidget(slider_width    );
    vlayout->addWidget(rad_pen         );
    vlayout->addWidget(rad_lasso       );
    vlayout->addWidget(rad_eraser      );
    vlayout->addWidget(rad_knockback   );
    vlayout->addWidget(rad_colorpicker );
    vlayout->addWidget(rad_move        );
    vlayout->addWidget(btn_show_preview);
    vlayout->addWidget(btn_copy_prev   );
    vlayout->addWidget(btn_onion_show  );
    vlayout->addWidget(btn_onion_next  );
    vlayout->addWidget(btn_onion_prev  );
    vlayout->addWidget(btn_onion_loop  );
    vlayout->addWidget(slider_fps      );
    vlayout->addWidget(file_save       );
    vlayout->addWidget(file_load       );
    vlayout->addWidget(file_export     );

    setLayout(vlayout);

    connect(btn_show_preview, &QCheckBox   ::pressed, this, [] { Mw::preview->toggle_visibility();     });
    connect(btn_onion_show  , &QCheckBox   ::pressed, this, [] { Mw::editor->toggle_onion_skin();      });
    connect(btn_copy_prev   , &QCheckBox   ::pressed, this, [] { Mw::editor->toggle_copy_prev_frame(); });
    connect(btn_onion_next  , &QCheckBox   ::pressed, this, [] { Mw::editor->toggle_onion_skin_next(); });
    connect(btn_onion_prev  , &QCheckBox   ::pressed, this, [] { Mw::editor->toggle_onion_skin_prev(); });
    connect(btn_onion_loop  , &QCheckBox   ::pressed, this, [] { Mw::editor->toggle_onion_skin_loop(); });
    connect(rad_pen         , &QRadioButton::pressed, this, [] { Mw::editor->set_tool(PEN);            });
    connect(rad_lasso       , &QRadioButton::pressed, this, [] { Mw::editor->set_tool(LASSOFILL);      });
    connect(rad_eraser      , &QRadioButton::pressed, this, [] { Mw::editor->set_tool(ERASER);         });
    connect(rad_knockback   , &QRadioButton::pressed, this, [] { Mw::editor->set_tool(KNOCKBACK);      });
    connect(rad_colorpicker , &QRadioButton::pressed, this, [] { Mw::editor->set_tool(COLORPICKER);    });
    connect(rad_move        , &QRadioButton::pressed, this, [] { Mw::editor->set_tool(MOVE);           });

    connect(slider_width, &QAbstractSlider::valueChanged, this, [slider_width] {
        Mw::editor->set_pen_size   (slider_width->value());
        Mw::editor->set_eraser_size(slider_width->value());
    });

    connect(slider_fps, &QAbstractSlider::valueChanged, this, [slider_fps] {
        // TODO: refactor ugly double toggle to reset preview
        Mw::preview->toggle_play();
        Mw::preview->toggle_play();
        Mw::animation->FPS = slider_fps->value();
    });

    connect(file_save, &QPushButton::pressed, this, [] {
       auto saved_file = QFileDialog::getSaveFileName(0, ("Save animation"), QDir::currentPath());
       auto file_info  = QFileInfo(saved_file);

       // TODO:
       qDebug() << file_info.baseName();
       qDebug() << file_info.suffix();
    });

    connect(file_load, &QPushButton::pressed, this, [] {
        // auto loaded_file = QFileDialog::getOpenFileName(0, ("Load animation"), QDir::currentPath(), "*.4n1m");
        // auto file_info   = QFileInfo(loaded_file);
        // TODO: Mw::animation->load_animation(loaded_file);
    });

    connect(file_export, &QPushButton::pressed, this, [] {
        auto exported_file = QFileDialog::getSaveFileName(0, ("Export animation"), QDir::currentPath());
        // auto file_info     = QFileInfo(exported_file);

        Mw::animation->export_animation(exported_file);
    });

    connect(color_wheel, &ColorWheel::select_color, this, [this]() {
        color_wheel->set_color(color_wheel->color());
        Mw::editor->set_pen_color(color_wheel->color());
        Mw::editor->set_brush_color(color_wheel->color());
    });
}

void Toolbar::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setBrush(Qt::gray);
    painter.drawRect(rect());
}
