#pragma once

#include <QMainWindow>
#include <QtWidgets>
#include <QAbstractButton>
#include <QColorDialog>
#include <QFileDialog>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QVBoxLayout>
#include <QUndoCommand>

class Editor;
class Animation;
class Timeline;
class Toolbar;
class TimelineFrame;
class TimelineLayer;

enum State {
    IDLE,
    SCRIBBLING,
    MOVING,
    PLAYING
};

enum Direction {
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
};

enum Tool {
    PEN,
    LASSOFILL
};

class Mw : public QMainWindow
{
    Q_OBJECT

public:
    Mw();
    virtual void dragEnterEvent(QDragEnterEvent*);
    virtual void dropEvent(QDropEvent*);
    virtual void closeEvent(QCloseEvent*);

    static void update_editor_and_timeline();
    static void set_painter_colors(QPainter* painter, QColor pen , QColor brush = nullptr);

    void create_shortcut(QKeySequence ks, std::function<void()> action);
    void undo();
    void redo();
    void play(bool loop);

    static Animation* animation;
    static Timeline* timeline;
    static Editor* editor;
    static Toolbar* toolbar;
    static QUndoStack* undostack;
};
