#ifndef EDITOR_H
#define EDITOR_H

#include "mainwindow.h"
#include "mainWidgets/toolbar.h"

class Editor : public QWidget
{
    Q_OBJECT

public:
    Editor(MainWindow*);
    Timeline* timeline() { return mainwindow->timeline; }
    Animation* animation() { return mainwindow->animation; }
    Toolbar* toolbar() { return mainwindow->toolbar; }
    QUndoStack* undostack() { return mainwindow->undostack; }
    int getTool(){ return currentTool; }
    bool isScribbling() { return scribbling; }
    int getPos(int layer = -1);
    void setBackgroundColor(QColor &newColor){ backgroundColor = newColor; }
    QColor getBackgroundColor(){ return backgroundColor; }
    QPen* getPenTool() { return &penTool; }
    QPen* getShapeTool() { return &shapeTool; }
    QPen* getEraserTool() { return &eraserTool; }
    QBrush* getFillTool() { return &filltool; }
    int getKnockbackAmount() { return knockbackAmount; }
    void setKnockbackAmount(int k) { knockbackAmount = k; }
    void drawPenStroke();
    void drawEraserStroke();
    void drawFill();
    void drawShape();
    void drawSelect();
    Tool getCurrentTool() {return currentTool; }
    void changeTool(Tool t= EMPTY) { currentTool = t; }
    void drawOnionSkin(QPaintEvent*, QPainter*, QPainterPath*, double, int, int, QColor);

    MainWindow* mainwindow;
    Tool currentTool = PEN;
    Tool shapeSubtool = LINE;
    Tool fillSubtool = LASSO;
    bool scribbling = false;
    bool onionskinVisible = true;
    bool onionskinloopVisible = false;
    QPen penTool = QPen(QColor(0,0,0,255), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPen shapeTool = QPen(QColor(0,0,0,255), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPen eraserTool = QPen(Qt::blue, 30, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QBrush filltool = QBrush(QColor(0,0,0,255), Qt::SolidPattern);
    QColor backgroundColor = QColor(243,200,149);
    int knockbackAmount = 50;
    double layerOpacity = 0.6;
    double onionOpacityFirst = 0.3;
    double onionOpacitySecond = 0.1;
    double onionOpacityLoop = 0.2;
    QPolygon stroke;

    SelectionState selectState = STATE_EMPTY;
    SelectionState selectMode = EMPTY_MODE;
    Tool selectSubtool = RECTANGLE;
    QRect select;
    QRect dselect;
    QImage selectedImg = QImage(1, 1, QImage::Format_ARGB32);
    QImage internetExplorerChanImg = QImage(1, 1, QImage::Format_ARGB32);
    int dxselect;
    int dyselect;

public slots:
    void clearImage();
    void knockback();
    void toggleOnionskin() { onionskinVisible = !onionskinVisible; update(); }
    void toggleOnionskinloop() { onionskinloopVisible = !onionskinloopVisible; update(); }
    void setToolAsPen() { if (!scribbling) {toolbar()->setCurrentTool(TOOL1); changeTool(PEN); update();}}
    void setToolAsShape() { if (!scribbling) {toolbar()->setCurrentTool(TOOL2); changeTool(SHAPE); update();}}
    void setToolAsLassoFill() { if (!scribbling) {toolbar()->setCurrentTool(TOOL3); changeTool(FILL); update();}}
    void setToolAsEraser() { if (!scribbling) {toolbar()->setCurrentTool(TOOL4); changeTool(ERASER); update();}}
    void setToolAsSelect() { if (!scribbling) {toolbar()->setCurrentTool(TOOL6); changeTool(SELECT); update();}}
    void setToolAsEmpty() { if (!scribbling) {toolbar()->setCurrentTool(TOOL5); changeTool(EMPTY); update();}}

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void paintEvent(QPaintEvent*) override;
};

#endif
