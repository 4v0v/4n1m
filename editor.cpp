#include "object.h"
#include "editor.h"
#include "timeline.h"
#include "preview.h"
#include "commands.h"
#include "titlebar.h"
#include "menubar.h"

Editor::Editor(MainWindow* mainwindow): QWidget(mainwindow)
{
    parent = mainwindow;
    setCursor(Qt::CrossCursor);
}

void Editor::mousePressEvent(QMouseEvent *event)
{
    if (!object()->isKeyframe(timeline()->getLayer(), timeline()->getPos())) timeline()->addKeyframe();
    scribbling = true;
    stroke << QPoint(event->pos().x(), event->pos().y());
    update();
}

void Editor::mouseReleaseEvent(QMouseEvent*)
{
    scribbling = false;
    switch (currentTool)
    {
        case Tool::PEN: drawPenStroke(); break;
        case Tool::LASSOFILL: drawLassoFill(); break;
        case Tool::LINE: drawLine(); break;
        case Tool::ERASER: drawEraserStroke(); break;
    }
    stroke.clear();
    update();
}

void Editor::mouseMoveEvent(QMouseEvent *event)
{
    if (!scribbling) return;
    stroke << QPoint(event->pos().x(), event->pos().y());
    update();
}

void Editor::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    // Background
    QImage backgroundImage = QImage(width(), height(), QImage::Format_ARGB32);
    backgroundImage.fill(backgroundColor);
    painter.drawImage(event->rect(), backgroundImage, event->rect());

    // Draw editor from layers
    object()->foreachLayerRevert([&painter, &event, this](int i){
        if (!object()->isKeyframe(i,  getPos(i))) return;
        QImage img = object()->getKeyframeImageAt(i, getPos(i))->copy();
        QPainter p(&img);

        if (i == timeline()->getLayer()){
            // Onionskin
            if (onionskinVisible)
            {
                QPainterPath path;
                path.addRect(0, 0, width(), height());
                int prev = object()->getPrevKeyframePos(i, getPos());
                int prevprev = object()->getPrevKeyframePos(i, prev);
                int next = object()->getNextKeyframePos(i, getPos());
                int nextnext = object()->getNextKeyframePos(i, next);

                if (prev < getPos())
                {
                    painter.setOpacity(0.3);
                    QImage img = object()->getKeyframeImageAt(i, prev)->copy();
                    QPainter p(&img);
                    p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
                    p.fillPath(path, Qt::red);
                    painter.drawImage(event->rect(), img, event->rect());
                }
                if (prevprev < getPos())
                {
                    painter.setOpacity(0.1);
                    QImage img = object()->getKeyframeImageAt(i, prevprev)->copy();
                    QPainter p(&img);
                    p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
                    p.fillPath(path, Qt::red);
                    painter.drawImage(event->rect(), img, event->rect());
                }
                if (next > getPos())
                {
                    painter.setOpacity(0.3);
                    QImage img = *object()->getKeyframeImageAt(i, next);
                    QPainter p(&img);
                    p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
                    p.fillPath(path, Qt::blue);
                    painter.drawImage(event->rect(), img, event->rect());
                }
                if (nextnext > getPos())
                {
                    painter.setOpacity(0.1);
                    QImage img = object()->getKeyframeImageAt(i, nextnext)->copy();
                    QPainter p(&img);
                    p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
                    p.fillPath(path, Qt::blue);
                    painter.drawImage(event->rect(), img, event->rect());
                }
                if (getPos() == object()->getFirstKeyframePos(i) && object()->getKeyframesCount(i) > 3)
                {
                    painter.setOpacity(0.3);
                    QImage img = object()->getKeyframeImageAt(i, object()->getLastKeyframePos(i))->copy();
                    QPainter p(&img);
                    p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
                    p.fillPath(path, Qt::darkGreen);
                    painter.drawImage(event->rect(), img, event->rect());
                }
                if (getPos() == object()->getLastKeyframePos(i) && object()->getKeyframesCount(i) > 3)
                {
                    painter.setOpacity(0.3);
                    QImage img = *object()->getKeyframeImageAt(i, object()->getFirstKeyframePos(i));
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
        if (layerTransparencyVisible && i != timeline()->getLayer()) painter.setOpacity(layerTransparency);
        painter.drawImage(event->rect(), img, event->rect());
        painter.setOpacity(1.00);
    });
}

void Editor::drawPenStroke()
{
    QImage i = object()->getKeyframeImageAt(timeline()->getLayer(), getPos())->copy();
    QImage j = i.copy();
    QPainter painter(&j);
    painter.setPen(linePen);
    painter.drawPolyline(stroke);
    undostack()->push(new ModifyImageCommand(i, j, timeline()->getLayer(), timeline()->getPos(), object()));
}

void Editor::drawLine()
{
    QImage i = object()->getKeyframeImageAt(timeline()->getLayer(), getPos())->copy();
    QImage j = i.copy();
    QPainter painter(&j);
    painter.setPen(linePen);
    QPolygon line;
    line << stroke.first();
    line << stroke.last();
    painter.drawPolyline(line);
    undostack()->push(new ModifyImageCommand(i, j, timeline()->getLayer(), timeline()->getPos(), object()));
}

void Editor::drawLassoFill()
{
    QImage i = object()->getKeyframeImageAt(timeline()->getLayer(), getPos())->copy();
    QImage j = i.copy();
    QPainter painter(&j);
    QPainterPath path;
    path.addPolygon(stroke);
    painter.fillPath(path, lassoBrush);
    undostack()->push(new ModifyImageCommand(i, j, timeline()->getLayer(), timeline()->getPos(), object()));
}

void Editor::drawEraserStroke()
{
    QImage i = object()->getKeyframeImageAt(timeline()->getLayer(), getPos())->copy();
    QImage j = i.copy();
    QImage k = i.copy();
    k.fill(Qt::transparent);
    QPainter painter(&k);
    painter.setPen(eraserPen);
    painter.drawPolyline(stroke);
    QPainter painter2(&j);
    painter2.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    painter2.drawImage(QPoint(0,0), k);
    undostack()->push(new ModifyImageCommand(i, j, timeline()->getLayer(), timeline()->getPos(), object()));
}

void Editor::clearImage()
{
    if (scribbling || !object()->isKeyframe(timeline()->getLayer(), getPos())) return;
    QImage i = object()->getKeyframeImageAt(timeline()->getLayer(), getPos())->copy();
    QImage j = i.copy();;
    j.fill(Qt::transparent);

    undostack()->push(new ModifyImageCommand(i, j, timeline()->getLayer(), timeline()->getPos(), object()));
}

void Editor::toggleOnionskin()
{
    onionskinVisible = !onionskinVisible;
    update();
}

void Editor::toggleLayerTransparency()
{
    layerTransparencyVisible = !layerTransparencyVisible;
    update();
}

int Editor::getPos(int layer)
{
    if (layer == -1) layer = timeline()->getLayer();
    return object()->isKeyframe(layer, timeline()->getPos()) ?
    timeline()->getPos() :
    object()->getPrevKeyframePos(layer, timeline()->getPos());
}
