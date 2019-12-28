#include "animation.h"
#include "preview.h"
#include "commands.h"
#include "mainWidgets/editor.h"
#include "mainWidgets/timeline.h"
#include "mainWidgets/titlebar.h"
#include "mainWidgets/menubar.h"

Animation::Animation(MainWindow* mw): QWidget(mw)
{
    mainwindow = mw;

    layers.insert(0, QMap<int, QImage>());
    layers.insert(1, QMap<int, QImage>());
    layers.insert(2, QMap<int, QImage>());
}

void Animation::resizeImage(int l, int p, int w , int h)
{
    QSize newSize = QSize(w, h);
    if (layers[l][p].size() == newSize) return;
    QImage newImage(newSize, QImage::Format_ARGB32);
    newImage.fill(Qt::transparent);
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), layers[l][p]);
    layers[l][p] = newImage;
}

int Animation::getPrevKey(int l, int p)
{
    int temp = -1;
    for (auto i = layers[l].begin(); i != layers[l].end(); ++i)
    {
        if (i.key() >= p) return temp;
        temp = i.key();
    }
    return temp;
}

int Animation::getNextKey(int l, int p)
{
    if (p >= getLastKey(l)) return getLastKey(l);
    return layers[l].upperBound(p).key();
}

void Animation::foreachKey(int l, std::function<void(int)> action, int begin, int end)
{
    if (end == 0) end = getLastKey(l);
    if (begin > end) begin = getLastKey(l);
    for (int i = begin; i < end + 1; ++i) if(isKey(l, i)) action(i);
}

void Animation::foreachKeyRevert(int l, std::function<void(int)> action, int begin, int end)
{
    if (end == 0) end = getLastKey(l);
    if (begin > end) begin = getLastKey(l);
    for (int i = end; i > begin - 1; --i) if(isKey(l, i)) action(i);
}

void Animation::foreachLayer(std::function<void(int)> action, int begin, int end)
{
    if (end == 0) end = layers.lastKey();
    if (begin > end) begin = layers.lastKey();
    for (int i = begin; i < end + 1; ++i) if(isLayer(i)) action(i);
}

void Animation::foreachLayerRevert(std::function<void(int)> action, int begin, int end)
{
    if (end == 0) end = layers.lastKey();
    if (begin > end) begin = layers.lastKey();
    for (int i = end; i > begin - 1; --i) if(isLayer(i)) action(i);
}

int Animation::getAnimationLength()
{
    int animationLength = 0;
    foreachLayerRevert([this, &animationLength](int i){
        if (getKeyCount(i) == 0) return;
        animationLength = getLastKey(i) > animationLength ? getLastKey(i) : animationLength;
    });
    if (animationLength > 0) animationLength += 1;
    return animationLength;
}

void Animation::saveAnimation()
{
    for (int i = 0; i < getAnimationLength(); ++i) {
        QString filename = QString::fromUtf8(("img_" + std::to_string(i) + ".png").c_str());
        QImage img = QImage(editor()->width(), editor()->height(), QImage::Format_ARGB32);
        QPainter painter(&img);

        foreachLayerRevert([this, &i, &painter](int layer){
            if (getKeyCount(layer) == 0) return;

            if (isKey(layer, i)) painter.drawImage(QPoint(0, 0), copyImageAt(layer, i));
            else {
                if (getPrevKey(layer, i) == -1) return;
                painter.drawImage(QPoint(0, 0), copyImageAt(layer, getPrevKey(layer, i)));
            }
        });
        img.save(filename);
    }
}
