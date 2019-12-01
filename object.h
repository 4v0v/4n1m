#ifndef OBJECT_H
#define OBJECT_H

#include <QMap>
#include <QString>
#include <QColor>
#include <QImage>

class Object
{
public:
    Object();
    QImage* getKeyframeAt(int pos) { return &keyframes[pos]; }
    void drawPenStroke(int pos, QPen pen, QPolygon poly);
    void drawLassoFill(int pos, QBrush brush, QPolygon poly);
    void clearImage(int pos);
    void resizeImage(int pos, int width, int height);

private:
    QList<QImage> keyframes = QList<QImage>() << QImage(1, 1, QImage::Format_ARGB32);
};

#endif
