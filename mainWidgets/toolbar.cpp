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
    setGeometry(0, 50, 40, 280);

    pen = new ToolbarButton(mainwindow, this, 0, 0, 40, 40, ToolbarButtonStyle::TOOL_CURRENT, "P");
    line = new ToolbarButton(mainwindow, this, 0, 40, 40, 40, ToolbarButtonStyle::TOOL_TEXT, "L");
    lassofill = new ToolbarButton(mainwindow, this, 0, 80, 40, 40, ToolbarButtonStyle::TOOL_TEXT, "F");
    eraser = new ToolbarButton(mainwindow, this, 0, 120, 40, 40, ToolbarButtonStyle::TOOL_TEXT, "E");
    other = new ToolbarButton(mainwindow, this, 0, 160, 40, 40, ToolbarButtonStyle::TOOL_TEXT, "O");
    knockback = new ToolbarButton(mainwindow, this, 0, 200, 40, 40, ToolbarButtonStyle::TOOL_WHITE, "K");
    clearimage = new ToolbarButton(mainwindow, this, 0, 240, 40, 40, ToolbarButtonStyle::TOOL_RED, "X");

    connect(pen, &QAbstractButton::clicked, this, [this]{ mainwindow->getEditor()->setToolAsPen(); setCurrentTool(ToolbarTool::TOOL1); });
    connect(line, &QAbstractButton::clicked, this, [this]{ mainwindow->getEditor()->setToolAsLine(); setCurrentTool(ToolbarTool::TOOL2); });
    connect(lassofill, &QAbstractButton::clicked, this, [this]{ mainwindow->getEditor()->setToolAsLassoFill(); setCurrentTool(ToolbarTool::TOOL3); });
    connect(eraser, &QAbstractButton::clicked, this, [this]{ mainwindow->getEditor()->setToolAsEraser(); setCurrentTool(ToolbarTool::TOOL4); });
    connect(other, &QAbstractButton::clicked, this, [this]{ mainwindow->getEditor()->setToolAsEmpty(); setCurrentTool(ToolbarTool::TOOL5); });
    connect(knockback, &QAbstractButton::clicked, this, [this]{ this->mainwindow->getEditor()->knockback(); });
    connect(clearimage, &QAbstractButton::clicked, this, [this]{ this->mainwindow->getEditor()->clearImage(); });
}

void Toolbar::setCurrentTool(ToolbarTool t)
{
    currentTool = t;
    sub1()->hide();
    sub2()->hide();
    sub3()->hide();
    sub4()->hide();
    sub5()->hide();

    pen->setStyle();
    line->setStyle();
    lassofill->setStyle();
    eraser->setStyle();
    other->setStyle();

    QImage* colorIcon = new QImage(26, 26, QImage::Format_ARGB32);
    colorIcon->fill(QColor(Qt::transparent));
    QPainter colorPainter(colorIcon);

    QImage* styleIcon = new QImage(26, 26, QImage::Format_ARGB32);
    QPainter stylePainter(styleIcon);
    styleIcon->fill(QColor(Qt::transparent));
    stylePainter.setPen(QPen(Qt::transparent, 1));

    QImage* widthIcon = new QImage(20, 20, QImage::Format_ARGB32);
    QPainter widthPainter(widthIcon);
    widthIcon->fill(QColor(Qt::transparent));
    widthPainter.setPen(QPen(Qt::black, 3));
    widthPainter.drawLine(0, 10, 20, 10);

    QImage* opacityIcon = new QImage(20, 20, QImage::Format_ARGB32);
    QPainter opacityPainter(opacityIcon);
    opacityIcon->fill(QColor(Qt::transparent));
    opacityPainter.setPen(QPen(Qt::black));
    opacityPainter.drawEllipse(0,0, 19, 19);
    opacityPainter.setBrush(Qt::black);
    opacityPainter.drawPie(0, 0, 19, 19, 90*16, 180*16);

    switch (t) {
        case ToolbarTool::TOOL1: {
            QString penOpacity = QString::fromUtf8((std::to_string((mainwindow->getEditor()->getPenTool()->color().alpha()*100)/255) + "%").c_str());
            QString penWidth = QString::fromUtf8((std::to_string(mainwindow->getEditor()->getPenTool()->width())).c_str());
            colorPainter.setPen(mainwindow->getEditor()->getPenTool()->color());
            colorPainter.setBrush(mainwindow->getEditor()->getPenTool()->color());
            colorPainter.drawEllipse(0, 0, 25, 25);

            pen->setStyle(ToolbarButtonStyle::TOOL_CURRENT);
            sub1()->show(); sub1()->setImage(*colorIcon); sub1()->setStyle(SUB_ICON);
            sub2()->show(); sub2()->setText(penOpacity); sub2()->setImage(*opacityIcon); sub2()->setStyle(SUB_TEXTICON);
            sub3()->show(); sub3()->setText(penWidth); sub3()->setImage(*widthIcon); sub3()->setStyle(SUB_TEXTICON);
            subtoolbar->setGeometry(40, 70, 40, 120);
            break;
       } case ToolbarTool::TOOL2: {
            QString lineOpacity = QString::fromUtf8((std::to_string((mainwindow->getEditor()->getLineTool()->color().alpha()*100)/255) + "%").c_str());
            QString lineWidth = QString::fromUtf8((std::to_string(mainwindow->getEditor()->getLineTool()->width())).c_str());
            colorPainter.setPen(mainwindow->getEditor()->getLineTool()->color());
            colorPainter.setBrush(mainwindow->getEditor()->getLineTool()->color());
            colorPainter.drawEllipse(0,0, 25, 25);

            line->setStyle(ToolbarButtonStyle::TOOL_CURRENT);
            sub1()->show(); sub1()->setImage(*colorIcon); sub1()->setStyle(SUB_ICON);
            sub2()->show(); sub2()->setText(lineOpacity); sub2()->setImage(*opacityIcon); sub2()->setStyle(SUB_TEXTICON);
            sub3()->show(); sub3()->setText(lineWidth); sub3()->setImage(*widthIcon); sub3()->setStyle(SUB_TEXTICON);
            subtoolbar->setGeometry(40, 70, 40, 120);
            break;
        } case ToolbarTool::TOOL3: {
            QString fillOpacity = QString::fromUtf8((std::to_string((mainwindow->getEditor()->getLassoFillTool()->color().alpha()*100)/255) + "%").c_str());
            colorPainter.setPen(mainwindow->getEditor()->getLassoFillTool()->color());
            colorPainter.setBrush(mainwindow->getEditor()->getLassoFillTool()->color());
            colorPainter.drawEllipse(0,0, 25, 25);
            stylePainter.setBrush(QBrush(Qt::black, mainwindow->getEditor()->getLassoFillTool()->style()));
            stylePainter.drawRect(1, 1, 24, 24);

            lassofill->setStyle(ToolbarButtonStyle::TOOL_CURRENT);
            sub1()->show(); sub1()->setImage(*colorIcon); sub1()->setStyle(SUB_ICON);
            sub2()->show(); sub2()->setText(fillOpacity); sub2()->setImage(*opacityIcon); sub2()->setStyle(SUB_TEXTICON);
            sub3()->show(); sub3()->setImage(*styleIcon); sub3()->setStyle(SUB_ICON);
            subtoolbar->setGeometry(40, 70, 40, 120);
            break;
        } case ToolbarTool::TOOL4: {
            QString eraserWidth = QString::fromUtf8((std::to_string(mainwindow->getEditor()->getEraserTool()->width())).c_str());

            eraser->setStyle(ToolbarButtonStyle::TOOL_CURRENT);
            sub1()->show(); sub1()->setText(eraserWidth); sub1()->setImage(*widthIcon); sub1()->setStyle(SUB_TEXTICON);
            subtoolbar->setGeometry(40, 70, 40, 40);
            break;
        } case ToolbarTool::TOOL5: {
            QString backgroundOpacity = QString::fromUtf8((std::to_string((mainwindow->getEditor()->getBackgroundColor().alpha()*100)/255) + "%").c_str());
            colorPainter.setBrush(mainwindow->getEditor()->getBackgroundColor());
            colorPainter.setPen(mainwindow->getEditor()->getBackgroundColor());
            colorPainter.drawEllipse(0,0, 25, 25);

            other->setStyle(ToolbarButtonStyle::TOOL_CURRENT);
            sub1()->show(); sub1()->setImage(*colorIcon); sub1()->setStyle(SUB_ICON);
            sub2()->show(); sub2()->setText(backgroundOpacity); sub2()->setImage(*opacityIcon); sub2()->setStyle(SUB_TEXTICON);
            subtoolbar->setGeometry(40, 70, 40, 80);
            break;
        } default: break;
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

Subtoolbar::Subtoolbar(MainWindow* mw, QWidget* p): QWidget(p)
{
    mainwindow = mw;
    p = parent;
    setFocusPolicy(Qt::NoFocus);
    setGeometry(40, 80, 40, 200);

    p1 = new ToolbarButton(mainwindow, this, 0, 0, 40, 40, ToolbarButtonStyle::SUB_ICON);
    p2 = new ToolbarButton(mainwindow, this, 0, 40, 40, 40, ToolbarButtonStyle::SUB_ICON);
    p3 = new ToolbarButton(mainwindow, this, 0, 80, 40, 40, ToolbarButtonStyle::SUB_ICON);
    p4 = new ToolbarButton(mainwindow, this, 0, 120, 40, 40, ToolbarButtonStyle::SUB_ICON);
    p5 = new ToolbarButton(mainwindow, this, 0, 160, 40, 40, ToolbarButtonStyle::SUB_ICON);

    connect(p1, &QAbstractButton::clicked, this, [this]{ this->clickSubtool(ToolbarTool::SUB1); });
    connect(p2, &QAbstractButton::clicked, this, [this]{ this->clickSubtool(ToolbarTool::SUB2); });
    connect(p3, &QAbstractButton::clicked, this, [this]{ this->clickSubtool(ToolbarTool::SUB3); });
    connect(p4, &QAbstractButton::clicked, this, [this]{ this->clickSubtool(ToolbarTool::SUB4); });
    connect(p5, &QAbstractButton::clicked, this, [this]{ this->clickSubtool(ToolbarTool::SUB5); });
}

void Subtoolbar::clickSubtool(ToolbarTool sub)
{
    switch(toolbar->getCurrentTool())
    {
        case ToolbarTool::TOOL1:
            switch(sub){
                case ToolbarTool::SUB1: this->mainwindow->openPenColorWindow(); break;
                case ToolbarTool::SUB2: this->mainwindow->openPenOpacityWindow(); break;
                case ToolbarTool::SUB3: this->mainwindow->openPenWidthWindow(); break;
                case ToolbarTool::SUB4: break;
                case ToolbarTool::SUB5: break;
                default: break;
            } break;
        case ToolbarTool::TOOL2:
            switch(sub){
                case ToolbarTool::SUB1: this->mainwindow->openLineColorWindow(); break;
                case ToolbarTool::SUB2: this->mainwindow->openLineOpacityWindow(); break;
                case ToolbarTool::SUB3: this->mainwindow->openLineWidthWindow(); break;
                case ToolbarTool::SUB4: break;
                case ToolbarTool::SUB5: break;
                default: break;
            } break;
        case ToolbarTool::TOOL3:
            switch(sub){
                case ToolbarTool::SUB1: this->mainwindow->openLassofillColorWindow(); break;
                case ToolbarTool::SUB2: this->mainwindow->openLassofillOpacityWindow(); break;
                case ToolbarTool::SUB3: this->mainwindow->openLassofillStyleWindow(); break;
                case ToolbarTool::SUB4: break;
                case ToolbarTool::SUB5: break;
                default: break;
            } break;
        case ToolbarTool::TOOL4:
            switch(sub){
                case ToolbarTool::SUB1: this->mainwindow->openEraserWidthWindow(); break;
                case ToolbarTool::SUB2: break;
                case ToolbarTool::SUB3: break;
                case ToolbarTool::SUB4: break;
                case ToolbarTool::SUB5: break;
                default: break;
            } break;
        case ToolbarTool::TOOL5:
            switch(sub){
                case ToolbarTool::SUB1: this->mainwindow->openBackgroundColorWindow(); break;
                case ToolbarTool::SUB2: this->mainwindow->openBackgroundOpacityWindow(); break;
                case ToolbarTool::SUB3: break;
                case ToolbarTool::SUB4: break;
                case ToolbarTool::SUB5: break;
                default: break;
            } break;
        default:
            break;
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

ToolbarButton::ToolbarButton(MainWindow* mw, QWidget* p, int x, int y, int w, int h, ToolbarButtonStyle style, QString t, QImage img): QPushButton(p)
{
    mainwindow = mw;
    parent = p;
    setFocusPolicy(Qt::NoFocus);
    setGeometry(x, y, w, h);
    setText(t);
    setImage(img);
    setStyle(style);
}

void ToolbarButton::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QPainterPath path;
    QFont font = painter.font();

    switch(style)
    {
        case TOOL_TEXT:
            path.addRect(0, 0, width(), height());
            painter.fillPath(path, QColor(50, 50, 50));
            font.setPixelSize(12);
            painter.setFont(font);
            painter.setPen(QColor(175, 175, 175));
            painter.drawText(QRect(0, 0, width(), height()), Qt::AlignCenter, text);
            break;
        case TOOL_CURRENT:
            path.addRect(0, 0, width(), height());
            painter.fillPath(path, QColor(175, 175, 175));
            font.setPixelSize(12);
            painter.setFont(font);
            painter.setPen(QColor(50, 50, 50));
            painter.drawText(QRect(0, 0, width(), height()), Qt::AlignCenter, text);
            break;
        case TOOL_RED:
            setStyleSheet(
                "QPushButton{background-color:rgb(255,0,0);border:0px;color:white;font-size:12px;}\
                QPushButton:pressed{background-color:white;border:0px;color:rgb(50,50,50);}"
            );
            QPushButton::paintEvent(event);
            break;
        case TOOL_WHITE:
            setStyleSheet(
                "QPushButton{background-color:white;border:0px;color:black;font-size:12px;}\
                QPushButton:pressed{background-color:rgb(50,50,50);border:0px;color:white;}"
            );
            QPushButton::paintEvent(event);
            break;
        case SUB_ICON:
            path.addRect(0, 0, width(), height());
            painter.fillPath(path, QColor(175, 175, 175));
            painter.drawImage(QPoint(7,7), image);
            break;
        case SUB_TEXTICON:
            path.addRect(0, 0, width(), height());
            painter.fillPath(path, QColor(175, 175, 175));
            painter.drawImage(QPoint(10,4), image);
            font.setPixelSize(11);
            painter.setFont(font);
            painter.setPen(QColor(50, 50, 50));
            painter.drawText(QRect(0, 25, width(), 11), Qt::AlignCenter, text);
            break;
        case SUB_TOGGLE:
            break;
    }
}
