#ifndef ANIMATION_H
#define ANIMATION_H

#include "mainwindow.h"

class Animation: public QWidget
{
    Q_OBJECT

public:
    Animation(MainWindow*);
    Timeline* timeline() { return mainwindow->timeline; }
    Editor* editor() { return mainwindow->editor; }
    QUndoStack* undostack() { return mainwindow->undostack; }

    QImage* getImageAt(int l, int p) { return &layers[l][p]; }
    QImage copyImageAt(int l, int p) { return layers[l][p].copy(); }
    void resizeImage(int l, int p, int w, int h);
    void addKeyAt(int l, int p, QImage i = QImage(1, 1, QImage::Format_ARGB32)) { layers[l].insert(p, i); }
    void removeKeyAt(int l, int p) { layers[l].remove(p); }
    bool isKey(int l, int p) { return layers[l].contains(p); }
    int getFirstKey(int l) { return layers[l].firstKey(); }
    int getLastKey(int l) { return layers[l].lastKey(); }
    int getPrevKey(int l, int p);
    int getNextKey(int l, int p);
    int getKeyCount(int l) { return layers[l].count(); }
    void foreachKey(int l, std::function<void(int)>, int = 0, int = 0);
    void foreachKeyRevert(int l, std::function<void(int)>, int = 0, int = 0);
    bool isLayer(int l) { return layers.contains(l); }
    int getFirstLayerPos() { return layers.firstKey(); }
    int getLastLayerPos() { return layers.lastKey(); }
    void foreachLayer(std::function<void(int)>, int = 0, int = 0);
    void foreachLayerRevert(std::function<void(int)>, int = 0, int = 0);
    int getAnimationLength();

    QRect animSize = QRect(0, 0, 800, 600);
    MainWindow* mainwindow;
    QMap<int, QMap<int, QImage>> layers;

public slots:
    void saveAnimation();

};

#endif
