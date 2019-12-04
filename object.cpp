#include <QtWidgets>
#include "object.h"

Object::Object()
{
    addKeyframeAt(0);
}

void Object::resizeImage(int pos, int width , int height)
{
    QSize newSize = QSize(width, height);
    if (keyframes[pos].size() == newSize) return;
    QImage newImage(newSize, QImage::Format_ARGB32);
    newImage.fill(Qt::transparent);
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), keyframes[pos]);
    keyframes[pos] = newImage;
}

void Object::drawPenStroke(int pos, QPen pen, QPolygon poly)
{
    QPainter painter(&keyframes[pos]);
    painter.setPen(pen);
    painter.drawPolyline(poly);
}

void Object::drawLassoFill(int pos, QBrush brush, QPolygon poly)
{
    QPainter painter(&keyframes[pos]);
    QPainterPath path;
    path.addPolygon(poly);
    painter.fillPath(path, brush);
}

void Object::clearImage(int pos)
{
    keyframes[pos].fill(Qt::transparent);
}

QList<int> Object::getKeyframePoss(int begin, int end = -1)
{
    QList<int> kfpositions;
    if (end == -1) end = keyframes.lastKey();
    for (int i = begin; i < end; ++i)
        if (isKeyframe(i)) kfpositions.push_back(i);
    return kfpositions;
}

void Object::moveKeyframesTo(QList<int> kfpositions, int pos)
{

}

int Object::getPrevKeyframePos(int pos)
{
    int temp = keyframes.begin().key();
    for (auto i = keyframes.begin(); i != keyframes.end(); ++i)
    {
        if (i.key() >= pos) return temp;
        temp = i.key();
    }
    return temp;
}
