#include <QtWidgets>
#include "timeline.h"
#include "editor.h"
#include "object.h"

Timeline::Timeline(Object* o, QUndoStack* u, QWidget* parent): QWidget(parent)
{
    undoStack = u;
    object = o;
    update();
}

void Timeline::mousePressEvent(QMouseEvent*){}
void Timeline::mouseReleaseEvent(QMouseEvent*){}
void Timeline::mouseMoveEvent(QMouseEvent*){}
void Timeline::resizeEvent(QResizeEvent *){}

void Timeline::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    for (int i = 0; i < 32; ++i) {
        QPainterPath path;

        path.addRect(i* 20, 0, 15, (height()-1)/2);
        QPen pen(Qt::black);
        painter.setPen(pen);
        painter.fillPath(path, object->isKeyframe(i) ? Qt::black : Qt::white );
        painter.drawPath(path);

        if (i == this->getPos())
        {
            path.addRect(i* 20, 0, 15, (height()-1)/2);
            QPen pen(Qt::red);
            pen.setWidth(4);
            painter.setPen(pen);
            painter.drawPath(path);
        }
    }
}

void Timeline::gotoNextFrame()
{
    if (editor->isScribbling()) return;

    this->setPos(this->getPos()+1);
    if (object->isKeyframe(this->getPos())) editor->setPos(this->getPos());
    else editor->setPos(object->getPrevKeyframePos(this->getPos()));

    this->update();
    editor->update();
}

void Timeline::gotoPrevFrame()
{
    if (editor->isScribbling()) return;
    if (this->getPos() == 0) return;

    this->setPos(this->getPos()-1);
    if (object->isKeyframe(this->getPos())) editor->setPos(this->getPos());
    else editor->setPos(object->getPrevKeyframePos(this->getPos()));

    this->update();
    editor->update();
}

void Timeline::gotoFrame(int pos)
{
    if (editor->isScribbling()) return;
    this->setPos(pos);
    update();
    editor->update();
}

void Timeline::addKeyframe()
{
    if (editor->isScribbling()) return;
    if (object->isKeyframe(this->getPos())) return;
    object->addKeyframeAt(this->getPos());
    object->resizeImage(this->getPos(), editor->width(), editor->height());
    editor->setPos(this->getPos());
    update();
    editor->update();
}

void Timeline::removeKeyframe()
{
    if (editor->isScribbling()) return;
    if (!object->isKeyframe(this->getPos())) return;
    object->removeKeyframeAt(this->getPos());
    if (this->getPos() == 0)
    {
        object->addKeyframeAt(0, QImage(editor->width(), editor->height(), QImage::Format_ARGB32));
    }
    editor->setPos(object->getPrevKeyframePos(this->getPos()));
    update();
    editor->update();
}

void Timeline::insertFrame()
{
    if (editor->isScribbling()) return;
    if (this->getPos() >= object->getLastKeyframePos()) return;
    object->foreachKeyframeRevert([this](int i){
            QImage img = object->getKeyframeImageAt(i)->copy();
            object->addKeyframeAt(i + 1, img);
            object->removeKeyframeAt(i);
    }, object->isKeyframe(this->getPos()) ? this->getPos() + 1 : this->getPos());
    if (!object->isKeyframe(this->getPos())) editor->setPos(object->getPrevKeyframePos(this->getPos()));
    update();
    editor->update();
}

void Timeline::removeFrame()
{
    if (editor->isScribbling()) return;
    if (this->getPos() >= object->getLastKeyframePos()) return;
    if (object->isKeyframe(this->getPos() + 1)) return;
    object->foreachKeyframe([this](int i){
            QImage img = object->getKeyframeImageAt(i)->copy();
            object->removeKeyframeAt(i);
            object->addKeyframeAt(i - 1, img);
    }, object->isKeyframe(this->getPos()) ? this->getPos() + 1 : this->getPos());
    if (!object->isKeyframe(this->getPos())) editor->setPos(object->getPrevKeyframePos(this->getPos()));
    update();
    editor->update();
}

void Timeline::copyFrame()
{
    if (!object->isKeyframe(this->getPos())) return;
    clipboard = object->getKeyframeImageAt(this->getPos())->copy();
}

void Timeline::cutFrame()
{
    if (!object->isKeyframe(this->getPos())) return;
    clipboard = object->getKeyframeImageAt(this->getPos())->copy();
    this->removeKeyframe();
}

void Timeline::pasteFrame()
{
    if (clipboard.width() <= 1 && clipboard.height() <= 1 ) return;
    object->addKeyframeAt(this->getPos(), clipboard.copy());
    editor->setPos(this->getPos());
    update();
    editor->update();
}

