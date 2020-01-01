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
    setGeometry(0, 40, 40, 300);

    pen = new Toolbarbutton(mainwindow, this, "P", 0, 0, 40, 40);
    pen->setIscurrent(true); pen->setCurrentStyle();
    connect(pen, &QAbstractButton::clicked, this, [this]{ this->mainwindow->getEditor()->setToolAsPen(); this->switchToolbarButton(1); });
    line = new Toolbarbutton(mainwindow, this, "L", 0, 40, 40, 40);
    connect(line, &QAbstractButton::clicked, this, [this]{ this->mainwindow->getEditor()->setToolAsLine(); this->switchToolbarButton(2); });
    lassofill = new Toolbarbutton(mainwindow, this, "F", 0, 80, 40, 40);
    connect(lassofill, &QAbstractButton::clicked, this, [this]{ this->mainwindow->getEditor()->setToolAsLassoFill(); this->switchToolbarButton(3); });
    eraser = new Toolbarbutton(mainwindow, this, "E", 0, 120, 40, 40);
    connect(eraser, &QAbstractButton::clicked, this, [this]{ this->mainwindow->getEditor()->setToolAsEraser(); this->switchToolbarButton(4); });
    other = new Toolbarbutton(mainwindow, this, "O", 0, 160, 40, 40);
    connect(other, &QAbstractButton::clicked, this, [this]{ this->mainwindow->getEditor()->setToolAsEmpty(); this->switchToolbarButton(5); });
    knockback = new Toolbarbutton(mainwindow, this, "K", 0, 200, 40, 40);
    knockback->setStyleSheet(
        "QPushButton"
        "{"
            "background-color: white;"
            "border: 1px solid white;"
            "color: black;"
            "font-size: 10px;"
        "}"
        "QPushButton:pressed"
        "{"
            "background-color: rgb(50,50,50);"
            "border: 1px solid rgb(50,50,50);"
            "color: white;"
            "font-size: 10px;"
        "}"
    );
    connect(knockback, &QAbstractButton::clicked, this, [this]{ this->mainwindow->getEditor()->knockback(); });
    clearimage = new Toolbarbutton(mainwindow, this, "X", 0, 240, 40, 40);
    clearimage->setStyleSheet(
        "QPushButton"
        "{"
            "background-color: rgb(255,0,0);"
            "border: 1px solid rgb(255,0,0);"
            "color: white;"
            "font-size: 10px;"
        "}"
        "QPushButton:pressed"
        "{"
            "background-color: white;"
            "border: 1px solid white;"
            "color: rgb(50,50,50);"
            "font-size: 10px;"
        "}"
    );
    connect(clearimage, &QAbstractButton::clicked, this, [this]{ this->mainwindow->getEditor()->clearImage(); });

    undo = new Toolbarbutton(mainwindow, this, "u", 0, 280, 20, 20);
    connect(undo, &QAbstractButton::clicked, this, [this]{ this->mainwindow->getUndoStack()->undo(); });
    redo = new Toolbarbutton(mainwindow, this, "r", 20, 280, 20, 20);
    connect(redo, &QAbstractButton::clicked, this, [this]{ this->mainwindow->getUndoStack()->redo(); });
}

void Toolbar::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QPainterPath path;
    path.addRect(0, 0, width(), height());
    painter.drawPath(path);
}

void Toolbar::switchToolbarButton(int i)
{
    switch (i) {
        case 1: {
            pen->setIscurrent(true); pen->setCurrentStyle();
            line->setIscurrent(false); line->setCurrentStyle();
            lassofill->setIscurrent(false); lassofill->setCurrentStyle();
            eraser->setIscurrent(false); eraser->setCurrentStyle();
            other->setIscurrent(false); other->setCurrentStyle();

            QPixmap* color = new QPixmap(32, 32);
            color->fill(mainwindow->getEditor()->getPenTool()->color().rgb());
            QPainter painter(color);
            QPainterPath path;
            path.addRect(0, 0, 31, 31);
            painter.drawPath(path);

            subtoolbar->p1->show(); subtoolbar->p1->setText(""); subtoolbar->p1->setIcon(QIcon(*color));
            subtoolbar->p2->show(); subtoolbar->p2->setText(tr("O"));
            subtoolbar->p3->show(); subtoolbar->p3->setText(tr("W"));
            subtoolbar->p4->show(); subtoolbar->p4->hide();
            subtoolbar->setGeometry(40, 60, 30, 90);
            break;
       } case 2: {
            pen->setIscurrent(false); pen->setCurrentStyle();
            line->setIscurrent(true); line->setCurrentStyle();
            lassofill->setIscurrent(false); lassofill->setCurrentStyle();
            eraser->setIscurrent(false); eraser->setCurrentStyle();
            other->setIscurrent(false); other->setCurrentStyle();

            QPixmap* color = new QPixmap(32, 32);
            color->fill(mainwindow->getEditor()->getLineTool()->color().rgb());
            QPainter painter(color);
            QPainterPath path;
            path.addRect(0, 0, 31, 31);
            painter.drawPath(path);

            subtoolbar->p1->show(); subtoolbar->p1->setText(tr("")); subtoolbar->p1->setIcon(QIcon(*color));
            subtoolbar->p2->show(); subtoolbar->p2->setText(tr("O"));
            subtoolbar->p3->show(); subtoolbar->p3->setText(tr("W"));
            subtoolbar->p4->show(); subtoolbar->p4->hide();
            subtoolbar->setGeometry(40, 60, 30, 90);
            break;
        } case 3: {
            pen->setIscurrent(false); pen->setCurrentStyle();
            line->setIscurrent(false); line->setCurrentStyle();
            lassofill->setIscurrent(true); lassofill->setCurrentStyle();
            eraser->setIscurrent(false); eraser->setCurrentStyle();
            other->setIscurrent(false); other->setCurrentStyle();

            QPixmap* color = new QPixmap(32, 32);
            color->fill(mainwindow->getEditor()->getLassoFillTool()->color().rgb());
            QPainter painter(color);
            QPainterPath path;
            path.addRect(0, 0, 31, 31);
            painter.drawPath(path);

            subtoolbar->p1->show(); subtoolbar->p1->setText(tr("")); subtoolbar->p1->setIcon(QIcon(*color));
            subtoolbar->p2->show(); subtoolbar->p2->setText(tr("O"));
            subtoolbar->p3->show(); subtoolbar->p3->setText(tr("S"));
            subtoolbar->p4->show(); subtoolbar->p4->hide();
            subtoolbar->setGeometry(40, 60, 30, 90);
            break;
        } case 4: {
            pen->setIscurrent(false); pen->setCurrentStyle();
            line->setIscurrent(false); line->setCurrentStyle();
            lassofill->setIscurrent(false); lassofill->setCurrentStyle();
            eraser->setIscurrent(true); eraser->setCurrentStyle();
            other->setIscurrent(false); other->setCurrentStyle();

            subtoolbar->p1->show(); subtoolbar->p1->setText(tr("W")); subtoolbar->p1->setIcon(QIcon());
            subtoolbar->p2->show(); subtoolbar->p2->hide();
            subtoolbar->p3->show(); subtoolbar->p3->hide();
            subtoolbar->p4->show(); subtoolbar->p4->hide();
            subtoolbar->setGeometry(40, 60, 30, 30);
            break;
        } case 5: {
            pen->setIscurrent(false); pen->setCurrentStyle();
            line->setIscurrent(false); line->setCurrentStyle();
            lassofill->setIscurrent(false); lassofill->setCurrentStyle();
            eraser->setIscurrent(false); eraser->setCurrentStyle();
            other->setIscurrent(true); other->setCurrentStyle();

            QPixmap* color = new QPixmap(32, 32);
            color->fill(mainwindow->getEditor()->getBackgroundColor().rgb());
            QPainter painter(color);
            QPainterPath path;
            path.addRect(0, 0, 31, 31);
            painter.drawPath(path);

            subtoolbar->p1->show(); subtoolbar->p1->setText(tr("")); subtoolbar->p1->setIcon(QIcon(*color));
            subtoolbar->p2->show(); subtoolbar->p2->setText(tr("O"));
            subtoolbar->p3->show(); subtoolbar->p3->hide();
            subtoolbar->p4->show(); subtoolbar->p4->hide();
            subtoolbar->setGeometry(40, 60, 30, 60);
            break;
        }
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

Subtoolbar::Subtoolbar(MainWindow* mw, QWidget* p): QWidget(p)
{
    mainwindow = mw;
    p = parent;
    setFocusPolicy(Qt::NoFocus);
    setGeometry(40, 60, 30, 120);

    p1 = new Toolbarbutton(mainwindow, this, "", 0, 0, 30, 30, true);
    QPixmap* colorIcon = new QPixmap(32, 32);
    colorIcon->fill(mainwindow->getEditor()->getPenTool()->color().rgb());
    p1->setIcon(QIcon(*colorIcon));
    connect(p1, &QAbstractButton::clicked, this, [this]{ this->clickSubtool(1); });


    p2 = new Toolbarbutton(mainwindow, this, "O", 0, 30, 30, 30, true);
    connect(p2, &QAbstractButton::clicked, this, [this]{ this->clickSubtool(2); });
    p3 = new Toolbarbutton(mainwindow, this, "W", 0, 60, 30, 30, true);
    connect(p3, &QAbstractButton::clicked, this, [this]{ this->clickSubtool(3); });
    p4 = new Toolbarbutton(mainwindow, this, "S", 0, 90, 30, 30, true);
    connect(p4, &QAbstractButton::clicked, this, [this]{ this->clickSubtool(4); });
}

void Subtoolbar::clickSubtool(int nbr)
{
    switch(mainwindow->getEditor()->getCurrentTool())
    {
        case Tool::PEN:
            switch(nbr){
                case 1: this->mainwindow->openPenColorWindow(); break;
                case 2: this->mainwindow->openPenOpacityWindow(); break;
                case 3: this->mainwindow->openPenWidthWindow(); break;
                case 4: break;
            }
            break;
        case Tool::LINE:
            switch(nbr){
                case 1: this->mainwindow->openLineColorWindow(); break;
                case 2: this->mainwindow->openLineOpacityWindow(); break;
                case 3: this->mainwindow->openLineWidthWindow(); break;
                case 4: break;
            }
            break;
        case Tool::LASSOFILL:
            switch(nbr){
                case 1: this->mainwindow->openLassofillColorWindow(); break;
                case 2: this->mainwindow->openLassofillOpacityWindow(); break;
                case 3: this->mainwindow->openLassofillStyleWindow(); break;
                case 4: break;
            }
            break;
        case Tool::ERASER:
            switch(nbr){
                case 1: this->mainwindow->openEraserWidthWindow(); break;
                case 2: break;
                case 3: break;
                case 4: break;
            }
            break;
        case Tool::EMPTY:
            switch(nbr){
                case 1: this->mainwindow->openBackgroundColorWindow(); break;
                case 2: this->mainwindow->openBackgroundOpacityWindow(); break;
                case 3: break;
                case 4: break;
            }
            break;
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

Toolbarbutton::Toolbarbutton(MainWindow* mw, QWidget* p, QString t, int x, int y, int w, int h, bool b): QPushButton(p)
{
    mainwindow = mw;
    setText(t);
    setGeometry(x, y, w, h);
    p = parent;
    text = t;
    setFocusPolicy(Qt::NoFocus);
    isSub = b;

    setCurrentStyle();
}

void Toolbarbutton::setCurrentStyle()
{
    if (isCurrent && !isSub)
    {
        setStyleSheet(
            "QPushButton"
            "{"
                "background-color: rgb(50,50,50);"
                "border: 1px solid rgb(50,50,50);"
                "color: rgb(210,210,210);"
                "font-size: 12px;"
            "}"
        );
    } else {
        setStyleSheet(
            "QPushButton"
            "{"
                "background-color: rgb(210,210,210);"
                "border: 1px solid rgb(210,210,210);"
                "color: rgb(50,50,50);"
                "font-size: 12px;"
            "}"
        );
    }

    if (isSub) {
        setStyleSheet(
            "QPushButton"
            "{"
                "background-color: rgb(175,175,175);"
                "border: 1px solid rgb(175,175,175);"
                "color: rgb(50,50,50);"
                "font-size: 10px;"
            "}"
            "QPushButton:pressed"
            "{"
                "background-color: rgb(50,50,50);"
                "border: 1px solid rgb(50,50,50);"
                "color: rgb(210,210,210);"
                "font-size: 10px;"
            "}"
        );
    }
}
