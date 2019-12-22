#include <QtWidgets>
#include "titlebar.h"

Titlebar::Titlebar(QWidget* parent): QWidget(parent)
{
    update();
}


void Titlebar::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QPainterPath path;

    path.addRect(0, 0, width(), height());
    painter.fillPath(path, QColor(50, 50, 51));
    painter.setPen(QPen(QColor(50, 50, 51), 1));
    painter.drawPath(path);
}

void Titlebar::mousePressEvent(QMouseEvent *event)
{
    p = event->pos();
    isDown = true;
}

void Titlebar::mouseReleaseEvent(QMouseEvent*)
{
    isDown = false;
}

void Titlebar::mouseMoveEvent(QMouseEvent *event)
{
    if(!isDown) return;
    QPoint diff= event->pos() - p;
    window()->move(window()->pos()+diff);
}
