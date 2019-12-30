#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H

#include "mainwindow.h"

class Editor : public QWidget
{
    Q_OBJECT

public:
    Editor(MainWindow*);
    Timeline* timeline() { return mainwindow->getTimeline(); }
    Animation* animation() { return mainwindow->getAnimation(); }
    QUndoStack* undostack() { return mainwindow->getUndoStack(); }

    int getTool(){ return currentTool; }
    bool isScribbling() { return scribbling; }
    int getPos(int layer = -1);
    void setBackgroundColor(QColor &newColor){ backgroundColor = newColor; }
    QColor getBackgroundColor(){ return backgroundColor; }
    QPen* getLinePen() { return &linePen; }
    QPen* getEraserPen() { return &eraserPen; }
    QBrush* getLassoFillBrush() { return &lassoBrush; }
    int getKnockbackAmount() { return knockbackAmount; }
    void setKnockbackAmount(int k) { knockbackAmount = k; }
    void drawPenStroke();
    void drawLassoFill();
    void drawEraserStroke();
    void drawLine();
    void changeTool(Tool t= Tool::EMPTY) { if (t != Tool::EMPTY) currentTool = t; }

public slots:
    void clearImage();
    void knockback();
    void toggleOnionskin() { onionskinVisible = !onionskinVisible; update(); }
    void toggleOnionskinloop() { onionskinloopVisible = !onionskinloopVisible; update(); }
    void setToolAsPen() { if (!scribbling) changeTool(Tool::PEN); }
    void setToolAsLine() { if (!scribbling) changeTool(Tool::LINE); }
    void setToolAsLassoFill() { if (!scribbling) changeTool(Tool::LASSOFILL); }
    void setToolAsEraser() { if (!scribbling) changeTool(Tool::ERASER); }

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void paintEvent(QPaintEvent*) override;

private:
    void drawOnionSkin(QPaintEvent*, QPainter*, QPainterPath*, double, int, int, QColor);

    MainWindow* mainwindow;

    QColor backgroundColor = QColor(243,200,149);
    bool scribbling = false;
    bool onionskinVisible = true;
    bool onionskinloopVisible = false;
    int knockbackAmount = 50;
    double layerOpacity = 0.6;
    double onionOpacityFirst = 0.3;
    double onionOpacitySecond = 0.1;
    double onionOpacityLoop = 0.3;

    int currentTool = Tool::PEN;
    QPolygon stroke;
    QPen linePen = QPen(QColor(0,0,0,255), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPen eraserPen = QPen(Qt::blue, 30, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QBrush lassoBrush = QBrush(QColor(0,0,0,255), Qt::SolidPattern);
};

#endif
