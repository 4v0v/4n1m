#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>

class Editor;
class Timeline;
class Object;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();


private slots:
    void penColor();
    void penWidth();
    void fillStyle();

private:
    void createActions();
    void createMenus();

    Editor *editor;
    Timeline *timeline;
    Object *object;

    QMenu *optionMenu;
    QAction *penColorAct;
    QAction *penWidthAct;
    QAction *toolAsPenAct;
    QAction *toolAsLassoFillAct;
    QAction *toolAsEraserAct;
    QAction *fillStyleAct;
    QAction *clearScreenAct;
};

#endif
