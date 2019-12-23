#include "object.h"
#include "editor.h"
#include "timeline.h"
#include "preview.h"
#include "commands.h"
#include "titlebar.h"
#include "menubar.h"

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

    for (int j = 0; j < object()->getLastLayerPos()+1; ++j) {
        for (int i = 0; i < 71; ++i) {
            QPainterPath path;
            path.addRect(i* 12, j * ((height()-1)/3), 10, (height()-1)/3);
            painter.setPen(QPen(Qt::black));
            painter.fillPath(path, object()->isKeyframe(j, i) ? Qt::black : Qt::white );
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
    if (editor()->isScribbling() || getLayer() == object()->getLastLayerPos()) return;
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

void Timeline::addKeyframe()
{
    if (editor()->isScribbling() || object()->isKeyframe(getLayer(), getPos())) return;
    undostack()->push(new AddImageCommand(QImage(editor()->width(), editor()->height(), QImage::Format_ARGB32), getLayer(), getPos(), object()));
}

void Timeline::removeKeyframe()
{
    if (editor()->isScribbling() || !object()->isKeyframe(getLayer(), getPos())) return;

    QImage i = object()->getKeyframeImageAt(getLayer(), getPos())->copy();
    undostack()->push(new RemoveImageCommand(i, getLayer(), getPos(), object()));

    update();
    editor()->update();
}

void Timeline::insertFrame()
{
    if (
        editor()->isScribbling() ||
        object()->getKeyframesCount(getLayer()) == 0 ||
        getPos() >= object()->getLastKeyframePos(getLayer())
    ) return;

    object()->foreachKeyframeRevert(getLayer(), [this](int i){
            QImage img = object()->getKeyframeImageAt(getLayer(), i)->copy();
            object()->addKeyframeAt(getLayer(), i + 1, img);
            object()->removeKeyframeAt(getLayer(), i);
    }, object()->isKeyframe(getLayer(), getPos()) ? getPos() + 1 : getPos());
    update();
    editor()->update();
}

void Timeline::removeFrame()
{
    if (
        editor()->isScribbling() ||
        object()->getKeyframesCount(getLayer()) == 0 ||
        getPos() >= object()->getLastKeyframePos(getLayer()) ||
        object()->isKeyframe(getLayer(), getPos() + 1)
    ) return;

    object()->foreachKeyframe(getLayer(), [this](int i){
            QImage img = object()->getKeyframeImageAt(getLayer(), i)->copy();
            object()->removeKeyframeAt(getLayer(), i);
            object()->addKeyframeAt(getLayer(), i - 1, img);
    }, object()->isKeyframe(getLayer(), getPos()) ? getPos() + 1 : getPos());
    update();
    editor()->update();
}

void Timeline::copyFrame()
{
    if (!object()->isKeyframe(getLayer(), getPos())) return;
    clipboard = object()->getKeyframeImageAt(getLayer(), getPos())->copy();
}

void Timeline::cutFrame()
{
    copyFrame();
    removeKeyframe();
}

void Timeline::pasteFrame()
{
    if (clipboard.width() <= 1 && clipboard.height() <= 1 ) return;
   
    if (object()->isKeyframe(getLayer(), getPos()))
    {
        QImage i = object()->getKeyframeImageAt(getLayer(), getPos())->copy();
        QImage j = i.copy();
        QPainter p(&j);
        p.drawImage(QPoint(0,0), clipboard.copy());
        undostack()->push(new ModifyImageCommand(i, j, getLayer(), getPos(), object()));
    }
    else undostack()->push(new AddImageCommand(clipboard.copy(), getLayer(), getPos(), object()));
}
