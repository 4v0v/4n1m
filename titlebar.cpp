#include "object.h"
#include "editor.h"
#include "timeline.h"
#include "preview.h"
#include "commands.h"
#include "titlebar.h"
#include "menubar.h"

Titlebar::Titlebar(QWidget* parent): QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);

    layout->setMargin(0);
    layout->setSpacing(0);

    QLabel* titleLabel = new QLabel(tr("4n1m"));
    titleLabel->setStyleSheet(
                "QLabel"
                "{"
                    "color: lightgray;"
                    "font-size: 15px;"
                "}"
            );

    layout->addWidget(titleLabel);
    layout->setAlignment(titleLabel, Qt::AlignCenter);

    QPushButton* closeButton = new QPushButton(tr("X"));
    closeButton->setStyleSheet(
                "QPushButton"
                "{"
                    "width: 80px;"
                    "height: 25px;"
                    "background-color: rgb(50,50,51);"
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
    layout->setAlignment(closeButton, Qt::AlignRight);



    connect(closeButton, &QPushButton::clicked, this, &Titlebar::CloseWindow);

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
