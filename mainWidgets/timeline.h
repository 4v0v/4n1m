#ifndef TIMELINE_H
#define TIMELINE_H

#include "mainwindow.h"
#include "mainWidgets/layer.h"

class TimelineScrollArea : public QScrollArea
{
     Q_OBJECT
public:
    TimelineScrollArea(MainWindow*, Timeline*);
    QList<Layer*>* getLayerWidgets() { return &layers; }

protected:
    void wheelEvent(QWheelEvent*) override{};
    void keyPressEvent(QKeyEvent*) override{};
    void resizeEvent(QResizeEvent*) override;
    bool eventFilter(QObject*, QEvent*) override;

private:
    MainWindow* mainwindow;
    QList<Layer*> layers;
};

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

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
    Frame* getFrameWidgetAt(int l, int f) { return timelineScroll->getLayerWidgets()->at(l)->getFrameWidgetAt(f); }
    Layer* getLayerWidgetAt(int l) { return timelineScroll->getLayerWidgets()->at(l); }

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

private:
    MainWindow* mainwindow;
    TimelineScrollArea* timelineScroll;

    QImage clipboard = QImage(1, 1, QImage::Format_ARGB32);
    int timelinePos = 0;
    int timelineLayer = 0;
};

#endif
