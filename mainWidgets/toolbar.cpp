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
    setGeometry(0, 40, 40, 220);

    pen = new Toolbarbutton(mainwindow, this, "P", 0, 0, 40, 40);
    pen->setIscurrent(true); pen->setCurrentStyle();
    connect(pen, &QAbstractButton::clicked, this, [this]{ this->mainwindow->getEditor()->setToolAsPen(); this->checkTool(Tool::PEN); });
    line = new Toolbarbutton(mainwindow, this, "L", 0, 40, 40, 40);
    connect(line, &QAbstractButton::clicked, this, [this]{ this->mainwindow->getEditor()->setToolAsLine(); this->checkTool(Tool::LINE); });
    lassofill = new Toolbarbutton(mainwindow, this, "F", 0, 80, 40, 40);
    connect(lassofill, &QAbstractButton::clicked, this, [this]{ this->mainwindow->getEditor()->setToolAsLassoFill(); this->checkTool(Tool::LASSOFILL); });
    eraser = new Toolbarbutton(mainwindow, this, "E", 0, 120, 40, 40);
    connect(eraser, &QAbstractButton::clicked, this, [this]{ this->mainwindow->getEditor()->setToolAsEraser(); this->checkTool(Tool::ERASER); });
    other = new Toolbarbutton(mainwindow, this, "O", 0, 160, 40, 40);
    connect(other, &QAbstractButton::clicked, this, [this]{ this->mainwindow->getEditor()->setToolAsEmpty(); this->checkTool(Tool::EMPTY); });
    undo = new Toolbarbutton(mainwindow, this, "<=", 0, 200, 20, 20);
    redo = new Toolbarbutton(mainwindow, this, "=>", 20, 200, 20, 20);
}

void Toolbar::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QPainterPath path;
    path.addRect(0, 0, width()-1, height()-1);
    painter.drawPath(path);
}

void Toolbar::checkTool(Tool t)
{
    switch (t) {
        case Tool::PEN:
            pen->setIscurrent(true); pen->setCurrentStyle();
            line->setIscurrent(false); line->setCurrentStyle();
            lassofill->setIscurrent(false); lassofill->setCurrentStyle();
            eraser->setIscurrent(false); eraser->setCurrentStyle();
            other->setIscurrent(false); other->setCurrentStyle();
            break;
        case Tool::LINE:
            pen->setIscurrent(false); pen->setCurrentStyle();
            line->setIscurrent(true); line->setCurrentStyle();
            lassofill->setIscurrent(false); lassofill->setCurrentStyle();
            eraser->setIscurrent(false); eraser->setCurrentStyle();
            other->setIscurrent(false); other->setCurrentStyle();
            break;
        case Tool::LASSOFILL:
            pen->setIscurrent(false); pen->setCurrentStyle();
            line->setIscurrent(false); line->setCurrentStyle();
            lassofill->setIscurrent(true); lassofill->setCurrentStyle();
            eraser->setIscurrent(false); eraser->setCurrentStyle();
            other->setIscurrent(false); other->setCurrentStyle();
            break;
        case Tool::ERASER:
            pen->setIscurrent(false); pen->setCurrentStyle();
            line->setIscurrent(false); line->setCurrentStyle();
            lassofill->setIscurrent(false); lassofill->setCurrentStyle();
            eraser->setIscurrent(true); eraser->setCurrentStyle();
            other->setIscurrent(false); other->setCurrentStyle();
            break;
        case Tool::EMPTY:
            pen->setIscurrent(false); pen->setCurrentStyle();
            line->setIscurrent(false); line->setCurrentStyle();
            lassofill->setIscurrent(false); lassofill->setCurrentStyle();
            eraser->setIscurrent(false); eraser->setCurrentStyle();
            other->setIscurrent(true); other->setCurrentStyle();
            break;
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

Subtoolbar::Subtoolbar(MainWindow* mw, QWidget* p): QWidget(p)
{
    mainwindow = mw;
    p = parent;
    setFocusPolicy(Qt::NoFocus);
    setGeometry(40, 80, 30, 120);

    p1 = new Toolbarbutton(mainwindow, this, "P1", 0, 0, 30, 30, true);
    p2 = new Toolbarbutton(mainwindow, this, "P2", 0, 30, 30, 30, true);
    p3 = new Toolbarbutton(mainwindow, this, "P3", 0, 60, 30, 30, true);
    p4 = new Toolbarbutton(mainwindow, this, "P4", 0, 90, 30, 30, true);
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

Toolbarbutton::Toolbarbutton(MainWindow* mw, QWidget* p, QString t, int x, int y, int w, int h, bool isSub): QPushButton(p)
{
    mainwindow = mw;
    setText(t);
    setGeometry(x, y, w, h);
    p = parent;
    text = t;
    setFocusPolicy(Qt::NoFocus);

    if (!isSub) setCurrentStyle();
    else
    {
        setStyleSheet(
            "QPushButton"
            "{"
                "background-color: lightgray;"
                "border: 1px solid lightgray;"
                "color: black;"
                "font-size: 10px;"
            "}"
            "QPushButton:pressed"
            "{"
                "background-color: rgb(50,50,51);"
                "border: 1px solid rgb(50,50,51);"
                "color: lightgray;"
                "font-size: 10px;"
            "}"

        );
    }
}

void Toolbarbutton::setCurrentStyle()
{
    if (isCurrent)
    {
        setStyleSheet(
            "QPushButton"
            "{"
                "background-color: rgb(50,50,51);"
                "border: 1px solid rgb(50,50,51);"
                "color: lightgray;"
                "font-size: 12px;"
            "}"
        );
    } else {
        setStyleSheet(
            "QPushButton"
            "{"
                "background-color: lightgray;"
                "border: 1px solid lightgray;"
                "color: black;"
                "font-size: 12px;"
            "}"
        );
    }
}
