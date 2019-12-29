#include "animation.h"
#include "preview.h"
#include "commands.h"
#include "mainWidgets/editor.h"
#include "mainWidgets/timeline.h"
#include "mainWidgets/titlebar.h"
#include "mainWidgets/menubar.h"
#include "mainWidgets/layer.h"
#include "mainWidgets/frame.h"

Frame::Frame(MainWindow* mw, Layer* l, int p): QWidget(mw)
{
    mainwindow = mw;
    layer = l;
    framePos = p;
    setMinimumWidth(15);
    setMaximumWidth(15);
}

void Frame::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    QPainterPath path;
    path.addRect(0, 0, width(), height()-1);
    painter.fillPath(path, keyColor);
    painter.setPen(QPen(Qt::black, 1));
    painter.drawPath(path);
    painter.setPen(QPen(currentColor, 6));
    painter.drawPath(path);
}

void Frame::mousePressEvent(QMouseEvent*)
{
    timeline()->getFrameWidgetAt(timeline()->getLayer(), timeline()->getPos())->toggleIsCurrent();
    toggleIsCurrent();
    timeline()->gotoFrame(layer->getPos(), framePos);
    update();
}

void Frame::toggleIsCurrent(){
    isCurrent = !isCurrent;

    if (isCurrent)
    {
        currentColorAnim->stop();
        currentColorAnim->setLoopCount(-1);
        currentColorAnim->setDuration(1000);
        currentColorAnim->setKeyValueAt(0, 255);
        currentColorAnim->setKeyValueAt(0.5, 0);
        currentColorAnim->setKeyValueAt(1, 255);
        currentColorAnim->start();
    } else {
        currentColorAnim->stop();
        currentColorAnim->setLoopCount(1);
        currentColorAnim->setDuration(500);
        currentColorAnim->setKeyValueAt(0, currentColor.alpha());
        currentColorAnim->setKeyValueAt(0.5, 0);
        currentColorAnim->setKeyValueAt(1, 0);
        currentColorAnim->start();
    }
}

void Frame::toggleIsKey()
{
    isKey = !isKey;

    if (isKey)
    {
        keyColorAnim->setDuration(500);
        keyColorAnim->setKeyValueAt(0, 255);
        keyColorAnim->setKeyValueAt(1, 0);
        keyColorAnim->start();
    } else {
        keyColorAnim->setDuration(500);
        keyColorAnim->setKeyValueAt(0, 0);
        keyColorAnim->setKeyValueAt(1, 255);
        keyColorAnim->start();
    }
}
