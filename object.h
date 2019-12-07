#ifndef OBJECT_H
#define OBJECT_H

#include <QMap>
#include <QString>
#include <QColor>
#include <QImage>
#include <QDebug>

class Object
{
public:
    Object();
    QImage* getKeyframeImageAt(int pos) { return &keyframes[pos]; }
    void addKeyframeAt(int pos, QImage img = QImage(1, 1, QImage::Format_ARGB32)) { keyframes.insert(pos, img); }
    void removeKeyframeAt(int pos) { keyframes.remove(pos); }
    void setPos(int pos) { position = pos; }
    bool isKeyframe(int pos) { return keyframes.contains(pos); }
    int getPos() { return position; }
    int getFirstKeyframePos() { return keyframes.firstKey(); }
    int getLastKeyframePos() { return keyframes.lastKey(); }

    void foreachKeyframe(std::function<void(int)>, int = 0, int = 0);
    void foreachKeyframeRevert(std::function<void(int)>, int = 0, int = 0);
    int getPrevKeyframePos(int);
    int getNextKeyframePos(int);
    void drawPenStroke(int, QPen, QPolygon);
    void drawLassoFill(int, QBrush, QPolygon);
    void clearImage(int);
    void resizeImage(int, int, int);

private:
    QMap<int, QImage> keyframes;
    int position = 0;
};

#endif
