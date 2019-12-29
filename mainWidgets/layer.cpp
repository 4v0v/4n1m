#include "animation.h"
#include "preview.h"
#include "commands.h"
#include "mainWidgets/editor.h"
#include "mainWidgets/timeline.h"
#include "mainWidgets/titlebar.h"
#include "mainWidgets/menubar.h"
#include "mainWidgets/layer.h"
#include "mainWidgets/frame.h"

Layer::Layer(MainWindow* mw, int l): QWidget(mw)
{
    mainwindow = mw;
    layerPos = l;

    setMaximumHeight(28);
    setMinimumHeight(28);
    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setSpacing(0);
    hlayout->setMargin(0);
    setLayout( hlayout );

    QLabel* titleLabel = new QLabel(tr(("Layer_" + std::to_string(getPos())).c_str()));
    titleLabel->setStyleSheet(
        "QLabel {"
            "qproperty-alignment: AlignCenter;"
            "color: black;"
            "font-size: 12px;"
            "min-width: 75;"
        "}"
    );
    hlayout->addWidget(titleLabel);

    for (int i =0; i < 200; i++)
    {
        frames.insert(i, new Frame(mainwindow, this, i));
        hlayout->addWidget(frames[i]);
    }

    hlayout->addStretch(1);

}

void Layer::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QPainterPath path;

    path.addRect(0, 0, width(), height());
    painter.fillPath(path, Qt::gray);
    painter.drawPath(path);
}
