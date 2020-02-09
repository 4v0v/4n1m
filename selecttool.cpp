#include "animation.h"
#include "preview.h"
#include "commands.h"
#include "editor.h"
#include "timeline.h"
#include "titlebar.h"
#include "toolbar.h"
#include "selecttool.h"

SelectTool::SelectTool(MainWindow* mw): QWidget(mw)
{
    mainwindow = mw;
}

void SelectTool::mousePress(QMouseEvent* event, int imgX, int imgY)
{
    QRect topRight    = QRect(deltaRectZone.topRight().x()    - 10, deltaRectZone.topRight().y()    - 10, 20, 20);
    QRect topLeft     = QRect(deltaRectZone.topLeft().x()     - 10, deltaRectZone.topLeft().y()     - 10, 20, 20);
    QRect bottomRight = QRect(deltaRectZone.bottomRight().x() - 10, deltaRectZone.bottomRight().y() - 10, 20, 20);
    QRect bottomLeft  = QRect(deltaRectZone.bottomLeft().x()  - 10, deltaRectZone.bottomLeft().y()  - 10, 20, 20);
    if (pasted) pasted = false;

    switch (state) {
        case STATE_EMPTY:
            state = STATE_SELECTING;
            if (subtool == RECTANGLE) deltaRectZone.setRect(event->x(), event->y(), 0, 0);
            if (subtool == LASSO) initialPolyZone << QPoint(event->pos());
            break;
        case STATE_SELECTED:
            if      (topRight.contains(event->pos()))    state = STATE_SCALING_TR;
            else if (topLeft.contains(event->pos()))     state = STATE_SCALING_TL;
            else if (bottomRight.contains(event->pos())) state = STATE_SCALING_BR;
            else if (bottomLeft.contains(event->pos()))  state = STATE_SCALING_BL;
            else if (deltaRectZone.contains(event->pos()))
            {
                state = STATE_MOVING;
                deltaPosition.setX(event->x() - deltaRectZone.x());
                deltaPosition.setY(event->y() - deltaRectZone.y());
            }
            else if (!deltaRectZone.contains(event->pos()))
            {
                draw(imgX, imgY);
                reset();
                if (subtool == RECTANGLE) deltaRectZone.setRect(event->x(), event->y(), 0, 0);
                if (subtool == LASSO) initialPolyZone << QPoint(event->pos());
                state = STATE_SELECTING;
            }
            break;
        default: break;
    }
}

void SelectTool::mouseMove(QMouseEvent* event)
{
    switch (state) {
        case STATE_SCALING_TR: deltaRectZone.setTopRight(event->pos()); break;
        case STATE_SCALING_TL: deltaRectZone.setTopLeft(event->pos()); break;
        case STATE_SCALING_BR: deltaRectZone.setBottomRight(event->pos()); break;
        case STATE_SCALING_BL: deltaRectZone.setBottomLeft(event->pos()); break;
        case STATE_MOVING: deltaRectZone.moveTo(event->x() - deltaPosition.x(), event->y() - deltaPosition.y()); break;
        case STATE_SELECTING: {
            if (subtool == RECTANGLE) deltaRectZone.setSize(QSize(event->x() - deltaRectZone.x(), event->y() - deltaRectZone.y()));
            if (subtool == LASSO) initialPolyZone << QPoint(event->pos());
            break;
      } default: break;
    }
}

void SelectTool::mouseRelease(QMouseEvent*, int imgX, int imgY)
{
    if (state == STATE_SELECTING && subtool == LASSO) deltaRectZone = initialPolyZone.boundingRect();
    bool horMirror = deltaRectZone.x() > deltaRectZone.x() + deltaRectZone.width();
    bool verMirror = deltaRectZone.y() > deltaRectZone.y() + deltaRectZone.height();
    int tempX = horMirror ? deltaRectZone.x() + deltaRectZone.width(): deltaRectZone.x();
    int tempY = verMirror ? deltaRectZone.y() + deltaRectZone.height(): deltaRectZone.y();
    int tempW = abs(deltaRectZone.width());
    int tempH = abs(deltaRectZone.height());

    switch (state) {
        case STATE_MOVING: state = STATE_SELECTED; break;
        case STATE_SCALING_TR: case STATE_SCALING_TL: case STATE_SCALING_BR: case STATE_SCALING_BL:
            state = STATE_SELECTED;
            deltaImage = initialImage.scaled(tempW, tempH).mirrored(horMirror, verMirror);
            break;
        case STATE_SELECTING:
            if (tempW < 20 || tempH < 20) {reset(); break;}
            state = STATE_SELECTED;
            deltaRectZone.setRect(tempX, tempY, tempW, tempH);
            initialRectZone.setRect(tempX - imgX, tempY - imgY, tempW, tempH);

            if (subtool == RECTANGLE) {
                initialImage = animation()->getImageAt(timeline()->getLayer(), timeline()->getPos())->copy(initialRectZone);
            }
            if (subtool == LASSO) {
                QPolygon p = initialPolyZone.translated(-tempX, -tempY);
                QImage i = animation()->getImageAt(timeline()->getLayer(), timeline()->getPos())->copy(initialRectZone);
                QImage j = i.copy();
                QPainter painter1(&i);
                QPainter painter2(&j);
                j.fill(Qt::transparent);
                painter2.setPen(Qt::red); painter2.setBrush(Qt::red);
                painter2.drawPolygon(p);
                painter1.setCompositionMode(QPainter::CompositionMode_DestinationIn);
                painter1.drawImage(QPoint(0, 0), j);
                initialImage = i.copy();
            }

            deltaImage = initialImage.copy();
            break;

        default: break;
    }
}

void SelectTool::paintLayer(QPainter* painter, int imgX, int imgY)
{
    if (state == STATE_EMPTY || state == STATE_SELECTING) return;
    bool horMirror = deltaRectZone.x() > deltaRectZone.x() + deltaRectZone.width();
    bool verMirror = deltaRectZone.y() > deltaRectZone.y() + deltaRectZone.height();
    int tempX = horMirror ? deltaRectZone.x() + deltaRectZone.width(): deltaRectZone.x();
    int tempY = verMirror ? deltaRectZone.y() + deltaRectZone.height(): deltaRectZone.y();
    int tempW = abs(deltaRectZone.width());
    int tempH = abs(deltaRectZone.height());

    painter->setCompositionMode(QPainter::CompositionMode_DestinationOut);
    painter->setPen(Qt::transparent); painter->setBrush(Qt::black);
    if (subtool == RECTANGLE) painter->drawRect(initialRectZone.translated(imgX, imgY));
    if (subtool == LASSO) painter->drawPolygon(initialPolyZone);
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter->drawImage(QPoint(tempX, tempY), initialImage.scaled(tempW, tempH).mirrored(horMirror, verMirror));
}

void SelectTool::paintGlobal(QPainter* painter)
{
    if (state == STATE_EMPTY) return;

    if (state == STATE_SELECTING) {
        painter->setPen(QPen(Qt::red, 1, Qt::DashLine));
        painter->setBrush(Qt::transparent);
        if (subtool == RECTANGLE) painter->drawRect(deltaRectZone);
        if (subtool == LASSO) painter->drawPolygon(initialPolyZone);
    } else {
        painter->setPen(QPen(state == STATE_SELECTED && !pasted ? Qt::blue : Qt::yellow, 1, Qt::DashLine));
        painter->setBrush(Qt::transparent);
        painter->drawRect(deltaRectZone);
        if (state == STATE_SELECTED){
            painter->setPen(Qt::black);
            painter->setBrush(pasted ? Qt::yellow :Qt::red);
            painter->drawRect(deltaRectZone.topRight().x() - 5, deltaRectZone.topRight().y() - 5, 10, 10);
            painter->drawRect(deltaRectZone.topLeft().x() - 5, deltaRectZone.topLeft().y() - 5, 10, 10);
            painter->drawRect(deltaRectZone.bottomRight().x() - 5, deltaRectZone.bottomRight().y() - 5, 10, 10);
            painter->drawRect(deltaRectZone.bottomLeft().x() - 5, deltaRectZone.bottomLeft().y() - 5, 10, 10);
        }
    }
}

void SelectTool::draw(int imgX, int imgY)
{
    if (
        deltaRectZone.width() == initialRectZone.width() &&
        deltaRectZone.height() == initialRectZone.height() &&
        deltaRectZone.x() == initialRectZone.x() &&
        deltaRectZone.y() == initialRectZone.y()
    ) return ;

    QImage i = animation()->copyImageAt(timeline()->getLayer(), timeline()->getPos());
    QImage j = i.copy();
    QPainter painter(&j);
    int tempX = deltaRectZone.x() > deltaRectZone.x() + deltaRectZone.width() ? deltaRectZone.x() + deltaRectZone.width(): deltaRectZone.x();
    int tempY = deltaRectZone.y() > deltaRectZone.y() + deltaRectZone.height() ? deltaRectZone.y() + deltaRectZone.height(): deltaRectZone.y();

    painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    painter.setPen(Qt::transparent); painter.setBrush(Qt::black);
    if (subtool == RECTANGLE) painter.drawRect(initialRectZone);
    if (subtool == LASSO) painter.drawPolygon(initialPolyZone.translated(-imgX, -imgY));
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(QPoint(tempX - imgX, tempY - imgY), deltaImage);

    undostack()->push(new ModifyImageCommand(i, j, timeline()->getLayer(), timeline()->getPos(), animation()));
}

void SelectTool::knockback()
{
}

void SelectTool::clear()
{
    QImage i = animation()->copyImageAt(timeline()->getLayer(), timeline()->getPos());
    QImage j = i.copy();
    QPainter painter(&j);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    painter.setPen(Qt::transparent); painter.setBrush(Qt::black);
    if (subtool == RECTANGLE) painter.drawRect(initialRectZone);
    if (subtool == LASSO) painter.drawPolygon(initialPolyZone);
    reset();
    undostack()->push(new ModifyImageCommand(i, j, timeline()->getLayer(), timeline()->getPos(), animation()));
}

void SelectTool::reset()
{
    state = STATE_EMPTY;
    initialPolyZone.clear();
    initialRectZone = QRect(0,0,1,1);
    initialImage = QImage(1, 1, QImage::Format_ARGB32);
    deltaRectZone = QRect(0,0,1,1);
    deltaImage = QImage(1, 1, QImage::Format_ARGB32);
    deltaPosition = QPoint(0, 0);
    pasted = false;
}
