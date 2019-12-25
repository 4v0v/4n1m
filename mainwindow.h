#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QApplication>
#include <QWindow>
#include <QDesktopWidget>
#include <QStyle>
#include <QMainWindow>
#include <QUndoView>
#include <QMap>
#include <QString>
#include <QImage>
#include <QPoint>
#include <QDebug>
#include <QUndoStack>
#include <QUndoCommand>
#include <QPainter>
#include <QObject>
#include <QWidget>
#include <QMenuBar>
#include <QColor>

class Editor;
class Timeline;
class Object;
class Preview;
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
    int getFPS() { return FPS; }

public slots:
    void openBackgroundColorWindow();
    void openPenColorWindow();
    void openPenWidthWindow();
    void openFillStyleWindow();
    void openKnockbackAmountWindow();
    void openChangeFPSWindow();
    void openPreviewWindow();
    void openUndoStackWindow();
    void toggleStayOnTop();
    void checkTool(Tool);

private:
    Editor* editor;
    Timeline* timeline;
    Object* object;
    Preview* preview;
    Titlebar* titlebar;
    Menubar* menubar;
    QUndoStack* undostack;
    QUndoView* undoView;
    
    QAction* setToolAsPenAct;
    QAction* setToolAsLineAct;
    QAction* setToolAsLassoFillAct;
    QAction* setToolAsEraserAct;
    QAction* toggleStayOnTopAct;
    QAction* toggleOnionskinAct;
    QAction* toggleOnionskinloopAct;
    QAction* toggleLayerTransparencyAct;

    int FPS = 24;
    int undostackAmount = 30;
};

#endif
