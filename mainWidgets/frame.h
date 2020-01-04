#ifndef FRAME_H
#define FRAME_H

#include "mainwindow.h"

class Frame : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int currentColorAlphaAnim READ getAlphaCurrentColor WRITE setAlphaCurrentColor)
    Q_PROPERTY(int keyColorAlphaAnim READ getAlphaKeyColor WRITE setAlphaKeyColor)

public:
    Frame(MainWindow*, Layer*, int);
    Timeline* timeline() { return mainwindow->timeline; }
    Editor* editor() { return mainwindow->editor; }
    Animation* animation() { return mainwindow->animation; }
    QUndoStack* undostack() { return mainwindow->undostack; }
    Layer* getLayer() { return layer;}
    int getPos() { return framePos; }

    void toggleIsKey() { isKey = !isKey; animateKey(); }
    void toggleIsKeyNoAnim() { 
        isKey = !isKey; 
        keyColor = isKey ? QColor(0, 0, 0, 255): QColor(255, 255, 255, 255);
        update();
    }
    void animateKey();
    void toggleIsCurrent();
    int getAlphaCurrentColor() { return currentColor.alpha(); }
    void setAlphaCurrentColor(int a) { currentColor.setAlpha(a); update();}

    int getAlphaKeyColor() { return keyColor.red(); }
    void setAlphaKeyColor(int a) { keyColor.setRgb(a, a ,a); update();}

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseDoubleClickEvent(QMouseEvent*) override;

private:
    MainWindow* mainwindow;
    Layer* layer;
    int framePos;

    QColor currentColor = QColor(255, 0, 0, 0);
    QPropertyAnimation* currentColorAnim = new QPropertyAnimation(this, "currentColorAlphaAnim");

    QColor keyColor = QColor(255, 255, 255, 255);
    QPropertyAnimation* keyColorAnim = new QPropertyAnimation(this, "keyColorAlphaAnim");

    bool isKey = false;
    bool isCurrent = false;
};

#endif
