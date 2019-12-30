#ifndef LAYER_H
#define LAYER_H

#include "mainwindow.h"
#include "mainWidgets/frame.h"

class LayerTitle : public QLabel
{
     Q_OBJECT
public:
    LayerTitle(MainWindow*);
    double getOpacity() { return opacity; }

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

private:
    MainWindow* mainwindow;

    bool isDown = false;
    double opacity = 1.0;
};

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

class Layer : public QWidget
{
    Q_OBJECT

public:
    Layer(MainWindow*, int);
    Timeline* timeline() { return mainwindow->getTimeline(); }
    Editor* editor() { return mainwindow->getEditor(); }
    Animation* animation() { return mainwindow->getAnimation(); }
    QUndoStack* undostack() { return mainwindow->getUndoStack(); }
    int getPos() { return layerPos; }

    Frame* getFrameWidgetAt(int p) { return frames[p]; }
    LayerTitle* getLayerTitle() { return layerTitle; }

protected:
    void paintEvent(QPaintEvent*) override;

private:
    MainWindow* mainwindow;
    int layerPos;
    double opacity = 1.0;
    QList<Frame*> frames;
    LayerTitle* layerTitle;
};

#endif
