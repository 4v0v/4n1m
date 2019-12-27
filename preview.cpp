#include "animation.h"
#include "preview.h"
#include "commands.h"
#include "mainWidgets/editor.h"
#include "mainWidgets/timeline.h"
#include "mainWidgets/titlebar.h"
#include "mainWidgets/menubar.h"

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

    animation()->foreachLayerRevert([&painter, &event, this](int i){
        if (animation()->getKeyCount(i) > 0 )
        {
            if (animation()->isKey(i, currentPosition))
            {
                painter.drawImage(
                    event->rect(), 
                    animation()->copyImageAt(i, currentPosition),
                    event->rect()
                );
            } else {
                if (animation()->getPrevKey(i, currentPosition) != -1)
                {
                    painter.drawImage(
                        event->rect(), 
                        animation()->copyImageAt(i, animation()->getPrevKey(i, currentPosition)),
                        event->rect()
                    );
                } 
            } 
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

    animation()->foreachLayerRevert([&maxFrame, this](int i){
        if (animation()->getKeyCount(i) > 0 )
        {
            maxFrame = animation()->getLastKey(i) > maxFrame ?
                        animation()->getLastKey(i) :
                        maxFrame;
        }
    });


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
