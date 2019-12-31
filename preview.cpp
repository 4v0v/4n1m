#include "animation.h"
#include "preview.h"
#include "commands.h"
#include "mainWidgets/editor.h"
#include "mainWidgets/timeline.h"
#include "mainWidgets/titlebar.h"
#include "mainWidgets/toolbar.h"

Preview::Preview(MainWindow* mw)
{
    mainwindow = mw;

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    timer = new QTimer();
    timer->connect(timer, SIGNAL(timeout()), this, SLOT(play()));
    timer->start(1000/mainwindow->getFPS());
}

void Preview::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QPainterPath path;
    animation()->foreachLayerRevert([&painter, &event, this](int i){
        if (animation()->getKeyCount(i) == 0 ) return;
        if (animation()->isKey(i, currentPosition))
        {
            QImage img = animation()->copyImageAt(i, currentPosition);
            painter.drawImage( event->rect(), img.scaled(img.width()*zoom/100, img.height()*zoom/100) , event->rect() );
        }
        else
        {
            if (animation()->getPrevKey(i, currentPosition) == -1) return;
            QImage img = animation()->copyImageAt(i, animation()->getPrevKey(i, currentPosition));
            painter.drawImage(event->rect(), img.scaled(img.width()*zoom/100, img.height()*zoom/100), event->rect());
        }
    });
    path.addRect(0, 0, (editor()->width()-1)*zoom/100, (editor()->height()-1)*zoom/100);
    painter.fillPath(path,QColor(0, 0, 0, 1));
    painter.setPen(QPen(Qt::white));
    painter.drawPath(path);
}

void Preview::play()
{
    currentPosition += 1;
    int maxFrame = -1;
    animation()->foreachLayerRevert([&maxFrame, this](int i){
        if (animation()->getKeyCount(i) == 0 ) return;
        maxFrame = animation()->getLastKey(i) > maxFrame ? animation()->getLastKey(i) : maxFrame;
    });
    if (currentPosition > maxFrame + waitBeforeLoop) currentPosition = 0;
    update();
}

void Preview::pause(){}

void Preview::mousePressEvent(QMouseEvent *event) { p = event->pos(); isDown = true; }
void Preview::mouseReleaseEvent(QMouseEvent*) { isDown = false; }
void Preview::mouseMoveEvent(QMouseEvent *event)
{
    if(!isDown) return;
    QPoint diff= event->pos() - p;
    window()->move(window()->pos()+diff);
}

void Preview::wheelEvent(QWheelEvent*)
{
//    zoom += event->delta()/10;
//    setMaximumSize(QSize(editor()->width()-1*zoom/100, editor()->height()-1*zoom/100));
//    setMinimumSize(QSize(editor()->width()-1*zoom/100, editor()->height()-1*zoom/100));
}
