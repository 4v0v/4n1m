#ifndef EDITOR_H
#define EDITOR_H

#include "mainwindow.h"

class Editor : public QWidget
{
    Q_OBJECT

public:
    Editor(MainWindow*);
    Timeline* timeline() { return mainwindow->timeline; }
    Animation* animation() { return mainwindow->animation; }
    QUndoStack* undostack() { return mainwindow->undostack; }
    int getTool(){ return currentTool; }
    bool isScribbling() { return scribbling; }
    int getPos(int layer = -1);
    void setBackgroundColor(QColor &newColor){ backgroundColor = newColor; }
    QColor getBackgroundColor(){ return backgroundColor; }
    QPen* getPenTool() { return &penTool; }
    QPen* getLineTool() { return &lineTool; }
    QPen* getEraserTool() { return &eraserTool; }
    QBrush* getLassoFillTool() { return &lassoFilltool; }
    int getKnockbackAmount() { return knockbackAmount; }
    void setKnockbackAmount(int k) { knockbackAmount = k; }
    void drawPenStroke();
    void drawLassoFill();
    void drawEraserStroke();
    void drawLine();
    void drawSelect();
    Tool getCurrentTool() {return currentTool; }
    void changeTool(Tool t= Tool::EMPTY) { currentTool = t; }
    void drawOnionSkin(QPaintEvent*, QPainter*, QPainterPath*, double, int, int, QColor);

    MainWindow* mainwindow;
    Tool currentTool = Tool::PEN;
    bool scribbling = false;
    bool onionskinVisible = true;
    bool onionskinloopVisible = false;
    QPen penTool = QPen(QColor(0,0,0,255), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPen lineTool = QPen(QColor(0,0,0,255), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPen eraserTool = QPen(Qt::blue, 30, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QBrush lassoFilltool = QBrush(QColor(0,0,0,255), Qt::SolidPattern);
    QColor backgroundColor = QColor(243,200,149);
    int knockbackAmount = 50;
    double layerOpacity = 0.6;
    double onionOpacityFirst = 0.3;
    double onionOpacitySecond = 0.1;
    double onionOpacityLoop = 0.2;
    QPolygon stroke;

    SelectionState selectState = STATE_EMPTY;
    QRect select;
    QRect dselect;
    QImage selectedImg = QImage(1, 1, QImage::Format_ARGB32);
    int dx;
    int dy;

public slots:
    void clearImage();
    void knockback();
    void toggleOnionskin() { onionskinVisible = !onionskinVisible; update(); }
    void toggleOnionskinloop() { onionskinloopVisible = !onionskinloopVisible; update(); }
    void setToolAsPen() { if (!scribbling) changeTool(Tool::PEN); }
    void setToolAsLine() { if (!scribbling) changeTool(Tool::LINE); }
    void setToolAsLassoFill() { if (!scribbling) changeTool(Tool::LASSOFILL); }
    void setToolAsEraser() { if (!scribbling) changeTool(Tool::ERASER); }
    void setToolAsSelect() { if (!scribbling) changeTool(Tool::SELECT); }
    void setToolAsEmpty() { if (!scribbling) changeTool(Tool::EMPTY); }

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void paintEvent(QPaintEvent*) override;
};

#endif
