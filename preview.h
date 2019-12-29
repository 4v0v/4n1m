#ifndef PREVIEW_H
#define PREVIEW_H

#include "mainwindow.h"

class Preview : public QWidget
{
    Q_OBJECT

public:
    Preview(MainWindow*);
    Timeline* timeline() { return mainwindow->getTimeline(); }
    Editor* editor() { return mainwindow->getEditor(); }
    Animation* animation() { return mainwindow->getAnimation(); }
    QUndoStack* undostack() { return mainwindow->getUndoStack(); }

public slots:
    void play();
    void pause();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void wheelEvent(QWheelEvent*) override;

private:
    MainWindow* mainwindow;

    QTimer* timer;
    int currentPosition = 0;
    int waitBeforeLoop = 0;
    int zoom = 100;
    QPoint p;
    int isDown = false;
};


#endif
