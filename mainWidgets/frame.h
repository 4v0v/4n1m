#ifndef FRAME_H
#define FRAME_H

#include "mainwindow.h"

class Frame : public QWidget
{
    Q_OBJECT

public:
    Frame(MainWindow*, Layer*, int);
    Timeline* timeline() { return mainwindow->getTimeline(); }
    Editor* editor() { return mainwindow->getEditor(); }
    Animation* animation() { return mainwindow->getAnimation(); }
    QUndoStack* undostack() { return mainwindow->getUndoStack(); }
    Layer* getLayer() { return layer;}
    int getPos() { return framePos; }

    void toggleIsKey() { isKey = !isKey; update(); }
    void toggleIsCurrent() { isCurrent = !isCurrent; update(); }

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;

private:
    MainWindow* mainwindow;
    Layer* layer;
    int framePos;

    bool isKey = false;
    bool isCurrent = false;
};

#endif
