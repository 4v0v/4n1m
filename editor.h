#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H

#include <QMap>
#include <QString>
#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QDebug>
#include <QUndoStack>
#include <QPainter>
#include "mainwindow.h"

class Editor : public QWidget
{
    Q_OBJECT

public:
    Editor(MainWindow*);
    Timeline* timeline() { return parent->getTimeline(); }
    Object* object() { return parent->getObject(); }
    QUndoStack* undostack() { return parent->getUndoStack(); }

    int getTool(){ return currentTool; }
    bool isScribbling() { return scribbling; }
    int getPos(int layer = -1);
    void setBackgroundColor(QColor &newColor){ backgroundColor = newColor; }
    QColor getBackgroundColor(){ return backgroundColor; }
    QPen* getLinePen() { return &linePen; }
    QBrush* getLassoFillBrush() { return &lassoBrush; }
    void drawPenStroke();
    void drawLassoFill();
    void drawEraserStroke();
    void drawLine();

public slots:
    void clearImage();
    void toggleOnionskin();
    void toggleLayerTransparency();
    void setToolAsPen() { if (!scribbling) currentTool = Tool::PEN; }
    void setToolAsLine() { if (!scribbling) currentTool = Tool::LINE; }
    void setToolAsLassoFill() { if (!scribbling) currentTool = Tool::LASSOFILL; }
    void setToolAsEraser() { if (!scribbling) currentTool = Tool::ERASER; }

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void paintEvent(QPaintEvent*) override;

private:
    MainWindow* parent;

    QColor backgroundColor = QColor(243, 200, 149, 255);
    bool scribbling = false;
    bool onionskinVisible = true;
    bool layerTransparencyVisible = true;
    double layerTransparency = 0.6;

    int currentTool = Tool::PEN;
    QPolygon stroke;
    QPen linePen = QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPen eraserPen = QPen(Qt::blue, 15, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QBrush lassoBrush = QBrush(Qt::black, Qt::SolidPattern);
};

#endif
