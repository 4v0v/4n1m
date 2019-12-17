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
    Editor* getEditor() { return editor; }
    void setEditor(Editor* e) { editor = e; }
    int getPos() { return timelinePos; }
    void setPos(int i) { timelinePos = i; }
    int getLayer() { return timelineLayer; }
    void setLayer(int i) { timelineLayer = i; }

public slots:
    void gotoNextFrame();
    void gotoPrevFrame();
    void gotoNextLayer();
    void gotoPrevLayer();
    void gotoFrame(int layer, int pos);
    void addKeyframe();
    void removeKeyframe();
    void insertFrame();
    void removeFrame();
    void copyFrame();
    void cutFrame();
    void pasteFrame();

protected:
    void paintEvent(QPaintEvent*) override;

private:
    Object* object;
    Editor* editor;
    QUndoStack* undoStack;
    QImage clipboard = QImage(1, 1, QImage::Format_ARGB32);
    int timelinePos = 0;
    int timelineLayer = 0;
};

#endif
