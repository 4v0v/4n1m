#include "object.h"
#include "editor.h"
#include "timeline.h"
#include "preview.h"
#include "commands.h"
#include "titlebar.h"
#include "menubar.h"

Object::Object(MainWindow* mainwindow): QWidget(mainwindow)
{
    parent = mainwindow;

    keyframes.insert(0, QMap<int, QImage>());
    keyframes.insert(1, QMap<int, QImage>());
    keyframes.insert(2, QMap<int, QImage>());
}

void Object::resizeImage(int layer, int pos, int width , int height)
{
    QSize newSize = QSize(width, height);
    if (keyframes[layer][pos].size() == newSize) return;
    QImage newImage(newSize, QImage::Format_ARGB32);
    newImage.fill(Qt::transparent);
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), keyframes[layer][pos]);
    keyframes[layer][pos] = newImage;
}

int Object::getPrevKeyframePos(int layer, int pos)
{
    int temp = keyframes[layer].firstKey();
    for (auto i = keyframes[layer].begin(); i != keyframes[layer].end(); ++i)
    {
        if (i.key() >= pos) return temp;
        temp = i.key();
    }
    return temp;
}

int Object::getNextKeyframePos(int layer, int pos)
{
    if (pos >= keyframes[layer].lastKey()) return keyframes[layer].lastKey();
    return keyframes[layer].upperBound(pos).key();
}

void Object::foreachKeyframe(int layer, std::function<void(int)> action, int begin, int end)
{
    if (end == 0) end = keyframes[layer].lastKey();
    if (begin > end) begin = keyframes[layer].lastKey();
    for (int i = begin; i < end + 1; ++i) if(isKeyframe(layer, i)) action(i);
}

void Object::foreachKeyframeRevert(int layer, std::function<void(int)> action, int begin, int end)
{
    if (end == 0) end = keyframes[layer].lastKey();
    if (begin > end) begin = keyframes[layer].lastKey();
    for (int i = end; i > begin - 1; --i) if(isKeyframe(layer, i)) action(i);
}

void Object::foreachLayer(std::function<void(int)> action, int begin, int end)
{
    if (end == 0) end = keyframes.lastKey();
    if (begin > end) begin = keyframes.lastKey();
    for (int i = begin; i < end + 1; ++i) if(isLayer(i)) action(i);
}

void Object::foreachLayerRevert(std::function<void(int)> action, int begin, int end)
{
    if (end == 0) end = keyframes.lastKey();
    if (begin > end) begin = keyframes.lastKey();
    for (int i = end; i > begin - 1; --i) if(isLayer(i)) action(i);
}
