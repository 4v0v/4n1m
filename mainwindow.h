#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QUndoView>

class Editor;
class Timeline;
class Object;
class Preview;
class QUndoStack;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void openPenColorWindow();
    void openPenWidthWindow();
    void openFillStyleWindow();
    void openPreviewWindow();
    void openUndoStackWindow();
    void changeNbrWindow();

private:
    Editor *editor;
    Timeline *timeline;
    Object *object;
    Preview *preview;
    QUndoStack *undoStack;
    QUndoView *undoView;

    int nbr = 0;
};

#endif
