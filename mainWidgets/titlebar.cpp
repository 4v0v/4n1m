#include "animation.h"
#include "preview.h"
#include "commands.h"
#include "mainWidgets/editor.h"
#include "mainWidgets/timeline.h"
#include "mainWidgets/titlebar.h"
#include "mainWidgets/toolbar.h"

Titlebar::Titlebar(MainWindow* mw): QWidget(mw)
{
    mainwindow = mw;
    setGeometry(0, 0, mainwindow->getWindowDimensions().width(), 25);
    setMaximumHeight(25);

    QHBoxLayout* layout = new QHBoxLayout(this);

    layout->setMargin(0);
    layout->setSpacing(0);

    menubar = new Menubar(mainwindow);
    layout->addWidget(menubar);

    QLabel* titleLabel = new QLabel(tr("4n1m"));
    titleLabel->setStyleSheet(
                "QLabel"
                "{"
                    "color: lightgray;"
                    "font-size: 15px;"
                "}"
            );
    layout->addWidget(titleLabel, 3, Qt::AlignCenter);

    QPushButton* minimizeButton = new QPushButton(tr("_"));
    minimizeButton->setFocusPolicy(Qt::NoFocus);
    minimizeButton->setStyleSheet(
                "QPushButton"
                "{"
                    "width: 80px;"
                    "height: 25px;"
                    "background-color: rgb(50,50,51);"
                    "color: lightgray;"
                    "font-size: 15px;"
                    "border: 1px solid rgb(50,50,51);"
                "}"
                "QPushButton:hover:!pressed"
                "{"
                    "color: white;"
                    "background-color: rgb(75,75,75);"
                    "border: 1px solid rgb(75,75,75);"
                "}"
            );
    layout->addWidget(minimizeButton);
    connect(minimizeButton, &QPushButton::clicked, this, &Titlebar::minimizeWindow);

    QPushButton* closeButton = new QPushButton(tr("X"));
    closeButton->setFocusPolicy(Qt::NoFocus);
    closeButton->setStyleSheet(
                "QPushButton"
                "{"
                    "width: 80px;"
                    "height: 25px;"
                    "background-color: rgb(50,50,51);"
                    "border: 1px solid rgb(50,50,51);"
                    "color: lightgray;"
                    "font-size: 15px;"
                "}"
                "QPushButton:hover:!pressed"
                "{"
                    "color: white;"
                    "background-color: red;"
                    "border: 1px solid red;"
                "}"
            );
    layout->addWidget(closeButton);
    connect(closeButton, &QPushButton::clicked, this, &Titlebar::closeWindow);

    update();
}

void Titlebar::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QPainterPath path;

    path.addRect(0, 0, width(), height());
    painter.fillPath(path, QColor(50, 50, 51));
    painter.setPen(QPen(QColor(50, 50, 51), 1));
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

    setStyleSheet(
        "QMenuBar"
        "{"
            "background-color: rgb(50,50,51);"
            "color: lightgray;"
            "font-size: 15px;"
        "}"
        "QMenuBar::item:selected"
        "{"
            "background-color: rgb(75,75,75);"
            "color: lightgray;"
            "font-size: 15px;"
        "}"
    );

    QString menuStyle =
    "QMenu"
    "{"
        "background-color: rgb(50,50,51);"
        "color: lightgray;"
        "font-size: 12px;"
    "}"
    "QMenu::item:selected"
    "{"
        "background-color: rgb(75,75,75);"
        "color: lightgray;"
        "font-size: 14px;"
    "}";

    options = new QMenu(tr("="));
    options->setStyleSheet(menuStyle);

    tools = new QMenu(tr("+"));
    tools->setStyleSheet(menuStyle);

    addMenu(options);
    addMenu(tools);
}

void Menubar::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QPainterPath path;

    QMenuBar::paintEvent(event);
}


