#include "animation.h"
#include "preview.h"
#include "commands.h"
#include "mainWidgets/editor.h"
#include "mainWidgets/timeline.h"
#include "mainWidgets/titlebar.h"
#include "mainWidgets/menubar.h"
#include "mainWidgets/layer.h"
#include "mainWidgets/frame.h"

Timeline::Timeline(MainWindow* mw): QWidget(mw)
{
    mainwindow = mw;
    setGeometry(0, 0, mainwindow->getWindowDimensions().width(), 75);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    scrollArea->setWidgetResizable( true );
    scrollArea->setGeometry(x(), y(), width(), 101 );

    QWidget* w = new QWidget();
    scrollArea->setWidget(w);
    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->setSpacing(0);
    vlayout->setMargin(0);
    w->setLayout( vlayout );

    for (int i =0; i < 3; i++)
    {
        layers.insert(i, new Layer(mainwindow, i));
        vlayout->addWidget(layers[i]);
    }
    vlayout->addStretch(1);
    getFrameWidgetAt(getLayer(), getPos())->toggleIsCurrent();
}

void Timeline::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QPainterPath path;
    path.addRect(0, 0, width(), height());
    painter.fillPath(path, Qt::gray);
    painter.drawPath(path);
}

void Timeline::gotoNextFrame()
{
    if (editor()->isScribbling()) return;
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
