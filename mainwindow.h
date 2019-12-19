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

public slots:
    void openBackgroundColorWindow();
    void openPenColorWindow();
    void openPenWidthWindow();
    void openFillStyleWindow();
    void openPreviewWindow();
    void openUndoStackWindow();
    void toggleStayOnTop();

private:
    Editor *editor;
    Timeline *timeline;
    Object *object;
    Preview *preview;
    QUndoStack *undoStack;
    QUndoView *undoView;
    QAction *toggleStayOnTopAct;
    QAction *toggleOnionskinAct;
    QAction *toggleLayerTransparencyAct;
};

#endif
