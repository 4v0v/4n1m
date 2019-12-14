#include <QtWidgets>
#include "preview.h"
#include "object.h"

Preview::Preview(Object *o, QWidget *parent) : QWidget(parent)
{
    setObject(o);
    QTimer *timer = new QTimer();
    timer->connect(timer, SIGNAL(timeout()), this, SLOT(play()));
    timer->start(48);
}

void Preview::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    object->foreachLayerRevert([&painter, &event, this](int i){
        QImage img = object->isKeyframe(i, currentPosition) ?
                    *object->getKeyframeImageAt(i, currentPosition) :
                    *object->getKeyframeImageAt(i, object->getPrevKeyframePos(i, currentPosition));
        painter.drawImage(event->rect(), img, event->rect());
    });
}

void Preview::focusOutEvent(QFocusEvent*){}

void Preview::play()
{
    currentPosition += 1;

    int maxFrame = object->getLastKeyframePos(0) > object->getLastKeyframePos(1) ? object->getLastKeyframePos(0) : object->getLastKeyframePos(1);
    if (currentPosition > maxFrame) currentPosition = 0;
    update();
}

void Preview::pause(){}
