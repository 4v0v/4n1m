#include "mainwindow.h"
#include "animation.h"
#include "preview.h"
#include "commands.h"
#include "mainWidgets/editor.h"
#include "mainWidgets/timeline.h"
#include "mainWidgets/titlebar.h"
#include "mainWidgets/toolbar.h"

Toolbar::Toolbar(MainWindow* mw, QWidget* p): QWidget(p)
{
    mainwindow = mw;
    p = parent;
    setFocusPolicy(Qt::NoFocus);
    setGeometry(0, 40, 80, 220);

    QPushButton* pen = new QPushButton("P", this);
    pen->setGeometry(0, 0, 40, 40);
    pen->setFocusPolicy(Qt::NoFocus);
    connect(pen, &QAbstractButton::clicked, this, [this]{ this->mainwindow->getEditor()->setToolAsPen(); this->mainwindow->checkTool(Tool::PEN); });

    QPushButton* line = new QPushButton("L", this);
    line->setGeometry(0, 40, 40, 40);
    line->setFocusPolicy(Qt::NoFocus);
    connect(line, &QAbstractButton::clicked, this, [this]{ this->mainwindow->getEditor()->setToolAsLine(); this->mainwindow->checkTool(Tool::LINE); });

    QPushButton* lassofill = new QPushButton("F", this);
    lassofill->setGeometry(0, 80, 40, 40);
    lassofill->setFocusPolicy(Qt::NoFocus);
    connect(lassofill, &QAbstractButton::clicked, this, [this]{ this->mainwindow->getEditor()->setToolAsLassoFill(); this->mainwindow->checkTool(Tool::LASSOFILL); });

    QPushButton* eraser = new QPushButton("E", this);
    eraser->setGeometry(0, 120, 40, 40);
    eraser->setFocusPolicy(Qt::NoFocus);
    connect(eraser, &QAbstractButton::clicked, this, [this]{ this->mainwindow->getEditor()->setToolAsEraser(); this->mainwindow->checkTool(Tool::ERASER); });

    QPushButton* other = new QPushButton("O", this);
    other->setGeometry(0, 160, 40, 40);
    other->setFocusPolicy(Qt::NoFocus);
    QPushButton* undo = new QPushButton("<=", this);
    undo->setGeometry(0, 200, 20, 20);
    undo->setFocusPolicy(Qt::NoFocus);
    QPushButton* redo = new QPushButton("=>", this);
    redo->setGeometry(20, 200, 20, 20);
    redo->setFocusPolicy(Qt::NoFocus);

    QPushButton* p1 = new QPushButton("P1", this);
    p1->setGeometry(40, 30, 30, 30);
    p1->setFocusPolicy(Qt::NoFocus);
    QPushButton* p2 = new QPushButton("P2", this);
    p2->setGeometry(40, 60, 30, 30);
    p2->setFocusPolicy(Qt::NoFocus);
    QPushButton* p3 = new QPushButton("P3", this);
    p3->setGeometry(40, 90, 30, 30);
    p3->setFocusPolicy(Qt::NoFocus);
    QPushButton* p4 = new QPushButton("P4", this);
    p4->setGeometry(40, 120, 30, 30);
    p4->setFocusPolicy(Qt::NoFocus);
}

void Toolbar::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QPainterPath path;
    path.addRect(0, 0, width()-1, height()-1);
    painter.drawPath(path);
}
