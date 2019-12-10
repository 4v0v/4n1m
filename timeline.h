#ifndef TIMELINE_H
#define TIMELINE_H

#include <QWidget>
#include <QUndoStack>

class Editor;
class Object;
class Timeline : public QWidget
{
    Q_OBJECT

public:
    Timeline(Object* = nullptr, QUndoStack* = nullptr, QWidget* = nullptr);
    Object* getObject() { return object; }
    void setEditor(Editor* e) { editor = e; }
    Editor* getEditor() { return editor; }
    int getPos() { return timelinePos; }
    void setPos(int i) { timelinePos = i; }

signals:
    void changeFrameUndo(int, int, Timeline*);
    void addImageUndo(QImage, int, Object*);
    void removeImageUndo(QImage, int, Object*);

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
    QUndoStack* undoStack;
    QImage clipboard = QImage(1, 1, QImage::Format_ARGB32);
    int timelinePos = 0;
};

#endif
