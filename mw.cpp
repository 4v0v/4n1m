#include "mw.h"
#include "editor.h"
#include "animation.h"
#include "timeline.h"
#include "colorwheel.h"
#include "toolbar.h"
#include "preview.h"

Animation* Mw::animation;
Timeline* Mw::timeline;
Editor* Mw::editor;
Toolbar* Mw::toolbar;
QUndoStack* Mw::undostack;
Preview* Mw::preview;

Mw::Mw()
{
    animation = new Animation();
    editor    = new Editor();
    timeline  = new Timeline();
    undostack = new QUndoStack();
    preview   = new Preview();
    toolbar   = new Toolbar();

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(editor, 8);
    layout->addWidget(timeline, 1);

    QWidget* main_widget = new QWidget();
    main_widget->setLayout(layout);
    preview->setParent(main_widget);
    toolbar->setParent(main_widget);

    undostack->setUndoLimit(20);

    setCentralWidget(main_widget);
    setWindowTitle(tr("4n1m"));
    setGeometry(QRect(0, 0, 1350, 850));
    setWindowState(Qt::WindowMaximized);
    setAcceptDrops(true);
    setMouseTracking(true);

    preview->move(editor->width() - preview->width(), editor->height() - preview->height());

    // restore previous session if possible
    animation->load_animation(".temp.4n1m");

    init_shortcuts();
}

void Mw::init_shortcuts() {
    create_shortcut(Qt::CTRL + Qt::Key_Z, [this](){ undo(); });
    create_shortcut(Qt::CTRL + Qt::Key_C,[]{ editor->copy(); });
    create_shortcut(Qt::CTRL + Qt::Key_X,[]{ editor->cut(); });
    create_shortcut(Qt::CTRL + Qt::Key_V,[]{ editor->paste(); });
    create_shortcut(Qt::CTRL + Qt::Key_T,[]{ editor->clear_current_layer(); });
    create_shortcut(Qt::CTRL + Qt::Key_Q,[]{ editor->clear_frame_at_current_pos(); });
    create_shortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_Z, [this]{ redo(); });
    create_shortcut(Qt::Key_Space,[]{ preview->toggle_play(); });
    create_shortcut(Qt::Key_Right,[]{ editor->goto_next_pos(); });
    create_shortcut(Qt::Key_Left,[]{ editor->goto_prev_pos(); });
    create_shortcut(Qt::Key_Down,[]{ editor->goto_next_layer(); });
    create_shortcut(Qt::Key_Up,[]{ editor->goto_prev_layer(); });
    create_shortcut(Qt::Key_1,[]{ editor->set_tool(PEN); });
    create_shortcut(Qt::Key_2,[]{ editor->set_tool(LASSOFILL); });
//    create_shortcut(Qt::Key_0,[]{ editor->toggle_onion_skin(); });
//    create_shortcut(Qt::Key_P,[]{ editor->toggle_onion_skin_loop(); });
//    create_shortcut(Qt::Key_I,[]{ editor->toggle_onion_skin_prev(); });
//    create_shortcut(Qt::Key_O,[]{ editor->toggle_onion_skin_next(); });
    create_shortcut(Qt::Key_9,[]{ editor->insert_frame_at_current_pos(); });
    create_shortcut(Qt::Key_8,[]{ editor->uninsert_frame_at_current_pos(); });
    create_shortcut(Qt::Key_G,[]{ editor->toggle_copy_prev_frame(); });
    create_shortcut(Qt::Key_K,[]{
        Tool t = editor->tool;
        editor->set_tool(KNOCKBACK);
        editor->tool_knockback->press(nullptr);
        editor->set_tool(t);
    });
    create_shortcut(Qt::Key_R,[]{ preview->toggle_visibility(); });
};

void Mw::create_shortcut(QKeySequence ks, std::function<void()> action)
{
    connect(new QShortcut(ks, this), &QShortcut::activated, this, action);
}

void Mw::dragEnterEvent(QDragEnterEvent* event)
{
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QFileInfo fileInfo = mimeData->urls().at(0).toLocalFile();
        QString filename = fileInfo.fileName();
        if (fileInfo.suffix() == "4n1m") {
            event->acceptProposedAction();
        }
    }
}

void Mw::closeEvent(QCloseEvent* e)
{
    animation->save_animation("temp.4n1m");
    e->accept();
}

void Mw::dropEvent(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();
    animation->load_animation(mimeData->urls().at(0).toLocalFile());
    update_all();
}

void Mw::resizeEvent(QResizeEvent *e)
{
    preview->move(editor->width() - preview->width(), editor->height() - preview->height());
    QWidget::resizeEvent(e);
}

void Mw::update_all()
{
    editor->create_onions_at_current_pos();
    editor->update();
    timeline->update_all_frames();
    timeline->update();
}

void Mw::set_painter_colors(QPainter* painter, QColor pen , QColor brush)
{
    painter->setPen(pen);
    painter->setBrush(brush == nullptr ? pen : brush);
}

void Mw::undo()
{
    if (Mw::editor->state != IDLE) return;
    Mw::undostack->undo();
}

void Mw::redo()
{
    if (Mw::editor->state != IDLE) return;
    Mw::undostack->redo();
}
