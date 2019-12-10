#include <QtWidgets>
#include "object.h"
#include "timeline.h"
#include "editor.h"

Object::Object(QUndoStack* u, QWidget* parent): QWidget(parent)
{
    undoStack = u;
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
