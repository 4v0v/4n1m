#include <QtWidgets>
#include "timeline.h"
#include "editor.h"
#include "object.h"
#include "commands.h"

Timeline::Timeline(Object* o, QUndoStack* u, QWidget* parent): QWidget(parent)
{
    undoStack = u;
    object = o;
    this->update();
}

void Timeline::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    QPainterPath path;

    path.addRect(0, 0, this->width(), this->height());
    painter.fillPath(path, Qt::gray);
    painter.drawPath(path);

    for (int j = 0; j < object->getLastLayerPos()+1; ++j) {
        for (int i = 0; i < 32; ++i) {
            QPainterPath path;
            path.addRect(i* 20, j * ((height()-1)/2), 15, (height()-1)/2);
            painter.setPen(QPen(Qt::black));
            painter.fillPath(path, object->isKeyframe(j, i) ? Qt::black : Qt::white );
            painter.drawPath(path);

            if (i == this->getPos() && j == this->getLayer())
            {
                path.addRect(i* 20, j * ((height()-1)/2), 15, (height()-1)/2);
                painter.setPen(QPen(Qt::red, 4));
                painter.drawPath(path);
            }
        }
    }
}

void Timeline::gotoNextFrame()
{
    if (editor->isScribbling()) return;
    this->setPos(this->getPos()+1);
    this->update();
    editor->update();
}

void Timeline::gotoPrevFrame()
{
    if (editor->isScribbling() || this->getPos() == 0) return;
    this->setPos(this->getPos()-1);
    this->update();
    editor->update();
}

void Timeline::gotoNextLayer()
{
    if (editor->isScribbling() || this->getLayer() == object->getLastLayerPos()) return;
    this->setLayer(this->getLayer()+1);
    this->update();
    editor->update();
}

void Timeline::gotoPrevLayer()
{
    if (editor->isScribbling() || this->getLayer() == 0) return;
    this->setLayer(this->getLayer()-1);
    this->update();
    editor->update();
}

void Timeline::gotoFrame(int layer, int pos)
{
    if (editor->isScribbling()) return;
    this->setLayer(layer);
    this->setPos(pos);
    this->update();
    editor->update();
}

void Timeline::addKeyframe()
{
    if (editor->isScribbling() || object->isKeyframe(this->getLayer(), this->getPos())) return;
    undoStack->push(new AddImageCommand(QImage(editor->width(), editor->height(), QImage::Format_ARGB32), this->getLayer(), this->getPos(), this->object));
}

void Timeline::removeKeyframe()
{
    if (editor->isScribbling() || !object->isKeyframe(this->getLayer(), this->getPos())) return;
    if (this->getPos() != 0)
    {
        QImage i = object->getKeyframeImageAt(this->getLayer(), this->getPos())->copy();
        undoStack->push(new RemoveImageCommand(i, this->getLayer(), this->getPos(), this->object));
    } else {
        object->removeKeyframeAt(this->getLayer(), this->getPos());
        object->addKeyframeAt(this->getLayer(), 0, QImage(editor->width(), editor->height(), QImage::Format_ARGB32));
    }
    this->update();
    editor->update();
}

void Timeline::insertFrame()
{
    if (editor->isScribbling()) return;
    if (this->getPos() >= object->getLastKeyframePos(this->getLayer())) return;
    object->foreachKeyframeRevert(this->getLayer(), [this](int i){
            QImage img = object->getKeyframeImageAt(this->getLayer(), i)->copy();
            object->addKeyframeAt(this->getLayer(), i + 1, img);
            object->removeKeyframeAt(this->getLayer(), i);
    }, object->isKeyframe(this->getLayer(), this->getPos()) ? this->getPos() + 1 : this->getPos());
    this->update();
    editor->update();
}

void Timeline::removeFrame()
{
    if (editor->isScribbling()) return;
    if (this->getPos() >= object->getLastKeyframePos(this->getLayer())) return;
    if (object->isKeyframe(this->getLayer(), this->getPos() + 1)) return;
    object->foreachKeyframe(this->getLayer(), [this](int i){
            QImage img = object->getKeyframeImageAt(this->getLayer(), i)->copy();
            object->removeKeyframeAt(this->getLayer(), i);
            object->addKeyframeAt(this->getLayer(), i - 1, img);
    }, object->isKeyframe(this->getLayer(), this->getPos()) ? this->getPos() + 1 : this->getPos());
    this->update();
    editor->update();
}

void Timeline::copyFrame()
{
    if (!object->isKeyframe(this->getLayer(), this->getPos())) return;
    clipboard = object->getKeyframeImageAt(this->getLayer(), this->getPos())->copy();
}

void Timeline::cutFrame()
{
    this->copyFrame();
    this->removeKeyframe();
}

void Timeline::pasteFrame()
{
    if (clipboard.width() <= 1 && clipboard.height() <= 1 ) return;
   
    if (object->isKeyframe(this->getLayer(), this->getPos()))
    {
        QImage i = object->getKeyframeImageAt(this->getLayer(), this->getPos())->copy();
        QImage j = i.copy();
        QPainter p(&j);
        p.drawImage(QPoint(0,0), clipboard.copy());
        undoStack->push(new ModifyImageCommand(i, j, this->getLayer(), this->getPos(), this->object));
    }
    else undoStack->push(new AddImageCommand(clipboard.copy(), this->getLayer(), this->getPos(), this->object));
}

void Timeline::mousePressEvent(QMouseEvent *event)
{
    p = event->pos();
    isDown = true;
}

void Timeline::mouseReleaseEvent(QMouseEvent*)
{
    isDown = false;
}

void Timeline::mouseMoveEvent(QMouseEvent *event)
{
    if(isDown)
    {
        QPoint diff= event->pos() - p;
        window()->move(window()->pos()+diff);
    }
}
