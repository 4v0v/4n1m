#include "animation.h"
#include "preview.h"
#include "commands.h"
#include "mainWidgets/editor.h"
#include "mainWidgets/timeline.h"
#include "mainWidgets/titlebar.h"
#include "mainWidgets/layer.h"
#include "mainWidgets/frame.h"
#include "mainWidgets/toolbar.h"

Timeline::Timeline(MainWindow* mw): QWidget(mw)
{
    mainwindow = mw;
    setGeometry(0, 0, mainwindow->getWindowDimensions().width(), 75);

    timelineScroll = new TimelineScrollArea(mainwindow, this);
}

void Timeline::gotoNextFrame()
{
    if (editor()->isScribbling() || getPos() == 199) return;
    getFrameWidgetAt(getLayer(), getPos())->toggleIsCurrent();
    setPos(getPos()+1);
    getFrameWidgetAt(getLayer(), getPos())->toggleIsCurrent();
    update();
    editor()->update();
}

void Timeline::gotoPrevFrame()
{
    if (editor()->isScribbling() || getPos() == 0) return;
    getFrameWidgetAt(getLayer(), getPos())->toggleIsCurrent();
    setPos(getPos()-1);
    getFrameWidgetAt(getLayer(), getPos())->toggleIsCurrent();
    update();
    editor()->update();
}

void Timeline::gotoNextLayer()
{
    if (editor()->isScribbling() || getLayer() == animation()->getLastLayerPos()) return;
    getFrameWidgetAt(getLayer(), getPos())->toggleIsCurrent();
    setLayer(getLayer()+1);
    getFrameWidgetAt(getLayer(), getPos())->toggleIsCurrent();
    update();
    editor()->update();
}

void Timeline::gotoPrevLayer()
{
    if (editor()->isScribbling() || getLayer() == 0) return;
    getFrameWidgetAt(getLayer(), getPos())->toggleIsCurrent();
    setLayer(getLayer()-1);
    getFrameWidgetAt(getLayer(), getPos())->toggleIsCurrent();
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

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

TimelineScrollArea::TimelineScrollArea(MainWindow* mw, Timeline* t) : QScrollArea(t)
{
    mainwindow = mw;

    setFocusPolicy(Qt::NoFocus);
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn);
    horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
    verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

    setLayoutDirection(Qt::RightToLeft);

    setWidgetResizable( true );
    setGeometry(t->x(), t->y(), t->width(), 101 );

    setStyleSheet(
        "QScrollBar:vertical {\
            background: rgb(50,50,50);\
            width: 15px;\
            margin: 0;\
        }\
        \
        QScrollBar:horizontal {\
            background: rgb(50,50,50);\
            height: 15px;\
            margin: 0;\
        }\
        \
        QScrollBar::handle {\
            background: rgb(50,50,50);\
        }\
        \
        QScrollBar::add-line, QScrollBar::sub-line {\
            width: 0px;\
            height: 0px;\
        }"
    );

    QWidget* w = new QWidget();
    setWidget(w);

    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->setSpacing(0);
    vlayout->setMargin(0);
    w->setLayout( vlayout );

    for (int i = 0; i <= mainwindow->getAnimation()->getLastLayerPos(); i++)
    {
        layers.insert(i, new Layer(mainwindow, i));
        vlayout->addWidget(layers[i]);
    }
    vlayout->addStretch(1);

    getLayerWidgets()->at(t->getLayer())->getFrameWidgetAt(t->getPos())->toggleIsCurrent();
}

void TimelineScrollArea::wheelEvent(QWheelEvent* event)
{
    horizontalScrollBar()->setValue(
        horizontalScrollBar()->value() + event->delta()/5
    );
}

void TimelineScrollArea::resizeEvent(QResizeEvent* event)
{
    QScrollArea::resizeEvent(event);
    horizontalScrollBar()->setValue(horizontalScrollBar()->maximum());
}


