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

int Object::getPrevKeyframePos(int pos)
{
    int temp = keyframes.firstKey();
    for (auto i = keyframes.begin(); i != keyframes.end(); ++i)
    {
        if (i.key() >= pos) return temp;
        temp = i.key();
    }
    return temp;
}

int Object::getNextKeyframePos(int pos)
{
    if (pos >= keyframes.lastKey()) return keyframes.lastKey();
    return keyframes.upperBound(pos).key();
}

void Object::foreachKeyframe(std::function<void(int)> action, int begin, int end)
{
    if (end == 0) end = keyframes.lastKey();
    if (begin > end) begin = keyframes.lastKey();

    for (int i = begin; i < end + 1; ++i)
    {
        if(isKeyframe(i)) action(i);
    }
}

void Object::foreachKeyframeRevert(std::function<void(int)> action, int begin, int end)
{
    if (end == 0) end = keyframes.lastKey();
    if (begin > end) begin = keyframes.lastKey();

    for (int i = end; i > begin - 1; --i)
    {
        if(isKeyframe(i)) action(i);
    }
}
