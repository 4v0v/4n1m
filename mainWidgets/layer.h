#ifndef LAYER_H
#define LAYER_H

#include "mainwindow.h"
#include "mainWidgets/frame.h"

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

protected:
    void paintEvent(QPaintEvent*) override;

private:
    MainWindow* mainwindow;
    int layerPos;

    QList<Frame*> frames;

};

#endif
