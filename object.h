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

    QImage* getKeyframeImageAt(int layer, int pos) { return &keyframes[layer][pos]; }
    void addKeyframeAt(int layer, int pos, QImage img = QImage(1, 1, QImage::Format_ARGB32)) { keyframes[layer].insert(pos, img); }
    void removeKeyframeAt(int layer, int pos) { keyframes[layer].remove(pos); }
    bool isKeyframe(int layer, int pos) { return keyframes[layer].contains(pos); }
    bool isLayer(int layer) { return keyframes.contains(layer); }
    int getFirstKeyframePos(int layer) { return keyframes[layer].firstKey(); }
    int getLastKeyframePos(int layer) { return keyframes[layer].lastKey(); }
    int getKeyframesCount(int layer) { return keyframes[layer].size(); }
    int getFirstLayerPos() { return keyframes.firstKey(); }
    int getLastLayerPos() { return keyframes.lastKey(); }
    void foreachKeyframe(int layer, std::function<void(int)>, int = 0, int = 0);
    void foreachKeyframeRevert(int layer, std::function<void(int)>, int = 0, int = 0);
    int getPrevKeyframePos(int layer, int pos);
    int getNextKeyframePos(int layer, int pos);
    void resizeImage(int layer, int pos, int width, int height);
    void foreachLayer(std::function<void(int)>, int = 0, int = 0);
    void foreachLayerRevert(std::function<void(int)>, int = 0, int = 0);
    
private:
    QMap<int, QMap<int, QImage>> keyframes;
    Editor* editor;
    Timeline* timeline;
    QUndoStack* undoStack;

};

#endif
