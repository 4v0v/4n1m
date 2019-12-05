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
    
    void foreachKeyframe(std::function<void(int)>, int begin = 0, int end = 0);
    void foreachKeyframeRevert(std::function<void(int)>, int begin = 0, int end = 0);

    bool isKeyframe(int pos) { return keyframes.contains(pos); }
    int getPrevKeyframePos(int pos);
    int getNextKeyframePos(int pos);
    int getPos() { return position; }
    int getFirstKeyframePos() { return keyframes.firstKey(); }
    int getLastKeyframePos() { return keyframes.lastKey(); }
    void setPos(int pos) { position = pos; }

    void drawPenStroke(int pos, QPen pen, QPolygon poly);
    void drawLassoFill(int pos, QBrush brush, QPolygon poly);
    void clearImage(int pos);
    void resizeImage(int pos, int width, int height);

private:
    QMap<int, QImage> keyframes;
    int position = 0;
};

#endif
