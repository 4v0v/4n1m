#include <QtWidgets>
#include "editor.h"
#include "timeline.h"
#include "object.h"
#include "commands.h"

Editor::Editor(Object *o, QUndoStack* u, QWidget* parent): QWidget(parent)
{
    undoStack = u;
    object = o;
    this->setCursor(Qt::CrossCursor);
    object->addKeyframeAt(0, 0, QImage(this->width(), this->height(), QImage::Format_ARGB32));
    object->addKeyframeAt(1, 0, QImage(this->width(), this->height(), QImage::Format_ARGB32));
    this->update();
}

void Editor::mousePressEvent(QMouseEvent *event)
{
    if (!object->isKeyframe(timeline->getLayer(), timeline->getPos())) timeline->addKeyframe();

    scribbling = true;
    switch (currentTool)
    {
        case Tool::PEN: penStroke << QPoint(event->pos().x(), event->pos().y()); break;
        case Tool::LASSOFILL: lassoFill << QPoint(event->pos().x(), event->pos().y()); break;
        case Tool::ERASER: eraserStroke << QPoint(event->pos().x(), event->pos().y()); break;
    }
    this->update();
}

void Editor::mouseReleaseEvent(QMouseEvent*)
{
    scribbling = false;
    switch (currentTool)
    {
        case Tool::PEN:
            this->drawPenStroke();
            penStroke.clear();
            break;
        case Tool::LASSOFILL:
            this->drawLassoFill();
            lassoFill.clear();
            break;
        case Tool::ERASER:
            this->drawEraserStroke();
            eraserStroke.clear();
            break;
    }
    this->update();
}

void Editor::mouseMoveEvent(QMouseEvent *event)
{
    if (!scribbling) return;
    switch (currentTool)
    {
        case Tool::PEN: penStroke << QPoint(event->pos().x(), event->pos().y()); break;
        case Tool::LASSOFILL: lassoFill << QPoint(event->pos().x(), event->pos().y()); break;
        case Tool::ERASER: eraserStroke << QPoint(event->pos().x(), event->pos().y()); break;
    }
    this->update();
}

void Editor::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    // Background
    QImage backgroundImage = QImage(this->width(), this->height(), QImage::Format_ARGB32);
    backgroundImage.fill(backgroundColor);
    painter.drawImage(event->rect(), backgroundImage, event->rect());

    // Onionskin
    if (onionskinVisible)
    {
        int prev = object->getPrevKeyframePos(timeline->getLayer(), this->getPos());
        int next = object->getNextKeyframePos(timeline->getLayer(), this->getPos());
        QPainterPath path;
        path.addRect(0, 0, width(), height());

        if (prev < this->getPos())
        {
            painter.setOpacity(0.3);
            QImage img = *object->getKeyframeImageAt(timeline->getLayer(), prev);
            QPainter p(&img);
            p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            p.fillPath(path, Qt::red);
            painter.drawImage(event->rect(), img, event->rect());
        }
        if (object->getPrevKeyframePos(timeline->getLayer(), prev) < this->getPos())
        {
            painter.setOpacity(0.1);
            QImage img = *object->getKeyframeImageAt(timeline->getLayer(), object->getPrevKeyframePos(timeline->getLayer(), prev));
            QPainter p(&img);
            p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            p.fillPath(path, Qt::red);
            painter.drawImage(event->rect(), img, event->rect());
        }
        if (next > this->getPos())
        {
            painter.setOpacity(0.3);
            QImage img = *object->getKeyframeImageAt(timeline->getLayer(), next);
            QPainter p(&img);
            p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            p.fillPath(path, Qt::blue);
            painter.drawImage(event->rect(), img, event->rect());
        }
        if (object->getNextKeyframePos(timeline->getLayer(), next) > this->getPos())
        {
            painter.setOpacity(0.1);
            QImage img = *object->getKeyframeImageAt(timeline->getLayer(), object->getNextKeyframePos(timeline->getLayer(), next));
            QPainter p(&img);
            p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            p.fillPath(path, Qt::blue);
            painter.drawImage(event->rect(), img, event->rect());
        }
        if (this->getPos() == object->getFirstKeyframePos(timeline->getLayer()) && object->getKeyframesCount(timeline->getLayer()) > 3)
        {
            painter.setOpacity(0.3);
            QImage img = *object->getKeyframeImageAt(timeline->getLayer(), object->getLastKeyframePos(timeline->getLayer()));
            QPainter p(&img);
            p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            p.fillPath(path, Qt::darkGreen);
            painter.drawImage(event->rect(), img, event->rect());
        }
        if (this->getPos() == object->getLastKeyframePos(timeline->getLayer()) && object->getKeyframesCount(timeline->getLayer()) > 3)
        {
            painter.setOpacity(0.3);
            QImage img = *object->getKeyframeImageAt(timeline->getLayer(), object->getFirstKeyframePos(timeline->getLayer()));
            QPainter p(&img);
            p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            p.fillPath(path, Qt::darkGreen);
            painter.drawImage(event->rect(), img, event->rect());
        }

        painter.setOpacity(1.00);
    }

    // Current image
    QImage currentImg = object->getKeyframeImageAt(timeline->getLayer(), this->getPos())->copy();
    painter.drawImage(event->rect(), currentImg, event->rect());

    // Tool previews
    switch (currentTool) {
        case Tool::PEN:
        {
            painter.setPen(linePen);
            painter.drawPolyline(penStroke);
            break;
        }
        case Tool::LASSOFILL:
        {
            QPainterPath path;
            path.addPolygon(lassoFill);
            painter.fillPath(path, lassoBrush);
            break;
        }
        case Tool::ERASER:
        {
            painter.setPen(eraserPen);
            painter.drawPolyline(eraserStroke);
            break;
        }
    }
}

void Editor::drawPenStroke()
{
    QImage i = object->getKeyframeImageAt(timeline->getLayer(), this->getPos())->copy();
    QImage j = object->getKeyframeImageAt(timeline->getLayer(), this->getPos())->copy();
    QPainter painter(&j);
    painter.setPen(linePen);
    painter.drawPolyline(penStroke);

    undoStack->push(new ModifyImageCommand(i, j, timeline->getLayer(), this->getPos(), this->object));
}

void Editor::drawLassoFill()
{
    QImage i = object->getKeyframeImageAt(timeline->getLayer(), this->getPos())->copy();
    QImage j = object->getKeyframeImageAt(timeline->getLayer(), this->getPos())->copy();
    QPainter painter(&j);
    QPainterPath path;
    path.addPolygon(lassoFill);
    painter.fillPath(path, lassoBrush);

    undoStack->push(new ModifyImageCommand(i, j, timeline->getLayer(), this->getPos(), this->object));
}

void Editor::drawEraserStroke()
{
    QImage i = object->getKeyframeImageAt(timeline->getLayer(), this->getPos())->copy();
    QImage j = object->getKeyframeImageAt(timeline->getLayer(), this->getPos())->copy();
    QImage k = object->getKeyframeImageAt(timeline->getLayer(), this->getPos())->copy();
    k.fill(Qt::transparent);
    QPainter painter(&k);
    painter.setPen(eraserPen);
    painter.drawPolyline(eraserStroke);

    QPainter painter2(&j);
    painter2.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    painter2.drawImage(QPoint(0,0), k);

    undoStack->push(new ModifyImageCommand(i, j, timeline->getLayer(), this->getPos(), this->object));
}

void Editor::clearImage()
{
    if (scribbling) return;
    QImage i = object->getKeyframeImageAt(timeline->getLayer(), this->getPos())->copy();
    QImage j = object->getKeyframeImageAt(timeline->getLayer(), this->getPos())->copy();
    j.fill(Qt::transparent);

    undoStack->push(new ModifyImageCommand(i, j, timeline->getLayer(), this->getPos(), this->object));
}

void Editor::toggleOnionskin()
{
    onionskinVisible = !onionskinVisible;
    this->update();
}
