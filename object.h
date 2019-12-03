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
    QImage* getKeyframeAt(int pos) { return &keyframes[pos]; }
    void addKeyframeAt(int pos) { keyframes.insert(pos, QImage(1, 1, QImage::Format_ARGB32)); }
    bool isKeyframe(int pos) { return keyframes.contains(pos); }

    void drawPenStroke(int pos, QPen pen, QPolygon poly);
    void drawLassoFill(int pos, QBrush brush, QPolygon poly);
    void clearImage(int pos);
    void resizeImage(int pos, int width, int height);

private:
    QMap<int, QImage> keyframes;
};

#endif
