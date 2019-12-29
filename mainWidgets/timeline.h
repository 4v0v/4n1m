#ifndef TIMELINE_H
#define TIMELINE_H

#include "mainwindow.h"
#include "mainWidgets/layer.h"

class Timeline : public QWidget
{
    Q_OBJECT

public:
    Timeline(MainWindow*);
    Editor* editor() { return mainwindow->getEditor(); }
    Animation* animation() { return mainwindow->getAnimation(); }
    QUndoStack* undostack() { return mainwindow->getUndoStack(); }

    int getPos() { return timelinePos; }
    void setPos(int i) { timelinePos = i; }
    int getLayer() { return timelineLayer; }
    void setLayer(int i) { timelineLayer = i; }
    Frame* getFrameWidgetAt(int l, int f) { return layers[l]->getFrameWidgetAt(f); }

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
    void wheelEvent(QWheelEvent*) override;

private:
    MainWindow* mainwindow;
    QScrollArea* timelineScroll;

    QImage clipboard = QImage(1, 1, QImage::Format_ARGB32);
    int timelinePos = 0;
    int timelineLayer = 0;

    QList<Layer*> layers;
};

#endif
