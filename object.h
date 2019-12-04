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
    QList<int> getKeyframePoss (int begin, int end);
    void addKeyframeAt(int pos) { keyframes.insert(pos, QImage(1, 1, QImage::Format_ARGB32)); }
    void removeKeyframeAt(int pos) { keyframes.remove(pos); }
    void moveKeyframesTo(QList<int> kfpositions, int pos);

    bool isKeyframe(int pos) { return keyframes.contains(pos); }
    int getPrevKeyframePos(int pos);
    int getPos() { return position; }
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
