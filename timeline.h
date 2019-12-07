#ifndef TIMELINE_H
#define TIMELINE_H

#include <QWidget>

class Editor;
class Object;
class Timeline : public QWidget
{
    Q_OBJECT

public:
    Timeline(Object *o = nullptr, QWidget *parent = nullptr);

    void setObject(Object* o) { object = o; }
    void setEditor(Editor* e) { editor = e; }

    int getPos() { return timelinePos; }

public slots:
    void gotoNextFrame();
    void gotoPrevFrame();
    void gotoFrame(int pos);
    void addKeyframe();
    void removeKeyframe();
    void insertFrame();
    void removeFrame();
    void copyFrame();
    void pasteFrame();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    Object* object;
    Editor* editor;

    QImage clipboard = QImage(1, 1, QImage::Format_ARGB32);
    int timelinePos = 0;
};

#endif
