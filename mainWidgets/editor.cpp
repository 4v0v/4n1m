#include "animation.h"
#include "preview.h"
#include "commands.h"
#include "mainWidgets/editor.h"
#include "mainWidgets/timeline.h"
#include "mainWidgets/titlebar.h"
#include "mainWidgets/toolbar.h"
#include "mainWidgets/selecttool.h"

Editor::Editor(MainWindow* mw): QWidget(mw)
{
    mainwindow = mw;
    setCursor(Qt::CrossCursor);
    selectTool = new SelectTool(mainwindow);
    setGeometry(0, 0, mainwindow->getWindowDimensions().width(), 530);
    setMouseTracking(true);
}

void Editor::mousePressEvent(QMouseEvent* event)
{
    mainwindow->subtoolbar->hideProperties();
    if (
        !animation()->isKey(timeline()->getLayer(), timeline()->getPos()) &&
        currentTool != EMPTY &&
        currentTool != ERASER &&
        currentTool != SELECT
    ) timeline()->addKey();

    scribbling = true;
    stroke << QPoint(event->pos().x(), event->pos().y());

    if (animation()->isKey(timeline()->getLayer(), timeline()->getPos())){
        switch (currentTool)
        {
            case SELECT: selectTool->mousePress(event); break;
            default: break;
        }
    }

    update();
}

void Editor::mouseMoveEvent(QMouseEvent* event)
{
    mousePosition.setX(event->x());
    mousePosition.setY(event->y());
    if (!scribbling) return;
    switch (currentTool)
    {
        case SELECT: selectTool->mouseMove(event); break;
        default: break;
    }
    stroke << QPoint(event->pos().x(), event->pos().y());
    update();
}

void Editor::mouseReleaseEvent(QMouseEvent* event)
{
    scribbling = false;
    switch (currentTool)
    {
        case PEN: drawPenStroke(); break;
        case FILL: drawFill(); break;
        case SHAPE: drawShape(); break;
        case ERASER: if (animation()->isKey(timeline()->getLayer(), timeline()->getPos())) drawEraserStroke(); break;
        case SELECT: selectTool->mouseRelease(event); break;
        default: break;
    }
    stroke.clear();
    update();
}

void Editor::paintEvent(QPaintEvent* event)
{
    QPainter globalPainter(this);

    // Background
    QImage backgroundImage = QImage(width(), height(), QImage::Format_ARGB32);
    backgroundImage.fill(backgroundColor);
    globalPainter.drawImage(event->rect(), backgroundImage, event->rect());

    // Draw editor from layers
    animation()->foreachLayerRevert([&globalPainter, &event, this](int i){
        if (animation()->getKeyCount(i) == 0) return;
        QImage img = QImage(width(), height(), QImage::Format_ARGB32);
        QPainter layerPainter(&img);

        // Draw current or previous keyframe
        if (animation()->isKey(i, getPos(i))) layerPainter.drawImage(QPoint(0,0), animation()->copyImageAt(i, getPos(i)));

        // Draw only on current layer
        if (i == timeline()->getLayer()){
            // Onionskin
            if (onionskinVisible)
            {
                QPainterPath path;
                path.addRect(0, 0, width(), height());
                int prev = animation()->getPrevKey(i, getPos());
                int prevprev = animation()->getPrevKey(i, prev);
                int next = animation()->getNextKey(i, getPos());
                int nextnext = animation()->getNextKey(i, next);

                if (onionskinloopVisible)
                {
                    if (getPos() == animation()->getFirstKey(i) && animation()->getKeyCount(i) > 3)
                        drawOnionSkin(event, &globalPainter, &path, onionOpacityLoop, i, animation()->getLastKey(i), QColor(Qt::darkGreen));
                    if (getPos() == animation()->getLastKey(i) && animation()->getKeyCount(i) > 3)
                        drawOnionSkin(event, &globalPainter, &path, onionOpacityLoop, i, animation()->getFirstKey(i), QColor(Qt::darkGreen));
                }

                if (prev < getPos() && prev != -1) drawOnionSkin(event, &globalPainter, &path, onionOpacityFirst, i, prev, QColor(Qt::red));
                if (prevprev < getPos() && prevprev != -1 ) drawOnionSkin(event, &globalPainter, &path, onionOpacitySecond, i, prevprev, QColor(Qt::red));
                if (next > getPos()) drawOnionSkin(event, &globalPainter, &path, onionOpacityFirst, i, next, QColor(Qt::blue));
                if (nextnext > getPos()) drawOnionSkin(event, &globalPainter, &path, onionOpacitySecond, i, nextnext, QColor(Qt::blue));

                globalPainter.setOpacity(1.0);
            }

            // Tool preview
            switch (currentTool)
            {
                case PEN: {
                    layerPainter.setPen(penTool);
                    if (stroke.count() == 1) layerPainter.drawPoint(stroke.first());
                    else if (stroke.count() > 1) layerPainter.drawPolyline(stroke);
                    break;
                } case SHAPE: {
                    if (stroke.count() < 2) break;
                    layerPainter.setPen(shapeTool);
                    switch(shapeSubtool){
                        case LINE: layerPainter.drawPolyline(QPolygon() << stroke.first() << stroke.last()); break;
                        case RECTANGLE: layerPainter.drawRect(stroke.first().x(),stroke.first().y(),stroke.last().x() - stroke.first().x(),stroke.last().y() - stroke.first().y()); break;
                        case ELLIPSE: layerPainter.drawEllipse(stroke.first().x(), stroke.first().y(), stroke.last().x() - stroke.first().x(), stroke.last().y() - stroke.first().y()); break;
                        default: break;
                    }
                    break;
                } case FILL: {
                    if (stroke.count() < 2) break;
                    layerPainter.setPen(Qt::transparent);
                    layerPainter.setBrush(filltool);
                    switch(fillSubtool){
                        case LASSO: layerPainter.drawPolygon(stroke); break;
                        case RECTANGLE: layerPainter.drawRect(stroke.first().x(),stroke.first().y(),stroke.last().x() - stroke.first().x(),stroke.last().y() - stroke.first().y()); break;
                        case ELLIPSE: layerPainter.drawEllipse(stroke.first().x(),stroke.first().y(),stroke.last().x() - stroke.first().x(),stroke.last().y() - stroke.first().y()); break;
                        default: break;
                    }
                    break;
                } case ERASER: {
                    if (stroke.count() < 1) break;
                    QImage tempImg = img.copy();
                    tempImg.fill(Qt::transparent);
                    QPainter p(&tempImg);
                    p.setPen(eraserTool);
                    if (stroke.count() == 1) p.drawPoint(stroke.first());
                    else if (stroke.count() > 1) p.drawPolyline(stroke);
                    layerPainter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
                    layerPainter.drawImage(QPoint(0,0), tempImg);
                    layerPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
                    break;
                } case SELECT: {
                    selectTool->paint(event, &layerPainter);
                    break;
                } default : break;
            }
        }

        globalPainter.setOpacity(timeline()->getLayerWidgetAt(i)->getLayerTitle()->getOpacity());
        globalPainter.drawImage(event->rect(), img, event->rect());
        globalPainter.setOpacity(1.0);

        if (currentTool == ERASER && scribbling)
        {
            globalPainter.setPen(QPen(Qt::red, 2));
            globalPainter.drawEllipse(stroke.last().x() -eraserTool.width()/2 , stroke.last().y() - eraserTool.width()/2, eraserTool.width(), eraserTool.width());
        }
    });
}

void Editor::drawPenStroke()
{
    QImage i = animation()->copyImageAt(timeline()->getLayer(), getPos());
    QImage j = i.copy();
    QPainter painter(&j);
    painter.setPen(penTool);
    if (stroke.count() == 1) painter.drawPoint(stroke.first());
    else if (stroke.count() > 1) painter.drawPolyline(stroke);
    undostack()->push(new ModifyImageCommand(i, j, timeline()->getLayer(), timeline()->getPos(), animation()));
}

void Editor::drawShape()
{
    QImage i = animation()->copyImageAt(timeline()->getLayer(), getPos());
    QImage j = i.copy();
    QPainter painter(&j);
    painter.setPen(shapeTool);
    switch(shapeSubtool){
        case LINE: painter.drawPolyline(QPolygon() << stroke.first() << stroke.last()); break;
        case RECTANGLE: painter.drawRect(stroke.first().x(),stroke.first().y(),stroke.last().x() - stroke.first().x(),stroke.last().y() - stroke.first().y()); break;
        case ELLIPSE: painter.drawEllipse(stroke.first().x(),stroke.first().y(),stroke.last().x() - stroke.first().x(),stroke.last().y() - stroke.first().y()); break;
        default: break;
    }
    undostack()->push(new ModifyImageCommand(i, j, timeline()->getLayer(), timeline()->getPos(), animation()));
}

void Editor::drawFill()
{
    QImage i = animation()->copyImageAt(timeline()->getLayer(), getPos());
    QImage j = i.copy();
    QPainter painter(&j);
    painter.setPen(Qt::transparent);
    painter.setBrush(filltool);
    switch(fillSubtool){
        case LASSO: painter.drawPolygon(stroke); break;
        case RECTANGLE: painter.drawRect(stroke.first().x(),stroke.first().y(),stroke.last().x() - stroke.first().x(),stroke.last().y() - stroke.first().y()); break;
        case ELLIPSE: painter.drawEllipse(stroke.first().x(),stroke.first().y(),stroke.last().x() - stroke.first().x(),stroke.last().y() - stroke.first().y()); break;
        default: break;
    }
    undostack()->push(new ModifyImageCommand(i, j, timeline()->getLayer(), timeline()->getPos(), animation()));
}

void Editor::drawSelect()
{
    if (scribbling || !animation()->isKey(timeline()->getLayer(), getPos())) return;
    selectTool->draw();
    selectTool->reset();
}

void Editor::drawEraserStroke()
{
    QImage i = animation()->copyImageAt(timeline()->getLayer(), getPos());
    QImage j = i.copy();
    QImage k = i.copy();
    k.fill(Qt::transparent);
    QPainter painter(&k);
    painter.setPen(eraserTool);
    if (stroke.count() == 1) painter.drawPoint(stroke.first());
    else if (stroke.count() > 1) painter.drawPolyline(stroke);
    QPainter painter2(&j);
    painter2.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    painter2.drawImage(QPoint(0,0), k);
    undostack()->push(new ModifyImageCommand(i, j, timeline()->getLayer(), timeline()->getPos(), animation()));
}

void Editor::knockback()
{
    if (scribbling || !animation()->isKey(timeline()->getLayer(), getPos())) return;
    QImage i = animation()->copyImageAt(timeline()->getLayer(), getPos());
    QImage j = i.copy();

    for (int y = 0; y < j.height(); y++) {
        QRgb* rgb = (QRgb*)j.scanLine(y);
        for (int x = 0; x < j.width(); x++) {
            rgb[x] = qRgba(qRed(rgb[x]), qGreen(rgb[x]), qBlue(rgb[x]), qAlpha(rgb[x]) > knockbackAmount ? qAlpha(rgb[x]) - knockbackAmount : 0 );
        }
    }
    undostack()->push(new ModifyImageCommand(i, j, timeline()->getLayer(), timeline()->getPos(), animation()));
}

void Editor::clearImage()
{
    if (scribbling || !animation()->isKey(timeline()->getLayer(), getPos())) return;
    QImage i = animation()->copyImageAt(timeline()->getLayer(), getPos());
    QImage j = i.copy();
    j.fill(Qt::transparent);
    undostack()->push(new ModifyImageCommand(i, j, timeline()->getLayer(), timeline()->getPos(), animation()));
}

int Editor::getPos(int layer)
{
    if (layer == -1) layer = timeline()->getLayer();
    return animation()->isKey(layer, timeline()->getPos()) ?
    timeline()->getPos() :
    animation()->getPrevKey(layer, timeline()->getPos());
}

void Editor::drawOnionSkin(QPaintEvent* event, QPainter* painter, QPainterPath* path, double opacity, int layer, int pos, QColor color)
{
    painter->setOpacity(opacity);
    QImage img = animation()->copyImageAt(layer, pos);
    QPainter p(&img);
    p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    p.fillPath(*path, color);
    painter->drawImage(event->rect(), img, event->rect());
}
