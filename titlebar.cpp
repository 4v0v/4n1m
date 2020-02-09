#include "animation.h"
#include "preview.h"
#include "commands.h"
#include "editor.h"
#include "timeline.h"
#include "titlebar.h"
#include "toolbar.h"

Titlebar::Titlebar(MainWindow* mw): QWidget(mw)
{
    mainwindow = mw;
    setGeometry(0, 0, mainwindow->getWindowDimensions().width(), 25);
    setMaximumHeight(25);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    menubar = new Menubar(mainwindow);
    QLabel* titleLabel = new QLabel(tr("4n1m"));
    QPushButton* minimizeButton = new QPushButton(tr("_"));
    QPushButton* closeButton = new QPushButton(tr("x"));

    layout->addWidget(menubar);
    layout->addWidget(titleLabel, 3, Qt::AlignCenter);
    layout->addWidget(minimizeButton);
    layout->addWidget(closeButton);

    titleLabel->setStyleSheet("QLabel{color: rgb(210,210,210);font-size: 12px;}");
    minimizeButton->setFocusPolicy(Qt::NoFocus);
    minimizeButton->setStyleSheet(
        "QPushButton{width: 35px;height: 25px;background-color: rgb(50,50,50);border: 0px;color: rgb(210,210,210);}\
        QPushButton:hover:!pressed{color: white;background-color: rgb(75,75,75);border: 0px;}"
    );
    closeButton->setFocusPolicy(Qt::NoFocus);
    closeButton->setStyleSheet(
        "QPushButton{width: 35px;height: 25px;background-color: rgb(50,50,50);border: 0px;color: rgb(210,210,210);}\
        QPushButton:hover:!pressed{color: white;background-color: red;border: 0px;}"
    );

    connect(minimizeButton, &QPushButton::clicked, this, &Titlebar::minimizeWindow);
    connect(closeButton, &QPushButton::clicked, this, &Titlebar::closeWindow);

    update();
}

void Titlebar::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QPainterPath path;

    path.addRect(0, 0, width(), height());
    painter.fillPath(path, QColor(50, 50, 50));
    painter.setPen(QPen(QColor(50, 50, 50), 1));
    painter.drawPath(path);
}

void Titlebar::mousePressEvent(QMouseEvent *event)
{
    p = event->pos();
    isDown = true;
}

void Titlebar::mouseReleaseEvent(QMouseEvent*)
{
    isDown = false;
}

void Titlebar::mouseMoveEvent(QMouseEvent *event)
{
    if(!isDown) return;
    QPoint diff= event->pos() - p;
    window()->move(window()->pos()+diff);
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

Menubar::Menubar(MainWindow* mw): QMenuBar()
{
    mainwindow = mw;
    setMinimumWidth(70);
    setMaximumWidth(70);

    setStyleSheet(
        "QMenuBar{background-color: rgb(50,50,50);color: rgb(210,210,210);width: 20px;height: 20px;}\
        QMenuBar::item:selected{background-color: rgb(75,75,75);color: rgb(210,210,210);}"
    );

    options = new QMenu(tr("="));
    options->setStyleSheet(
        "QMenu{background-color: rgb(50,50,50);color: rgb(210,210,210);}\
        QMenu::item:selected{background-color: rgb(75,75,75);color: rgb(210,210,210);}"
    );
    addMenu(options);
}

void Menubar::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QPainterPath path;

    QMenuBar::paintEvent(event);
}


