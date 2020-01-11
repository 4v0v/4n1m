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
    internetExplorerChanImg = QImage(width(), height(), QImage::Format_ARGB32);
}

void Editor::mousePressEvent(QMouseEvent *event)
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

    if ( animation()->isKey(timeline()->getLayer(), timeline()->getPos()) &&
         currentTool == SELECT
    ){
        switch (selectState) {
            case STATE_EMPTY:
                selectState = STATE_SELECTING;
                select.setRect(event->x(), event->y(), 1, 1);
                pselect.clear();
                pselect << QPoint(event->x(), event->y());
                break;
            case STATE_SELECTED:
                if (!select.contains(event->pos())) // if click is !inside selected zone
                {
                    if (selectMode != EMPTY_MODE) drawSelect();
                    selectState = STATE_SELECTING;
                    select.setRect(event->x(), event->y(), 1, 1);
                    pselect.clear();
                    pselect << QPoint(event->x(), event->y());
                }
                else // if click is inside selected zone
                {
                    SelectionState prevMode = selectMode;
                    SelectionState nextMode;

                    if( event->modifiers().testFlag(Qt::ControlModifier) && !event->modifiers().testFlag(Qt::ShiftModifier)) nextMode = COPY_MODE;
                    else if( event->modifiers().testFlag(Qt::ShiftModifier) && !event->modifiers().testFlag(Qt::ControlModifier)) nextMode = IECHAN_MODE;
                    else nextMode = CUT_MODE;

                    if (prevMode == COPY_MODE)
                    {
                        QRect tempSelect = QRect(select.x(), select.y(), select.width(), select.height());
                        QImage tempImg = selectedImg.copy();
                        QPolygon tempPolygon(pselect);

                        drawSelect();
                        select.setRect(tempSelect.x(), tempSelect.y(), tempSelect.width(), tempSelect.height());
                        dselect.setRect(tempSelect.x(), tempSelect.y(), tempSelect.width(), tempSelect.height());
                        pselect.putPoints(0, tempPolygon.size(), tempPolygon);
                        selectedImg = tempImg;
                        selectState = STATE_SELECTED;
                    }
                    else if (prevMode == IECHAN_MODE && prevMode != nextMode)
                    {
                        QRect tempSelect = QRect(select.x(), select.y(), select.width(), select.height());
                        QImage tempImg = selectedImg.copy();
                        QPolygon tempPolygon(pselect);
                        drawSelect();
                        select.setRect(tempSelect.x(), tempSelect.y(), tempSelect.width(), tempSelect.height());
                        dselect.setRect(tempSelect.x(), tempSelect.y(), tempSelect.width(), tempSelect.height());
                        pselect.putPoints(0, tempPolygon.size(), tempPolygon);
                        selectedImg = tempImg;
                        selectState = STATE_SELECTED;
                    }
                    else if (prevMode == CUT_MODE && prevMode != nextMode)
                    {
                        QRect tempSelect = QRect(select.x(), select.y(), select.width(), select.height());
                        QImage tempImg = selectedImg.copy();
                        QPolygon tempPolygon(pselect);
                        drawSelect();
                        select.setRect(tempSelect.x(), tempSelect.y(), tempSelect.width(), tempSelect.height());
                        dselect.setRect(tempSelect.x(), tempSelect.y(), tempSelect.width(), tempSelect.height());
                        pselect.putPoints(0, tempPolygon.size(), tempPolygon);
                        selectedImg = tempImg;
                        selectState = STATE_SELECTED;
                    }

                    selectMode = nextMode;
                    dxselect = event->x() - select.x();
                    dyselect = event->y() - select.y();
                }
                break;
            default: break;
        }
    }
    update();
}

void Editor::mouseReleaseEvent(QMouseEvent*)
{
    scribbling = false;
    switch (currentTool)
    {
        case PEN: drawPenStroke(); break;
        case FILL: drawFill(); break;
        case SHAPE: drawShape(); break;
        case ERASER: if (animation()->isKey(timeline()->getLayer(), timeline()->getPos())) drawEraserStroke(); break;
        case SELECT:
            switch (selectState)
            {
                case STATE_SELECTING:
                    bool canSelect;

                    if (selectSubtool == RECTANGLE) canSelect = abs(select.width())>5 && abs(select.height())>5;
                    else canSelect = abs(pselect.boundingRect().width())>5 && abs(pselect.boundingRect().height())>5;

                    if ( canSelect ) { //TODO a la place, regarder si il y au - 1 pixel à l'intérieur de la sélection
                        // retourner le rect si il est négatif
                        if (selectSubtool == LASSO) select = pselect.boundingRect();
                        if (select.width() < 0) { int tempw = select.width(); int tempx = select.x(); select.setX(tempx + tempw); select.setWidth(abs(tempw)); }
                        if (select.height() < 0) { int temph = select.height(); int tempy = select.y(); select.setY(tempy + temph); select.setHeight(abs(temph));}
                        dselect.setRect(select.x(), select.y(), select.width(), select.height());

                        QImage simg;
                        if (selectSubtool == RECTANGLE) simg = animation()->getImageAt(timeline()->getLayer(), timeline()->getPos())->copy(select);
                        else
                        {
                            simg = animation()->getImageAt(timeline()->getLayer(), timeline()->getPos())->copy(select);
                            QImage lassoImg = QImage(select.width(), select.height(), QImage::Format_ARGB32);
                            lassoImg.fill(Qt::transparent);
                            QPolygon tempPoly(pselect);
//                            tempPoly.putPoints(0, pselect.size(), pselect);
                            tempPoly.translate(-select.x(), -select.y());
                            QPainter p(&lassoImg);
                            p.setBrush(Qt::black);
                            p.drawPolygon(tempPoly);

                            QPainter p2(&simg);
                            p2.setCompositionMode( QPainter::CompositionMode_SourceIn);
                            p2.drawImage(QPoint(0,0), lassoImg);
                        }

                        selectedImg = simg;
                        selectState = STATE_SELECTED;
                    }
                    else {
                        select.setRect(0,0,0,0);
                        dselect.setRect(0,0,0,0);
                        pselect.clear();
                        selectedImg =  QImage(1, 1, QImage::Format_ARGB32);
                        internetExplorerChanImg =  QImage(width(), height(), QImage::Format_ARGB32);
                        selectState = STATE_EMPTY;
                        selectMode = EMPTY_MODE;
                    }
                    break;
                default: break;
            }
        default: break;
    }
    stroke.clear();
    update();
}

void Editor::mouseMoveEvent(QMouseEvent *event)
{
    if (!scribbling) return;
    if (currentTool == SELECT){
        int tempx = event->x(); if (tempx > width()) tempx = width(); if (tempx < 0) tempx = 0;
        int tempy = event->y(); if (tempy > height()) tempy = height(); if (tempy < 0) tempy = 0;
        if (selectState == STATE_SELECTING)
        {
            select.setWidth(tempx - select.x()); select.setHeight(tempy - select.y());
            pselect << QPoint(event->x(), event->y());
        }
        else if (selectState == STATE_SELECTED)
        {
            select.moveTo(event->x() - dxselect, event->y() - dyselect);
            pselect.translate(event->x() - pselect.boundingRect().x() - dxselect, event->y() - pselect.boundingRect().y() - dyselect);
            if (selectMode == IECHAN_MODE)
            {
                QPainter painter(&internetExplorerChanImg);
                painter.drawImage(QPoint(select.x(), select.y()), selectedImg);
            }
        }
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
                    if (selectState == STATE_SELECTING)
                    {
                        layerPainter.setPen(QPen(Qt::black, 1, Qt::DashLine));
                        if (selectSubtool == RECTANGLE) layerPainter.drawRect(select);
                        if (selectSubtool == LASSO) layerPainter.drawPolygon(pselect);
                    }
                    else if (selectState == STATE_SELECTED)
                    {
                        if (selectMode == CUT_MODE)
                        {
                            layerPainter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
                            layerPainter.drawImage(QPoint(dselect.x(),dselect.y()), selectedImg);
                            layerPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
                        }
                        if (selectMode == IECHAN_MODE) {
                            layerPainter.drawImage(QPoint(0, 0), internetExplorerChanImg);
                        }
                        layerPainter.drawImage(QPoint(select.x(), select.y()), selectedImg);
                        layerPainter.setPen(QPen(Qt::blue, 1, Qt::DashLine));
                        layerPainter.drawRect(select);
                        if (selectSubtool == LASSO)
                        {
                            layerPainter.setPen(QPen(Qt::red, 1, Qt::DashLine));
                            layerPainter.drawPolygon(pselect);
                        }
                    }
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
    if (selectState != STATE_SELECTED) return;
    QImage i = animation()->copyImageAt(timeline()->getLayer(), getPos());
    QImage j = i.copy();
    QPainter painter(&j);

    if (selectMode == CUT_MODE)
    {
        painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        painter.drawImage(QPoint(dselect.x(),dselect.y()), selectedImg);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    }
    else if (selectMode == IECHAN_MODE)
    {
        painter.drawImage(QPoint(0, 0), internetExplorerChanImg);
    }
    painter.drawImage(QPoint(select.x(), select.y()), selectedImg);
    undostack()->push(new ModifyImageCommand(i, j, timeline()->getLayer(), timeline()->getPos(), animation()));

    select.setRect(0,0,0,0);
    dselect.setRect(0,0,0,0);
    pselect.clear();
    selectedImg =  QImage(1, 1, QImage::Format_ARGB32);
    internetExplorerChanImg =  QImage(width(), height(), QImage::Format_ARGB32);
    selectState = STATE_EMPTY;
    selectMode = EMPTY_MODE;
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
