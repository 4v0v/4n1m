#include "animation.h"
#include "preview.h"
#include "commands.h"
#include "mainWidgets/editor.h"
#include "mainWidgets/timeline.h"
#include "mainWidgets/titlebar.h"
#include "mainWidgets/toolbar.h"

Editor::Editor(MainWindow* mw): QWidget(mw)
{
    mainwindow = mw;
    setCursor(Qt::CrossCursor);

    setGeometry(0, 0, mainwindow->getWindowDimensions().width(), 530);
}

void Editor::mousePressEvent(QMouseEvent *event)
{
    mainwindow->subtoolbar->hideProperties();
    if ( !animation()->isKey(timeline()->getLayer(), timeline()->getPos()) && currentTool != Tool::EMPTY && currentTool != Tool::ERASER && currentTool != Tool::SELECT) 
        timeline()->addKey();

    scribbling = true;
    stroke << QPoint(event->pos().x(), event->pos().y());

    if ( currentTool == Tool::SELECT)
    {
        if (selectState == STATE_SELECTED && !select.contains(event->pos())){drawSelect();}
        if (selectState == STATE_SELECTED) dx = event->x() - select.x(); dy = event->y() - select.y();
        if (selectState != STATE_SELECTED && animation()->isKey(timeline()->getLayer(), timeline()->getPos())){
            selectState = STATE_SELECTING;
            select.setX(event->x());
            select.setY(event->y());
            select.setWidth(1);
            select.setHeight(1);
        }
    }
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
        case Tool::ERASER: if (animation()->isKey(timeline()->getLayer(), timeline()->getPos())) drawEraserStroke(); break;
        case Tool::SELECT:
            if (selectState == STATE_SELECTING )
            {
                if ( abs(select.width())>2 && abs(select.width())>2) {
                    selectState = STATE_SELECTED;
                    if (select.width() < 0) {
                        int tempw = select.width();
                        int tempx = select.x();
                        select.setX(tempx + tempw); select.setWidth(abs(tempw));
                    }
                    if (select.height() < 0) {
                        int temph = select.height();
                        int tempy = select.y();
                        select.setY(tempy + temph); select.setHeight(abs(temph));
                    }
                    dselect.setRect(select.x(), select.y(), select.width(), select.height());
                    selectedImg = animation()->getImageAt(timeline()->getLayer(), timeline()->getPos())->copy(select);
                }
                else selectState = STATE_EMPTY;
            }
        case Tool::EMPTY: break;
        default: break;
    }
    stroke.clear();
    update();
}

void Editor::mouseMoveEvent(QMouseEvent *event)
{
    if (!scribbling) return;
    if (currentTool == Tool::SELECT){
        int tempx = event->x();
        if (tempx > width()) tempx = width();
        if (tempx < 0) tempx = 0;
        int tempy = event->y();
        if (tempy > height()) tempy = height();
        if (tempy < 0) tempy = 0;

        if (selectState == STATE_SELECTING) {select.setWidth(tempx - select.x()); select.setHeight(tempy - select.y());}
        else if (selectState == STATE_SELECTED) {select.moveTo(event->x() - dx, event->y() - dy);}
    }

    stroke << QPoint(event->pos().x(), event->pos().y());
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
                case Tool::PEN: {
                    layerPainter.setPen(penTool);
                    if (stroke.count() == 1) layerPainter.drawPoint(stroke.first());
                    else if (stroke.count() > 1) layerPainter.drawPolyline(stroke);
                    break;
                } case Tool::LINE: {
                    if (stroke.count() < 2) break;
                    layerPainter.setPen(lineTool);
                    layerPainter.drawPolyline(QPolygon() << stroke.first() << stroke.last());
                    break;
                } case Tool::LASSOFILL: {
                    if (stroke.count() < 2) break;
                    QPainterPath path;
                    path.addPolygon(stroke);
                    layerPainter.fillPath(path, lassoFilltool);
                    break;
                } case Tool::ERASER: {
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
                } case Tool::SELECT: {
                    if (selectState == STATE_SELECTING)
                    {
                        layerPainter.setPen(QPen(Qt::black, 1, Qt::DashLine));
                        layerPainter.drawRect(select);
                    }
                    else if (selectState == STATE_SELECTED)
                    {
                        layerPainter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
                        layerPainter.drawImage(QPoint(dselect.x(),dselect.y()), selectedImg);
                        layerPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
                        layerPainter.setPen(QPen(Qt::blue, 1, Qt::DashLine));
                        layerPainter.drawRect(select);
                        layerPainter.drawImage(QPoint(select.x(), select.y()), selectedImg);
                    }
                    break;
                } case Tool::EMPTY: {
                    break;
                }
            }
        }

        globalPainter.setOpacity(timeline()->getLayerWidgetAt(i)->getLayerTitle()->getOpacity());
        globalPainter.drawImage(event->rect(), img, event->rect());
        globalPainter.setOpacity(1.0);

        if (currentTool == Tool::ERASER && scribbling)
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

void Editor::drawLine()
{
    QImage i = animation()->copyImageAt(timeline()->getLayer(), getPos());
    QImage j = i.copy();
    QPainter painter(&j);
    painter.setPen(lineTool);
    painter.drawPolyline(QPolygon() << stroke.first() << stroke.last());
    undostack()->push(new ModifyImageCommand(i, j, timeline()->getLayer(), timeline()->getPos(), animation()));
}

void Editor::drawLassoFill()
{
    QImage i = animation()->copyImageAt(timeline()->getLayer(), getPos());
    QImage j = i.copy();
    QPainter painter(&j);
    QPainterPath path;
    path.addPolygon(stroke);
    painter.fillPath(path, lassoFilltool);
    undostack()->push(new ModifyImageCommand(i, j, timeline()->getLayer(), timeline()->getPos(), animation()));
}

void Editor::drawSelect()
{
    selectState = STATE_EMPTY;
    QImage i = animation()->copyImageAt(timeline()->getLayer(), getPos());
    QImage j = i.copy();
    QPainter painter(&j);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    painter.drawImage(QPoint(dselect.x(),dselect.y()), selectedImg);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(QPoint(select.x(), select.y()), selectedImg);
    undostack()->push(new ModifyImageCommand(i, j, timeline()->getLayer(), timeline()->getPos(), animation()));
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
    selectState = STATE_EMPTY;
    QImage i = animation()->copyImageAt(timeline()->getLayer(), getPos());
    QImage j =  i.copy();;
    QPainter painter(&j);
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
    selectState = STATE_EMPTY;
    QImage i = animation()->copyImageAt(timeline()->getLayer(), getPos());
    QImage j = i.copy();;
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
