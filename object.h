#ifndef OBJECT_H
#define OBJECT_H

#include <QMap>
#include <QString>
#include <QColor>
#include <QImage>
#include <QDebug>
#include <QWidget>
#include <QUndoStack>

class Editor;
class Timeline;
class Object: public QWidget
{
    Q_OBJECT

public:
    Object(QUndoStack* = nullptr, QWidget* = nullptr);
    void setEditor(Editor* e) { editor = e; }
    void setTimeline(Timeline* t) { timeline = t; }
    Editor* getEditor() { return editor; }
    Timeline* getTimeline() { return timeline; }

    QImage* getKeyframeImageAt(int pos) { return &keyframes[pos]; }
    void addKeyframeAt(int pos, QImage img = QImage(1, 1, QImage::Format_ARGB32)) { keyframes.insert(pos, img); }
    void removeKeyframeAt(int pos) { keyframes.remove(pos); }
    bool isKeyframe(int pos) { return keyframes.contains(pos); }
    int getFirstKeyframePos() { return keyframes.firstKey(); }
    int getLastKeyframePos() { return keyframes.lastKey(); }
    int getKeyframesCount() { return keyframes.size(); }
    QMap<int, QImage> getKeyframes() { return keyframes; }

    void foreachKeyframe(std::function<void(int)>, int = 0, int = 0);
    void foreachKeyframeRevert(std::function<void(int)>, int = 0, int = 0);
    int getPrevKeyframePos(int);
    int getNextKeyframePos(int);
    void resizeImage(int, int, int);

private:
    QMap<int, QImage> keyframes;
    Editor* editor;
    Timeline* timeline;
    QUndoStack* undoStack;

};

#endif
