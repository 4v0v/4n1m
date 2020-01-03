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
    ToolbarButton* subtool1;
    ToolbarButton* subtool2;
    ToolbarButton* subtool3;
    ToolbarButton* subtool4;
    ToolbarButton* subtool5;

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
    Editor* editor() {return mainwindow->getEditor(); }
    ToolbarTool getCurrentTool() { return currentTool; }
    void setCurrentTool(ToolbarTool t);
    ToolbarButton* sub1() { return subtoolbar->subtool1; }
    ToolbarButton* sub2() { return subtoolbar->subtool2; }
    ToolbarButton* sub3() { return subtoolbar->subtool3; }
    ToolbarButton* sub4() { return subtoolbar->subtool4; }
    ToolbarButton* sub5() { return subtoolbar->subtool5; }
    ToolbarButton* tool1;
    ToolbarButton* tool2;
    ToolbarButton* tool3;
    ToolbarButton* tool4;
    ToolbarButton* tool5;
    ToolbarButton* tool6;
    ToolbarButton* tool7;

private:
    ToolbarTool currentTool = ToolbarTool::TOOL1;
    MainWindow* mainwindow;
    QWidget* parent;
    Subtoolbar* subtoolbar;
};

#endif
