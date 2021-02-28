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
#include <QString>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QDirIterator>

class Editor;
class Animation;
class Timeline;
class Toolbar;
class TimelineFrame;
class TimelineLayer;
class Preview;

enum State {
    IDLE,
    SCRIBBLING,
    MOVING,
    PLAYING,
};

enum Mode {
    EMPTY,
    PREVIOUS,
};

enum Direction {
    LEFT,
    RIGHT,
    TOP,
    BOTTOM,
};

enum Tool {
    PEN,
    LASSOFILL,
    ERASER,
    COLORPICKER,
};

class Mw : public QMainWindow
{
    Q_OBJECT

public:
    Mw();
    virtual void dragEnterEvent(QDragEnterEvent*);
    virtual void dropEvent(QDropEvent*);
    virtual void closeEvent(QCloseEvent*);
    virtual void resizeEvent(QResizeEvent*);

    static void update_all();
    static void set_painter_colors(QPainter* painter, QColor pen , QColor brush = nullptr);

    void init_shortcuts();
    void create_shortcut(QKeySequence ks, std::function<void()> action);
    void undo();
    void redo();

    static Mw* mainwindow;
    static Animation* animation;
    static Timeline* timeline;
    static Editor* editor;
    static Toolbar* toolbar;
    static Preview* preview;
    static QUndoStack* undostack;
};
