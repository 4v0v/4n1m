#include "object.h"
#include "editor.h"
#include "timeline.h"
#include "preview.h"
#include "commands.h"
#include "titlebar.h"
#include "menubar.h"

Preview::Preview(MainWindow* mainwindow)
{
    parent = mainwindow;

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    timer = new QTimer();
    timer->connect(timer, SIGNAL(timeout()), this, SLOT(play()));
    timer->start(1000/parent->getFPS());
}

void Preview::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QPainterPath path;

    object()->foreachLayerRevert([&painter, &event, this](int i){
        if (object()->getKeyframesCount(i) > 0 )
        {
            QImage img = object()->isKeyframe(i, currentPosition) ?
                        *object()->getKeyframeImageAt(i, currentPosition) :
                        *object()->getKeyframeImageAt(i, object()->getPrevKeyframePos(i, currentPosition));
            painter.drawImage(event->rect(), img, event->rect());
        }
    });

    path.addRect(0, 0, editor()->width()-1, editor()->height()-1);

    painter.fillPath(path,QColor(0, 0, 0, 1));
    painter.setPen(QPen(Qt::white));
    painter.drawPath(path);
}

void Preview::play()
{
    currentPosition += 1;
    int maxFrame = -1;

    object()->foreachLayerRevert([&maxFrame, this](int i){
        if (object()->getKeyframesCount(i) > 0 )
        {
            maxFrame = object()->getLastKeyframePos(i) > maxFrame ?
                        object()->getLastKeyframePos(i) :
                        maxFrame;
        }
    });


    if (currentPosition > maxFrame) currentPosition = 0;
    timer->setInterval(1000/parent->getFPS());
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
