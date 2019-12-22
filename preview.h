#ifndef PREVIEW_H
#define PREVIEW_H

#include "mainwindow.h"

class Preview : public QWidget
{
    Q_OBJECT

public:
    Preview(MainWindow*);
    Timeline* timeline() { return parent->getTimeline(); }
    Editor* editor() { return parent->getEditor(); }
    Object* object() { return parent->getObject(); }
    QUndoStack* undostack() { return parent->getUndoStack(); }

public slots:
    void play();
    void pause();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

private:
    MainWindow* parent;

    QTimer* timer;
    int currentPosition = 0;
    QPoint p;
    int isDown = false;
};


#endif
