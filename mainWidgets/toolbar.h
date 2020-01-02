#ifndef TOOLBAR_H
#define TOOLBAR_H

#include "mainwindow.h"

class ToolbarButton : public QPushButton
{
    Q_OBJECT
public:
    ToolbarButton(MainWindow*, QWidget*, int, int, int, int, ToolbarButtonStyle = ToolbarButtonStyle::TOOL_TEXT, QString = "", QImage i = QImage(1, 1, QImage::Format_ARGB32));
    void setStyle(ToolbarButtonStyle s = ToolbarButtonStyle::TOOL_TEXT){ style = s; update(); };
    void setText(QString t = ""){ text = t; }
    void setImage(QImage i = QImage(1, 1, QImage::Format_ARGB32)){ image = i; }

protected:
    void paintEvent(QPaintEvent*) override;

private:
    MainWindow* mainwindow;
    QWidget* parent;
    QString text;
    QImage image;
    ToolbarButtonStyle style;
};

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

class Subtoolbar : public QWidget
{
    Q_OBJECT
public:
    Subtoolbar(MainWindow*, QWidget*);
    void setToolbar(Toolbar* t) { toolbar = t;}
    void clickSubtool(ToolbarTool);
    ToolbarButton* p1;
    ToolbarButton* p2;
    ToolbarButton* p3;
    ToolbarButton* p4;
    ToolbarButton* p5;

private:
    MainWindow* mainwindow;
    QWidget* parent;
    Toolbar* toolbar;
};

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

class Toolbar : public QWidget
{
    Q_OBJECT
public:
    Toolbar(MainWindow*, QWidget*);
    void setSuboolbar(Subtoolbar* s) { subtoolbar = s; }
    ToolbarTool getCurrentTool() { return currentTool; }
    void setCurrentTool(ToolbarTool t);
    ToolbarButton* sub1() { return subtoolbar->p1; }
    ToolbarButton* sub2() { return subtoolbar->p2; }
    ToolbarButton* sub3() { return subtoolbar->p3; }
    ToolbarButton* sub4() { return subtoolbar->p4; }
    ToolbarButton* sub5() { return subtoolbar->p5; }
    ToolbarButton* pen;
    ToolbarButton* line;
    ToolbarButton* lassofill;
    ToolbarButton* eraser;
    ToolbarButton* other;
    ToolbarButton* clearimage;
    ToolbarButton* knockback;

private:
    ToolbarTool currentTool = ToolbarTool::TOOL1;
    MainWindow* mainwindow;
    QWidget* parent;
    Subtoolbar* subtoolbar;
};




#endif
