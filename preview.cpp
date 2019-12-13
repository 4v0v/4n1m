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
    object->isKeyframe(currentLayer, currentPosition) ?
    painter.drawImage(event->rect(), *object->getKeyframeImageAt(currentLayer, currentPosition), event->rect()) :
    painter.drawImage(event->rect(), *object->getKeyframeImageAt(currentLayer, object->getPrevKeyframePos(currentLayer, currentPosition)), event->rect());
}

void Preview::focusOutEvent(QFocusEvent*){}

void Preview::play()
{
    currentPosition += 1;
    if (currentPosition > object->getLastKeyframePos(currentLayer)) currentPosition = 0;
    update();
}

void Preview::pause(){}
