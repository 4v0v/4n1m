#include "animation.h"
#include "preview.h"
#include "commands.h"
#include "mainWidgets/editor.h"
#include "mainWidgets/timeline.h"
#include "mainWidgets/titlebar.h"
#include "mainWidgets/menubar.h"

Timeline::Timeline(MainWindow* mainwindow): QWidget(mainwindow)
{
    parent = mainwindow;
}

void Timeline::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    QPainterPath path;

    path.addRect(0, 0, width(), height());
    painter.fillPath(path, Qt::gray);
    painter.drawPath(path);

    for (int j = 0; j < animation()->getLastLayerPos()+1; ++j) {
        for (int i = 0; i < 71; ++i) {
            QPainterPath path;
            path.addRect(i* 12, j * ((height()-1)/3), 10, (height()-1)/3);
            painter.setPen(QPen(Qt::black));
            painter.fillPath(path, animation()->isKey(j, i) ? Qt::black : Qt::white );
            painter.drawPath(path);

            if (i == getPos() && j == getLayer())
            {
                path.addRect(i* 12, j * ((height()-1)/3), 10, (height()-1)/3);
                painter.setPen(QPen(Qt::red, 2));
                painter.drawPath(path);
            }
        }
    }
}

void Timeline::gotoNextFrame()
{
    if (editor()->isScribbling()) return;
    setPos(getPos()+1);
    update();
    editor()->update();
}

void Timeline::gotoPrevFrame()
{
    if (editor()->isScribbling() || getPos() == 0) return;
    setPos(getPos()-1);
    update();
    editor()->update();
}

void Timeline::gotoNextLayer()
{
    if (editor()->isScribbling() || getLayer() == animation()->getLastLayerPos()) return;
    setLayer(getLayer()+1);
    update();
    editor()->update();
}

void Timeline::gotoPrevLayer()
{
    if (editor()->isScribbling() || getLayer() == 0) return;
    setLayer(getLayer()-1);
    update();
    editor()->update();
}

void Timeline::gotoFrame(int layer, int pos)
{
    if (editor()->isScribbling()) return;
    setLayer(layer);
    setPos(pos);
    update();
    editor()->update();
}

void Timeline::addKey()
{
    if (editor()->isScribbling() || animation()->isKey(getLayer(), getPos())) return;
    undostack()->push(new AddImageCommand(QImage(editor()->width(), editor()->height(), QImage::Format_ARGB32), getLayer(), getPos(), animation()));
}

void Timeline::removeKey()
{
    if (editor()->isScribbling() || !animation()->isKey(getLayer(), getPos())) return;
    QImage i = animation()->copyImageAt(getLayer(), getPos());
    undostack()->push(new RemoveImageCommand(i, getLayer(), getPos(), animation()));
}

void Timeline::insertFrame()
{
    if (
        editor()->isScribbling() ||
        animation()->getKeyCount(getLayer()) == 0 ||
        getPos() >= animation()->getLastKey(getLayer())
    ) return;
    undostack()->push(new InsertFrameCommand(getLayer(), getPos(), animation()));
}

void Timeline::removeFrame()
{
    if (
        editor()->isScribbling() ||
        animation()->getKeyCount(getLayer()) == 0 ||
        getPos() >= animation()->getLastKey(getLayer()) ||
        animation()->isKey(getLayer(), getPos() + 1)
    ) return;
    undostack()->push(new RemoveFrameCommand(getLayer(), getPos(), animation()));
}

void Timeline::copyFrame()
{
    if (!animation()->isKey(getLayer(), getPos())) return;
    clipboard = animation()->copyImageAt(getLayer(), getPos());
}

void Timeline::cutFrame()
{
    if (editor()->isScribbling() || !animation()->isKey(getLayer(), getPos())) return;
    copyFrame();
    removeKey();
}

void Timeline::pasteFrame()
{
    if (clipboard.width() <= 1 && clipboard.height() <= 1 ) return;
   
    if (animation()->isKey(getLayer(), getPos()))
    {
        QImage i = animation()->copyImageAt(getLayer(), getPos());
        QImage j = i.copy();
        QPainter p(&j);
        p.drawImage(QPoint(0,0), clipboard.copy());
        undostack()->push(new ModifyImageCommand(i, j, getLayer(), getPos(), animation()));
    }
    else undostack()->push(new AddImageCommand(clipboard.copy(), getLayer(), getPos(), animation()));
}
