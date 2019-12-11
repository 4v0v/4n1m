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
    object->addKeyframeAt(0, QImage(this->width(), this->height(), QImage::Format_ARGB32));
    this->update();
}

void Editor::mousePressEvent(QMouseEvent *event)
{
    if (!object->isKeyframe(timeline->getPos())) timeline->addKeyframe();

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
        int prev = object->getPrevKeyframePos(this->getPos());
        int next = object->getNextKeyframePos(this->getPos());
        QPainterPath path;
        path.addRect(0, 0, width(), height());

        if (prev < this->getPos())
        {
            painter.setOpacity(0.3);
            QImage img = *object->getKeyframeImageAt(prev);
            QPainter p(&img);
            p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            p.fillPath(path, Qt::red);
            painter.drawImage(event->rect(), img, event->rect());
        }
        if (object->getPrevKeyframePos(prev) < this->getPos())
        {
            painter.setOpacity(0.1);
            QImage img = *object->getKeyframeImageAt(object->getPrevKeyframePos(prev));
            QPainter p(&img);
            p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            p.fillPath(path, Qt::red);
            painter.drawImage(event->rect(), img, event->rect());
        }
        if (next > this->getPos())
        {
            painter.setOpacity(0.3);
            QImage img = *object->getKeyframeImageAt(next);
            QPainter p(&img);
            p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            p.fillPath(path, Qt::blue);
            painter.drawImage(event->rect(), img, event->rect());
        }
        if (object->getNextKeyframePos(next) > this->getPos())
        {
            painter.setOpacity(0.1);
            QImage img = *object->getKeyframeImageAt(object->getNextKeyframePos(next));
            QPainter p(&img);
            p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            p.fillPath(path, Qt::blue);
            painter.drawImage(event->rect(), img, event->rect());
        }
        if (this->getPos() == object->getFirstKeyframePos() && object->getKeyframesCount() > 3)
        {
            painter.setOpacity(0.3);
            QImage img = *object->getKeyframeImageAt(object->getLastKeyframePos());
            QPainter p(&img);
            p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            p.fillPath(path, Qt::darkGreen);
            painter.drawImage(event->rect(), img, event->rect());
        }
        if (this->getPos() == object->getLastKeyframePos() && object->getKeyframesCount() > 3)
        {
            painter.setOpacity(0.3);
            QImage img = *object->getKeyframeImageAt(object->getFirstKeyframePos());
            QPainter p(&img);
            p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            p.fillPath(path, Qt::darkGreen);
            painter.drawImage(event->rect(), img, event->rect());
        }

        painter.setOpacity(1.00);
    }

    // Current image
    painter.drawImage(event->rect(), *object->getKeyframeImageAt(this->getPos()), event->rect());

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
    QImage i = object->getKeyframeImageAt(this->getPos())->copy();
    QImage j = object->getKeyframeImageAt(this->getPos())->copy();
    QPainter painter(&j);
    painter.setPen(linePen);
    painter.drawPolyline(penStroke);

    undoStack->push(new ModifyImageCommand(i, j, this->getPos(), this->object));
}

void Editor::drawLassoFill()
{
    QImage i = object->getKeyframeImageAt(this->getPos())->copy();
    QImage j = object->getKeyframeImageAt(this->getPos())->copy();
    QPainter painter(&j);
    QPainterPath path;
    path.addPolygon(lassoFill);
    painter.fillPath(path, lassoBrush);

    undoStack->push(new ModifyImageCommand(i, j, this->getPos(), this->object));
}

void Editor::drawEraserStroke()
{
    QImage i = object->getKeyframeImageAt(this->getPos())->copy();
    QImage j = object->getKeyframeImageAt(this->getPos())->copy();
    QImage k = object->getKeyframeImageAt(this->getPos())->copy();
    k.fill(Qt::transparent);
    QPainter painter(&k);
    painter.setPen(eraserPen);
    painter.drawPolyline(eraserStroke);

    QPainter painter2(&j);
    painter2.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    painter2.drawImage(QPoint(0,0), k);

    undoStack->push(new ModifyImageCommand(i, j, this->getPos(), this->object));
}

void Editor::clearImage()
{
    if (scribbling) return;
    QImage i = object->getKeyframeImageAt(this->getPos())->copy();
    QImage j = object->getKeyframeImageAt(this->getPos())->copy();
    j.fill(Qt::transparent);

    undoStack->push(new ModifyImageCommand(i, j, this->getPos(), this->object));
}

void Editor::toggleOnionskin()
{
    onionskinVisible = !onionskinVisible;
    this->update();
}
