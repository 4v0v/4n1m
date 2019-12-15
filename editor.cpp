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
    stroke << QPoint(event->pos().x(), event->pos().y());
    this->update();
}

void Editor::mouseReleaseEvent(QMouseEvent*)
{
    scribbling = false;
    switch (currentTool)
    {
        case Tool::PEN: this->drawPenStroke(); break;
        case Tool::LASSOFILL: this->drawLassoFill(); break;
        case Tool::ERASER: this->drawEraserStroke(); break;
    }
    stroke.clear();
    this->update();
}

void Editor::mouseMoveEvent(QMouseEvent *event)
{
    if (!scribbling) return;
    stroke << QPoint(event->pos().x(), event->pos().y());
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
        QPainterPath path;
        path.addRect(0, 0, width(), height());
        int prev = object->getPrevKeyframePos(timeline->getLayer(), this->getPos());
        int next = object->getNextKeyframePos(timeline->getLayer(), this->getPos());

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


    // Draw editor from layers
    object->foreachLayerRevert([&painter, &event, this](int i){
        QImage img = object->getKeyframeImageAt(i, this->getPos(i))->copy();
        if (layerTransparencyVisible &&i != timeline->getLayer()) painter.setOpacity(layerTransparency);
        painter.drawImage(event->rect(), img, event->rect());
        painter.setOpacity(1.00);

        // Tool previews
        if (i == timeline->getLayer()){
            switch (currentTool) {
                case Tool::PEN:
                {
                    painter.setPen(linePen);
                    painter.drawPolyline(stroke);
                    break;
                }
                case Tool::LASSOFILL:
                {
                    QPainterPath path;
                    path.addPolygon(stroke);
                    painter.fillPath(path, lassoBrush);
                    break;
                }
                case Tool::ERASER:
                {
                    painter.setPen(eraserPen);
                    painter.drawPolyline(stroke);
                    break;
                }
            }
        }
    });
}

void Editor::drawPenStroke()
{
    QImage i = object->getKeyframeImageAt(timeline->getLayer(), this->getPos())->copy();
    QImage j = object->getKeyframeImageAt(timeline->getLayer(), this->getPos())->copy();
    QPainter painter(&j);
    painter.setPen(linePen);
    painter.drawPolyline(stroke);

    undoStack->push(new ModifyImageCommand(i, j, timeline->getLayer(), this->getPos(), this->object));
}

void Editor::drawLassoFill()
{
    QImage i = object->getKeyframeImageAt(timeline->getLayer(), this->getPos())->copy();
    QImage j = object->getKeyframeImageAt(timeline->getLayer(), this->getPos())->copy();
    QPainter painter(&j);
    QPainterPath path;
    path.addPolygon(stroke);
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
    painter.drawPolyline(stroke);

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

void Editor::toggleLayerTransparency()
{
    layerTransparencyVisible = !layerTransparencyVisible;
    this->update();
}

int Editor::getPos(int layer)
{
    if (layer == -1) layer = timeline->getLayer();
    return object->isKeyframe(layer, timeline->getPos()) ?
    timeline->getPos() :
    object->getPrevKeyframePos(layer, timeline->getPos());
}
