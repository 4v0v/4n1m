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
        case Tool::LINE: this->drawLine(); break;
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


    // Draw editor from layers
    object->foreachLayerRevert([&painter, &event, this](int i){
        QImage img = object->getKeyframeImageAt(i, this->getPos(i))->copy();
        QPainter p(&img);

        if (i == timeline->getLayer()){
            // Onionskin
            if (onionskinVisible)
            {
                QPainterPath path;
                path.addRect(0, 0, width(), height());
                int prev = object->getPrevKeyframePos(i, this->getPos());
                int prevprev = object->getPrevKeyframePos(i, prev);
                int next = object->getNextKeyframePos(i, this->getPos());
                int nextnext = object->getNextKeyframePos(i, next);

                if (prev < this->getPos())
                {
                    painter.setOpacity(0.3);
                    QImage img = object->getKeyframeImageAt(i, prev)->copy();
                    QPainter p(&img);
                    p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
                    p.fillPath(path, Qt::red);
                    painter.drawImage(event->rect(), img, event->rect());
                }
                if (prevprev < this->getPos())
                {
                    painter.setOpacity(0.1);
                    QImage img = object->getKeyframeImageAt(i, prevprev)->copy();
                    QPainter p(&img);
                    p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
                    p.fillPath(path, Qt::red);
                    painter.drawImage(event->rect(), img, event->rect());
                }
                if (next > this->getPos())
                {
                    painter.setOpacity(0.3);
                    QImage img = *object->getKeyframeImageAt(i, next);
                    QPainter p(&img);
                    p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
                    p.fillPath(path, Qt::blue);
                    painter.drawImage(event->rect(), img, event->rect());
                }
                if (nextnext > this->getPos())
                {
                    painter.setOpacity(0.1);
                    QImage img = object->getKeyframeImageAt(i, nextnext)->copy();
                    QPainter p(&img);
                    p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
                    p.fillPath(path, Qt::blue);
                    painter.drawImage(event->rect(), img, event->rect());
                }
                if (this->getPos() == object->getFirstKeyframePos(i) && object->getKeyframesCount(i) > 3)
                {
                    painter.setOpacity(0.3);
                    QImage img = object->getKeyframeImageAt(i, object->getLastKeyframePos(i))->copy();
                    QPainter p(&img);
                    p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
                    p.fillPath(path, Qt::darkGreen);
                    painter.drawImage(event->rect(), img, event->rect());
                }
                if (this->getPos() == object->getLastKeyframePos(i) && object->getKeyframesCount(i) > 3)
                {
                    painter.setOpacity(0.3);
                    QImage img = *object->getKeyframeImageAt(i, object->getFirstKeyframePos(i));
                    QPainter p(&img);
                    p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
                    p.fillPath(path, Qt::darkGreen);
                    painter.drawImage(event->rect(), img, event->rect());
                }
                painter.setOpacity(1.00);
            }

            // Tool preview
            switch (currentTool) {
                case Tool::PEN:
                {
                    p.setPen(linePen);
                    p.drawPolyline(stroke);
                    break;
                }
                case Tool::LASSOFILL:
                {
                    QPainterPath path;
                    path.addPolygon(stroke);
                    p.fillPath(path, lassoBrush);
                    break;
                }
                case Tool::LINE:
                {
                    if (stroke.count() < 2) break;
                    p.setPen(linePen);
                    QPolygon line;
                    line << stroke.first();
                    line << stroke.last();
                    p.drawPolyline(line);
                    break;
                }
                case Tool::ERASER:
                {
                    QImage img2 = img.copy();
                    img2.fill(Qt::transparent);
                    QPainter p2(&img2);
                    p2.setPen(eraserPen);
                    p2.drawPolyline(stroke);
                    p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
                    p.drawImage(QPoint(0,0), img2);
                    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
                    break;
                }
            }
        }
        if (layerTransparencyVisible && i != timeline->getLayer()) painter.setOpacity(layerTransparency);
        painter.drawImage(event->rect(), img, event->rect());
        painter.setOpacity(1.00);
    });
}

void Editor::drawPenStroke()
{
    QImage i = object->getKeyframeImageAt(timeline->getLayer(), this->getPos())->copy();
    QImage j = i.copy();
    QPainter painter(&j);
    painter.setPen(linePen);
    painter.drawPolyline(stroke);
    undoStack->push(new ModifyImageCommand(i, j, timeline->getLayer(), timeline->getPos(), this->object));
}

void Editor::drawLine()
{
    QImage i = object->getKeyframeImageAt(timeline->getLayer(), this->getPos())->copy();
    QImage j = i.copy();
    QPainter painter(&j);
    painter.setPen(linePen);
    QPolygon line;
    line << stroke.first();
    line << stroke.last();
    painter.drawPolyline(line);
    undoStack->push(new ModifyImageCommand(i, j, timeline->getLayer(), timeline->getPos(), this->object));
}

void Editor::drawLassoFill()
{
    QImage i = object->getKeyframeImageAt(timeline->getLayer(), this->getPos())->copy();
    QImage j = i.copy();
    QPainter painter(&j);
    QPainterPath path;
    path.addPolygon(stroke);
    painter.fillPath(path, lassoBrush);
    undoStack->push(new ModifyImageCommand(i, j, timeline->getLayer(), timeline->getPos(), this->object));
}

void Editor::drawEraserStroke()
{
    QImage i = object->getKeyframeImageAt(timeline->getLayer(), this->getPos())->copy();
    QImage j = i.copy();
    QImage k = i.copy();
    k.fill(Qt::transparent);
    QPainter painter(&k);
    painter.setPen(eraserPen);
    painter.drawPolyline(stroke);
    QPainter painter2(&j);
    painter2.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    painter2.drawImage(QPoint(0,0), k);
    undoStack->push(new ModifyImageCommand(i, j, timeline->getLayer(), timeline->getPos(), this->object));
}

void Editor::clearImage()
{
    if (scribbling) return;
    QImage i = object->getKeyframeImageAt(timeline->getLayer(), this->getPos())->copy();
    QImage j = object->getKeyframeImageAt(timeline->getLayer(), this->getPos())->copy();
    j.fill(Qt::transparent);

    undoStack->push(new ModifyImageCommand(i, j, timeline->getLayer(), timeline->getPos(), this->object));
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
