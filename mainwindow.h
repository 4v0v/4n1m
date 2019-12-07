#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>

class Editor;
class Timeline;
class Object;
class Preview;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void openPenColorWindow();
    void openPenWidthWindow();
    void openFillStyleWindow();
    void openPreviewWindow();

private:
    Editor *editor;
    Timeline *timeline;
    Object *object;
    Preview *preview;
};

#endif
