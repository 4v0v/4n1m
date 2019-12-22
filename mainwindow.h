#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QUndoView>

class Editor;
class Timeline;
class Object;
class Preview;
class QUndoStack;
class Titlebar;
class Menubar;

enum Tool {
    PEN,
    LASSOFILL,
    ERASER,
    LINE
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    Editor* getEditor() { return editor; }
    Timeline* getTimeline() { return timeline; }
    Object* getObject() { return object; }
    Preview* getPreview() { return preview; }
    Titlebar* getTitlebar() { return titlebar; }
    Menubar* getMenubar() { return menubar; }
    QUndoStack* getUndoStack() { return undostack; }

public slots:
    void openBackgroundColorWindow();
    void openPenColorWindow();
    void openPenWidthWindow();
    void openFillStyleWindow();
    void openPreviewWindow();
    void openUndoStackWindow();
    void toggleStayOnTop();

private:
    Editor* editor;
    Timeline* timeline;
    Object* object;
    Preview* preview;
    Titlebar* titlebar;
    Menubar* menubar;
    QUndoStack* undostack;
    QUndoView* undoView;
    QAction* toggleStayOnTopAct;
    QAction* toggleOnionskinAct;
    QAction* toggleLayerTransparencyAct;
};

#endif
