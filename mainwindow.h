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
#include <QList>
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
class Animation;
class Preview;
class Titlebar;
class Menubar;
class Layer;
class Frame;

enum Tool {
    EMPTY,
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
    Animation* getAnimation() { return animation; }
    Preview* getPreview() { return preview; }
    Titlebar* getTitlebar() { return titlebar; }
    Menubar* getMenubar() { return menubar; }
    QUndoStack* getUndoStack() { return undostack; }
    int getFPS() { return FPS; }
    QRect getWindowDimensions() { return windowDimensions; }

public slots:
    void openBackgroundColorWindow();
    void openPenColorWindow();
    void openPenWidthWindow();
    void openEraserWidthWindow();
    void openFillStyleWindow();
    void openUndoAmountWindow();
    void openKnockbackAmountWindow();
    void openChangeFPSWindow();
    void openPreviewWindow();
    void openUndoStackWindow();
    void toggleStayOnTop();
    void checkTool(Tool);

private:
    Editor* editor;
    Timeline* timeline;
    Animation* animation;
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
    QAction* toggleLayerOpacityAct;

    int FPS = 24;
    int undostackAmount = 30;
    QRect windowDimensions = QRect(100, 100, 850, 650);
};

#endif
