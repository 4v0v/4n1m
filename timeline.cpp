#include <QtWidgets>
#include "timeline.h"
#include "editor.h"
#include "object.h"

Timeline::Timeline(Object *o, QWidget *parent): QWidget(parent)
{
    setObject(o);
    update();
}

void Timeline::mousePressEvent(QMouseEvent *event){}
void Timeline::mouseReleaseEvent(QMouseEvent *event){}
void Timeline::mouseMoveEvent(QMouseEvent *event){}
void Timeline::resizeEvent(QResizeEvent *event) { QWidget::resizeEvent(event); }

void Timeline::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    for (int i = 0; i < 31; ++i) {
        QPainterPath path;

        if (object->isKeyframe(i))
        {
            path.addRect(i* 20, 0, 15, height() - 1);
            QPen pen(Qt::black);
            painter.setPen(pen);
            painter.fillPath(path, Qt::black);
            painter.drawPath(path);
        }
        else
        {
            path.addRect(i* 20, 0, 15, height() - 1);
            QPen pen(Qt::black);
            painter.setPen(pen);
            painter.fillPath(path, Qt::white);
            painter.drawPath(path);
        }

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

void Timeline::addEmptyFrame()
{
    if (editor->isScribbling()) return;

    update();
    editor->update();
}

void Timeline::removeEmptyFrame()
{
    if (editor->isScribbling()) return;

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
