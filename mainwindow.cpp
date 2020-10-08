#include "mainwindow.h"
#include "editor.h"
#include "animation.h"
#include "timeline.h"
#include "colorwheel.h"
#include "toolbar.h"

Animation* Mw::animation;
Timeline* Mw::timeline;
Editor* Mw::editor;
Toolbar* Mw::toolbar;
QUndoStack* Mw::undostack;

Mw::Mw()
{
    //widgets & layout initialization
    animation = new Animation();
    editor    = new Editor(this);
    timeline  = new Timeline(this);
    toolbar   = new Toolbar(this);
    undostack = new QUndoStack(this);
    undostack->setUndoLimit(20);

    QHBoxLayout* toolbar_and_editor_layout = new QHBoxLayout();
    QWidget* toolbar_and_editor = new QWidget();
    toolbar_and_editor_layout->setSpacing(0);
    toolbar_and_editor_layout->setMargin(0);
    toolbar_and_editor_layout->addWidget(toolbar, 1);
    toolbar_and_editor_layout->addWidget(editor, 9);
    toolbar_and_editor->setLayout(toolbar_and_editor_layout);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(toolbar_and_editor, 8);
    layout->addWidget(timeline, 1);
    QWidget* main_widget = new QWidget();
    main_widget->setLayout(layout);

    setCentralWidget(main_widget);
    setWindowTitle(tr("4n1m"));
    setGeometry(QRect(0, 0, 1350, 850));
    setWindowState(Qt::WindowMaximized);
    setAcceptDrops(true);

    //shortcuts initialization
    create_shortcut(Qt::CTRL + Qt::Key_Z, [this](){ undo(); });
    create_shortcut(Qt::CTRL + Qt::Key_C,[]{ editor->copy(); });
    create_shortcut(Qt::CTRL + Qt::Key_X,[]{ editor->cut(); });
    create_shortcut(Qt::CTRL + Qt::Key_V,[]{ editor->paste(); });
    create_shortcut(Qt::CTRL + Qt::Key_T,[]{ editor->clear_current_layer(); });
    create_shortcut(Qt::CTRL + Qt::Key_Q,[]{ editor->clear_frame_at_current_pos(); });
    create_shortcut(Qt::CTRL + Qt::Key_Space,[]{ editor->play_from(editor->frame_pos > 5 ? editor->frame_pos - 5 : 0, false); });
    create_shortcut(Qt::CTRL + Qt::Key_S,[]{ animation->save_animation("", "temp"); });
    create_shortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_Z, [this]{ redo(); });
    create_shortcut(Qt::SHIFT + Qt::Key_Space, [this]{play(false); });
    create_shortcut(Qt::Key_Space, [this]{ play(true); });
    create_shortcut(Qt::Key_Right,[]{ editor->goto_next_pos(); });
    create_shortcut(Qt::Key_Left,[]{ editor->goto_prev_pos(); });
    create_shortcut(Qt::Key_Down,[]{ editor->goto_next_layer(); });
    create_shortcut(Qt::Key_Up,[]{ editor->goto_prev_layer(); });
    create_shortcut(Qt::Key_1,[]{ editor->set_tool(PEN); });
    create_shortcut(Qt::Key_2,[]{ editor->set_tool(LASSOFILL); });
    create_shortcut(Qt::Key_0,[]{ editor->toggle_onion_skin(); });
    create_shortcut(Qt::Key_P,[]{ editor->toggle_onion_skin_loop(); });
    create_shortcut(Qt::Key_I,[]{ editor->toggle_onion_skin_prev(); });
    create_shortcut(Qt::Key_O,[]{ editor->toggle_onion_skin_next(); });
    create_shortcut(Qt::Key_9,[]{ editor->insert_frame_at_current_pos(); });
    create_shortcut(Qt::Key_8,[]{ editor->uninsert_frame_at_current_pos(); });
    create_shortcut(Qt::Key_G,[]{ editor->set_add_frame_mode(EMPTY); });
    create_shortcut(Qt::Key_H,[]{ editor->set_add_frame_mode(PREVIOUS); });
    create_shortcut(Qt::Key_K,[]{ editor->knockback(); });
    create_shortcut(Qt::Key_M,[]{
        auto _OutputFolder = QFileDialog::getExistingDirectory(0, ("Select Output Folder"), QDir::currentPath());
        qDebug() << _OutputFolder;
        auto _OutputFile = QFileDialog::getOpenFileName(0, ("Select Output File"), QDir::currentPath(), "*.4n1m");
        qDebug() << _OutputFile;
    });
    create_shortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_S,[]{
        auto _OutputFolder = QFileDialog::getExistingDirectory(0, ("Select Output Folder"), QDir::currentPath());
        animation->export_animation(_OutputFolder);
    });

    // restore_previous_session
    animation->load_animation("temp.4n1m");
    update_all();
    timeline->update_all_frames();
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
    animation->save_animation( "", "temp");
    e->accept();
}

void Mw::dropEvent(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();
    animation->load_animation(mimeData->urls().at(0).toLocalFile());
    update_all();
}

void Mw::create_shortcut(QKeySequence ks, std::function<void()> action){
    connect(new QShortcut(ks, this), &QShortcut::activated, this, action);
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

void Mw::play(bool loop)
{
    if (editor->state == IDLE) editor->play_from(editor->frame_pos, loop);
    else if (editor->state == PLAYING) editor->stop();
}
