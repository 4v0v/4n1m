#ifndef TIMELINE_H
#define TIMELINE_H

#include <QWidget>

class Editor;
class Object;
class Timeline : public QWidget
{
    Q_OBJECT

public:
    Timeline(Object* = nullptr, QWidget* = nullptr);
    void setObject(Object* o) { object = o; }
    void setEditor(Editor* e) { editor = e; }

    int getPos() { return timelinePos; }

public slots:
    void gotoNextFrame();
    void gotoPrevFrame();
    void gotoFrame(int);
    void addKeyframe();
    void removeKeyframe();
    void insertFrame();
    void removeFrame();
    void copyFrame();
    void cutFrame();
    void pasteFrame();

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;

private:
    Object* object;
    Editor* editor;
    QImage clipboard = QImage(1, 1, QImage::Format_ARGB32);
    int timelinePos = 0;
};

#endif
