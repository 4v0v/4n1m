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
    if (editor()->isScribbling()) return;

    if (getPos() + 2 > getLayerWidgetAt(0)->frames.count())
    {
        for( auto j = timelineScroll->layers.begin(); j != timelineScroll->layers.end(); ++j )
        {
            Frame* f = new Frame(mainwindow, j.i->t(), getPos() + 1);
            Layer* l = j.i->t();
            l->frames.insert(getPos() + 1, f);
            l->hlayout->addWidget(l->frames[getPos() + 1]);
            l->update();
        }
    };
    editor()->drawSelect();
    getFrameWidgetAt(getLayer(), getPos())->toggleIsCurrent();
    setPos(getPos()+1);
    getFrameWidgetAt(getLayer(), getPos())->toggleIsCurrent();
    update();
    editor()->update();
}

void Timeline::gotoPrevFrame()
{
    if (editor()->isScribbling() || getPos() == 0) return;
    editor()->drawSelect();
    getFrameWidgetAt(getLayer(), getPos())->toggleIsCurrent();
    setPos(getPos()-1);
    getFrameWidgetAt(getLayer(), getPos())->toggleIsCurrent();
    update();
    editor()->update();
}

void Timeline::gotoNextLayer()
{
    if (editor()->isScribbling() || getLayer() == animation()->getLastLayerPos()) return;
    editor()->drawSelect();
    getFrameWidgetAt(getLayer(), getPos())->toggleIsCurrent();
    setLayer(getLayer()+1);
    getFrameWidgetAt(getLayer(), getPos())->toggleIsCurrent();
    update();
    editor()->update();
}

void Timeline::gotoPrevLayer()
{
    if (editor()->isScribbling() || getLayer() == 0) return;
    editor()->drawSelect();
    getFrameWidgetAt(getLayer(), getPos())->toggleIsCurrent();
    setLayer(getLayer()-1);
    getFrameWidgetAt(getLayer(), getPos())->toggleIsCurrent();
    update();
    editor()->update();
}

void Timeline::gotoFrame(int layer, int pos)
{
    if (editor()->isScribbling()) return;
    editor()->drawSelect();
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
    if (editor()->isScribbling() || animation()->getKeyCount(getLayer()) == 0 || getPos() >= animation()->getLastKey(getLayer())) return;
    undostack()->push(new InsertFrameCommand(getLayer(), getPos(), animation()));
}

void Timeline::removeFrame()
{
    if (editor()->isScribbling() || animation()->getKeyCount(getLayer()) == 0 || getPos() >= animation()->getLastKey(getLayer()) || animation()->isKey(getLayer(), getPos() + 1)) return;
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
    setLayoutDirection(Qt::RightToLeft);
    setFocusPolicy(Qt::NoFocus);
    setWidgetResizable( true );
    setGeometry(-1, 0, t->width(), 106 );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn);
    horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
    verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
    horizontalScrollBar()->setFocusPolicy(Qt::NoFocus);
    verticalScrollBar()->setFocusPolicy(Qt::NoFocus);
    setStyleSheet(
        "QScrollBar:vertical {background: rgb(50,50,50);width: 21px;margin: 0;}\
        QScrollBar:horizontal {background: rgb(50,50,50);height: 22px;margin: 0;}\
        QScrollBar::handle {background: rgb(50,50,50);}\
        QScrollBar::add-line, QScrollBar::sub-line {width: 0px;height: 0px;}"
    );

    QWidget* w = new QWidget();
    setWidget(w);

    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->setSpacing(0);
    vlayout->setMargin(0);
    w->setLayout( vlayout );

    for (int i = 0; i <= mainwindow->animation->getLastLayerPos(); i++)
    {
        layers.insert(i, new Layer(mainwindow, i));
        vlayout->addWidget(layers[i]);
    }
    vlayout->addStretch(1);
    getLayerWidgets()->at(t->getLayer())->getFrameWidgetAt(t->getPos())->toggleIsCurrent();
}

void TimelineScrollArea::resizeEvent(QResizeEvent* event)
{
    QScrollArea::resizeEvent(event);
    horizontalScrollBar()->setValue(horizontalScrollBar()->maximum());
}

bool TimelineScrollArea::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::Wheel && object == verticalScrollBar())
    {
        QWheelEvent* wevent = static_cast<QWheelEvent*>(event);
        verticalScrollBar()->setValue(verticalScrollBar()->value() - wevent->delta()/8);
        return true;
    }
    else if (event->type() == QEvent::Wheel && object != verticalScrollBar())
    {
        QWheelEvent* wevent = static_cast<QWheelEvent*>(event);
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() + wevent->delta()/5);
        return true;
    }
    return false;
}


