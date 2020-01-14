#include "animation.h"
#include "preview.h"
#include "commands.h"
#include "mainWidgets/editor.h"
#include "mainWidgets/timeline.h"
#include "mainWidgets/titlebar.h"
#include "mainWidgets/toolbar.h"
#include "mainWidgets/selecttool.h"

SelectTool::SelectTool(MainWindow* mw): QWidget(mw)
{
    mainwindow = mw;
}

void SelectTool::mousePress(QMouseEvent* event)
{
    QRect topRight    = QRect(rectZone.topRight().x()    - 10, rectZone.topRight().y()    - 10, 20, 20);
    QRect topLeft     = QRect(rectZone.topLeft().x()     - 10, rectZone.topLeft().y()     - 10, 20, 20);
    QRect bottomRight = QRect(rectZone.bottomRight().x() - 10, rectZone.bottomRight().y() - 10, 20, 20);
    QRect bottomLeft  = QRect(rectZone.bottomLeft().x()  - 10, rectZone.bottomLeft().y()  - 10, 20, 20);

    switch (state) {
        case STATE_EMPTY:
            state = STATE_SELECTING;
            rectZone.setRect(event->x(), event->y(), 0, 0);
            break;
        case STATE_SELECTED:
            if      (topRight.contains(event->pos()))    state = STATE_SCALING_TR;
            else if (topLeft.contains(event->pos()))     state = STATE_SCALING_TL;
            else if (bottomRight.contains(event->pos())) state = STATE_SCALING_BR;
            else if (bottomLeft.contains(event->pos()))  state = STATE_SCALING_BL;
            else if (rectZone.contains(event->pos()))
            {
                state = STATE_MOVING;
                deltaPosition.setX(event->x() - rectZone.x());
                deltaPosition.setY(event->y() - rectZone.y());
            }
            else if (!rectZone.contains(event->pos()))
            {
                reset();
            }
            break;
        default: break;
    }
}

void SelectTool::mouseMove(QMouseEvent* event)
{
    switch (state) {
        case STATE_SELECTING:
            rectZone.setWidth(event->x() - rectZone.x());
            rectZone.setHeight(event->y() - rectZone.y());
            break;
        case STATE_MOVING:
            rectZone.moveTo(event->x() - deltaPosition.x(), event->y() - deltaPosition.y());
            break;
        case STATE_SCALING_TR: rectZone.setTopRight(event->pos()); break;
        case STATE_SCALING_TL: rectZone.setTopLeft(event->pos()); break;
        case STATE_SCALING_BR: rectZone.setBottomRight(event->pos()); break;
        case STATE_SCALING_BL: rectZone.setBottomLeft(event->pos()); break;
        default: break;
    }
}

void SelectTool::mouseRelease(QMouseEvent*)
{
    bool horMirror = rectZone.x() > rectZone.x() + rectZone.width();
    bool verMirror = rectZone.y() > rectZone.y() + rectZone.height();
    int tempX = horMirror ? rectZone.x() + rectZone.width(): rectZone.x();
    int tempY = verMirror ? rectZone.y() + rectZone.height(): rectZone.y();
    int tempW = abs(rectZone.width());
    int tempH = abs(rectZone.height());

    switch (state) {
        case STATE_SELECTING:
            state = STATE_SELECTED;
            rectZone.setRect(tempX, tempY, tempW, tempH);
            initialRectZone.setRect(rectZone.x(), rectZone.y(), rectZone.width(), rectZone.height());
            initialImage = animation()->getImageAt(timeline()->getLayer(), timeline()->getPos())->copy(initialRectZone);
            deltaImage = initialImage.copy();
            break;
        case STATE_MOVING:
            state = STATE_SELECTED;
            break;
        case STATE_SCALING_TR: case STATE_SCALING_TL: case STATE_SCALING_BR: case STATE_SCALING_BL:
            state = STATE_SELECTED;
            deltaImage = initialImage.scaled(tempW, tempH).mirrored(horMirror, verMirror);
            break;
        default: break;
    }
}

void SelectTool::paint(QPaintEvent*, QPainter* painter)
{
    bool horMirror = rectZone.x() > rectZone.x() + rectZone.width();
    bool verMirror = rectZone.y() > rectZone.y() + rectZone.height();
    int tempX = horMirror ? rectZone.x() + rectZone.width(): rectZone.x();
    int tempY = verMirror ? rectZone.y() + rectZone.height(): rectZone.y();
    int tempW = abs(rectZone.width());
    int tempH = abs(rectZone.height());

    switch (state) {
        case STATE_SELECTING:
            painter->setPen(QPen(Qt::red, 1, Qt::DashLine));
            painter->drawRect(rectZone);
            break;
        case STATE_MOVING:
            painter->setPen(QPen(Qt::black, 1, Qt::DashLine));
            painter->setBrush(QBrush(Qt::BrushStyle::Dense6Pattern));
            painter->drawRect(initialRectZone);
            painter->drawImage(QPoint(tempX, tempY), initialImage.scaled(tempW, tempH).mirrored(horMirror, verMirror));
            painter->setPen(QPen(Qt::darkGreen, 1, Qt::DashLine));
            painter->setBrush(QBrush(Qt::transparent));
            painter->drawRect(rectZone);
            break;
        case STATE_SCALING_TR: case STATE_SCALING_TL: case STATE_SCALING_BR: case STATE_SCALING_BL:
            painter->setPen(QPen(Qt::black, 1, Qt::DashLine));
            painter->setBrush(QBrush(Qt::BrushStyle::Dense6Pattern));
            painter->drawRect(initialRectZone);
            painter->drawImage(QPoint(tempX, tempY), initialImage.scaled(tempW, tempH).mirrored(horMirror, verMirror));
            painter->setPen(QPen(Qt::yellow, 1, Qt::DashLine));
            painter->setBrush(QBrush(Qt::transparent));
            painter->drawRect(rectZone);
            break;
       case STATE_SELECTED:
            painter->setPen(QPen(Qt::black, 1, Qt::DashLine));
            painter->setBrush(QBrush(Qt::BrushStyle::Dense6Pattern));
            painter->drawRect(initialRectZone);
            painter->drawImage(QPoint(tempX, tempY), initialImage.scaled(tempW, tempH).mirrored(horMirror, verMirror));
            painter->setPen(QPen(Qt::blue, 1, Qt::DashLine));
            painter->setBrush(QBrush(Qt::transparent));
            painter->drawRect(rectZone);
            painter->setPen(Qt::black);
            painter->setBrush(Qt::red);
            painter->drawRect(rectZone.topRight().x() - 5, rectZone.topRight().y() - 5, 10, 10);
            painter->drawRect(rectZone.topLeft().x() - 5, rectZone.topLeft().y() - 5, 10, 10);
            painter->drawRect(rectZone.bottomRight().x() - 5, rectZone.bottomRight().y() - 5, 10, 10);
            painter->drawRect(rectZone.bottomLeft().x() - 5, rectZone.bottomLeft().y() - 5, 10, 10);
            break;
        default: break;
    }
}

void SelectTool::draw()
{
    QImage i = animation()->copyImageAt(timeline()->getLayer(), timeline()->getPos());
    QImage j = i.copy();
    undostack()->push(new ModifyImageCommand(i, j, timeline()->getLayer(), timeline()->getPos(), animation()));
}

void SelectTool::reset()
{
    state = STATE_EMPTY;
    rectZone = QRect(0,0,1,1);
    initialRectZone = QRect(0,0,1,1);
    deltaPosition = QPoint(0, 0);
    initialImage = QImage(1, 1, QImage::Format_ARGB32);
    deltaImage = QImage(1, 1, QImage::Format_ARGB32);
}
