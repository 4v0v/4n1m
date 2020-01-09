#include "animation.h"
#include "preview.h"
#include "commands.h"
#include "mainWidgets/editor.h"
#include "mainWidgets/timeline.h"
#include "mainWidgets/titlebar.h"
#include "mainWidgets/toolbar.h"
#include "mainWidgets/layer.h"
#include "mainWidgets/frame.h"

Layer::Layer(MainWindow* mw, int l): QWidget(mw)
{
    mainwindow = mw;
    layerPos = l;

    setMaximumHeight(28);
    setMinimumHeight(28);
    hlayout = new QHBoxLayout();
    hlayout->setSpacing(0);
    hlayout->setMargin(0);
    setLayout( hlayout );

    layerTitle = new LayerTitle(mainwindow);
    layerTitle->setText(tr(("Layer_" + std::to_string(getPos())).c_str()));
    hlayout->addWidget(layerTitle);

    for (int i =0; i < initialFrameNumber; i++)
    {
        frames.insert(i, new Frame(mainwindow, this, i));
        hlayout->addWidget(frames[i]);
    }

    hlayout->addStretch(1);
    setLayoutDirection(Qt::LeftToRight);
}

void Layer::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QPainterPath path;
    path.addRect(0, 0, width(), height());
    painter.fillPath(path, Qt::white);
    painter.drawPath(path);
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

LayerTitle::LayerTitle(MainWindow* mw) : QLabel(mw)
{
    mainwindow = mw;
    setStyleSheet("QLabel { qproperty-alignment: AlignCenter; color: rgb(50,50,50); font-size: 12px; min-width: 75; }");
}

void LayerTitle::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(50, 50, 50), 1));
    painter.setBrush(QBrush(QColor(175, 175, 175)));
    painter.drawRect(0, 0, width() * opacity, height());
    QLabel::paintEvent(event);
}

void LayerTitle::mouseMoveEvent(QMouseEvent* event)
{
    if (!isDown) return;
    double newOpacity = (event->x() * 1.0 / width() * 1.0);
    if (newOpacity > 1) newOpacity = 1.0;
    if (newOpacity < 0) newOpacity = 0.0;
    opacity = newOpacity;
    update();
    mainwindow->editor->update();
}

void LayerTitle::mousePressEvent(QMouseEvent* event)
{
    isDown = true;
    double newOpacity = (event->x() * 1.0 / width() * 1.0);
    if (newOpacity > 1) newOpacity = 1.0;
    if (newOpacity < 0) newOpacity = 0.0;
    opacity = newOpacity;
    update();
    mainwindow->editor->update();
}

void LayerTitle::mouseReleaseEvent(QMouseEvent*) { isDown = false; }
