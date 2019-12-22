#include <QtWidgets>
#include "preview.h"
#include "object.h"
#include "mainwindow.h"

Preview::Preview(MainWindow* mainwindow) : QWidget(mainwindow)
{
    parent = mainwindow;

    QTimer *timer = new QTimer();
    timer->connect(timer, SIGNAL(timeout()), this, SLOT(play()));
    timer->start(48);

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
}

void Preview::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    object()->foreachLayerRevert([&painter, &event, this](int i){
        QImage img = object()->isKeyframe(i, currentPosition) ?
                    *object()->getKeyframeImageAt(i, currentPosition) :
                    *object()->getKeyframeImageAt(i, object()->getPrevKeyframePos(i, currentPosition));
        painter.drawImage(event->rect(), img, event->rect());
    });
}

void Preview::play()
{
    currentPosition += 1;

    int maxFrame = object()->getLastKeyframePos(0) > object()->getLastKeyframePos(1) ? object()->getLastKeyframePos(0) : object()->getLastKeyframePos(1);
    if (currentPosition > maxFrame) currentPosition = 0;
    update();
}

void Preview::pause(){}

void Preview::mousePressEvent(QMouseEvent *event)
{
    p = event->pos();
    isDown = true;
}

void Preview::mouseReleaseEvent(QMouseEvent*)
{
    isDown = false;
}

void Preview::mouseMoveEvent(QMouseEvent *event)
{
    if(isDown)
    {
        QPoint diff= event->pos() - p;
        window()->move(window()->pos()+diff);
    }
}
