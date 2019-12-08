#include <QtWidgets>
#include "timeline.h"
#include "editor.h"
#include "object.h"

Timeline::Timeline(Object *o, QWidget *parent): QWidget(parent)
{
    setObject(o);
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

        path.addRect(i* 20, 0, 15, height() - 1);
        QPen pen(Qt::black);
        painter.setPen(pen);
        painter.fillPath(path, object->isKeyframe(i) ? Qt::black : Qt::white );
        painter.drawPath(path);

        if (i == timelinePos)
        {
            path.addRect(i* 20, 0, 15, height() - 1);
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
    timelinePos += 1;
    if (object->isKeyframe(timelinePos)) object->setPos(timelinePos);
    update();
    editor->update();
}

void Timeline::gotoPrevFrame()
{
    if (editor->isScribbling()) return;
    if (timelinePos == 0) return;
    timelinePos -= 1;
    if (object->isKeyframe(timelinePos)) object->setPos(timelinePos);
    else object->setPos(object->getPrevKeyframePos(timelinePos));
    update();
    editor->update();
}

void Timeline::gotoFrame(int pos)
{
    if (editor->isScribbling()) return;
    timelinePos = pos;
    update();
    editor->update();
}

void Timeline::addKeyframe()
{
    if (editor->isScribbling()) return;
    if (object->isKeyframe(timelinePos)) return;
    object->addKeyframeAt(timelinePos);
    object->resizeImage(timelinePos, editor->width(), editor->height());
    object->setPos(timelinePos);
    update();
    editor->update();
}

void Timeline::removeKeyframe()
{
    if (editor->isScribbling()) return;
    if (!object->isKeyframe(timelinePos)) return;
    object->removeKeyframeAt(timelinePos);
    if (timelinePos == 0)
    {
        object->addKeyframeAt(0);
        object->resizeImage(0, editor->width(), editor->height());
    }
    object->setPos(object->getPrevKeyframePos(timelinePos));
    update();
    editor->update();
}

void Timeline::insertFrame()
{
    if (editor->isScribbling()) return;
    if (timelinePos >= object->getLastKeyframePos()) return;
    object->foreachKeyframeRevert([this](int i){
            QImage img = object->getKeyframeImageAt(i)->copy();
            object->addKeyframeAt(i + 1, img);
            object->removeKeyframeAt(i);
    }, object->isKeyframe(timelinePos) ? timelinePos + 1 : timelinePos);
    if (!object->isKeyframe(timelinePos)) object->setPos(object->getPrevKeyframePos(timelinePos));
    update();
    editor->update();
}

void Timeline::removeFrame()
{
    if (editor->isScribbling()) return;
    if (timelinePos >= object->getLastKeyframePos()) return;
    if (object->isKeyframe(timelinePos + 1)) return;
    object->foreachKeyframe([this](int i){
            QImage img = object->getKeyframeImageAt(i)->copy();
            object->removeKeyframeAt(i);
            object->addKeyframeAt(i - 1, img);
    }, object->isKeyframe(timelinePos) ? timelinePos + 1 : timelinePos);
    if (!object->isKeyframe(timelinePos)) object->setPos(object->getPrevKeyframePos(timelinePos));
    update();
    editor->update();
}

void Timeline::copyFrame()
{
    if (!object->isKeyframe(timelinePos)) return;
    clipboard = object->getKeyframeImageAt(timelinePos)->copy();
}

void Timeline::cutFrame()
{
    if (!object->isKeyframe(timelinePos)) return;
    clipboard = object->getKeyframeImageAt(timelinePos)->copy();
    removeFrame();
}

void Timeline::pasteFrame()
{
    if (clipboard.width() <= 1 && clipboard.height() <= 1 ) return;
    object->addKeyframeAt(timelinePos, clipboard.copy());
    object->setPos(timelinePos);
    update();
    editor->update();
}

