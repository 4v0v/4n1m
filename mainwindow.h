#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>

class Editor;
class Timeline;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void penColor();
    void penWidth();
    void selectTool();
    void selectFillStyle();

private:
    void createActions();
    void createMenus();

    Editor *editor;
    Timeline *timeline;
    QMenu *optionMenu;
    QAction *penColorAct;
    QAction *penWidthAct;
    QAction *toolAct;
    QAction *fillStyleAct;
    QAction *clearScreenAct;
};

#endif
