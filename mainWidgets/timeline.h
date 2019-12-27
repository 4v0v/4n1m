#ifndef TIMELINE_H
#define TIMELINE_H

#include "mainwindow.h"

class Timeline : public QWidget
{
    Q_OBJECT

public:
    Timeline(MainWindow*);
    Editor* editor() { return parent->getEditor(); }
    Animation* animation() { return parent->getAnimation(); }
    QUndoStack* undostack() { return parent->getUndoStack(); }

    int getPos() { return timelinePos; }
    void setPos(int i) { timelinePos = i; }
    int getLayer() { return timelineLayer; }
    void setLayer(int i) { timelineLayer = i; }

public slots:
    void gotoNextFrame();
    void gotoPrevFrame();
    void gotoNextLayer();
    void gotoPrevLayer();
    void gotoFrame(int layer, int pos);
    void addKey();
    void removeKey();
    void insertFrame();
    void removeFrame();
    void copyFrame();
    void cutFrame();
    void pasteFrame();

protected:
    void paintEvent(QPaintEvent*) override;

private:
    MainWindow* parent;

    QImage clipboard = QImage(1, 1, QImage::Format_ARGB32);
    int timelinePos = 0;
    int timelineLayer = 0;
};

#endif
