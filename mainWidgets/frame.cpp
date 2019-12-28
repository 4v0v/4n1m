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

    path.addRect(0, 0, width(), height());
    painter.fillPath(path, isKey ? Qt::black : Qt::white );
    if (isCurrent) painter.setPen(QPen(Qt::red, 6));
    else painter.setPen(QPen(Qt::black, 1));

    painter.drawPath(path);
}

void Frame::mousePressEvent(QMouseEvent*)
{
    timeline()->getFrameWidgetAt(timeline()->getLayer(), timeline()->getPos())->toggleIsCurrent();
    toggleIsCurrent();
    timeline()->gotoFrame(layer->getPos(), framePos);
    update();
}

