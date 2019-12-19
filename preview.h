#ifndef PREVIEW_H
#define PREVIEW_H

#include <QWidget>

class Object;
class Preview : public QWidget
{
    Q_OBJECT

public:
    Preview(Object *o = nullptr, QWidget *parent = nullptr);
    void setObject(Object* o) { object = o; }

public slots:
    void play();
    void pause();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

private:
    Object* object;
    int currentPosition = 0;
    int currentLayer = 0;

    QPoint p;
    int isDown = false;
};


#endif
