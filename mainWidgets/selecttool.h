#ifndef SELECTTOOL_H
#define SELECTTOOL_H

#include "mainwindow.h"
#include "mainWidgets/toolbar.h"

class SelectTool : public QWidget
{
    Q_OBJECT

public:
    SelectTool(MainWindow*);
    Timeline* timeline() { return mainwindow->timeline; }
    Animation* animation() { return mainwindow->animation; }
    Toolbar* toolbar() { return mainwindow->toolbar; }
    Editor* editor() { return mainwindow->editor; }
    QUndoStack* undostack() { return mainwindow->undostack; }

    void mousePress(QMouseEvent*);
    void mouseMove(QMouseEvent*);
    void mouseRelease(QMouseEvent*);
    void paint(QPaintEvent*, QPainter*);
    void draw();
    void reset();
    void knockback();
    void clear();

    MainWindow* mainwindow;
    Tool subtool = RECTANGLE;
    State state = STATE_EMPTY;
    QPolygon initialPolyZone;
    QRect initialRectZone = QRect(0,0,1,1);
    QImage initialImage = QImage(1, 1, QImage::Format_ARGB32);
    QRect deltaRectZone = QRect(0,0,1,1);
    QImage deltaImage = QImage(1, 1, QImage::Format_ARGB32);
    QPoint deltaPosition = QPoint(0, 0);
};

#endif
