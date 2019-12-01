#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H

#include <QMap>
#include <QString>
#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QDebug>
#include "object.h"

class Object;
class Editor : public QWidget
{
    Q_OBJECT

public:
    enum Tool {
        PEN,
        LASSOFILL,
        ERASER
    };

    Editor(Object *o, QWidget *parent = nullptr);

    void setObject(Object* o) { object = o; }
    void setPenColor(const QColor &newColor){ penColor = newColor; }
    void setPenWidth(int newWidth){ penWidth = newWidth; }
    int getTool(){ return currentTool; }
    void setFillStyle(Qt::BrushStyle b){ lassoFillPattern = b; }
    int getFillStyle(){ return lassoFillPattern; }
    QColor getPenColor() const { return penColor; }
    int getPenWidth() const { return penWidth; }

public slots:
    void clearImage();
    void setToolAsPen() { if (!scribbling) currentTool = Tool::PEN; }
    void setToolAsLassoFill() { if (!scribbling) currentTool = Tool::LASSOFILL; }
    void setToolAsEraser() { if (!scribbling) currentTool = Tool::ERASER; }

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    Object* object;
    QImage bgImage;
    QRgb bgColor = qRgba(238, 198, 148, 255);
    int currentPosition = 0;

    bool scribbling = false;
    int currentTool = Tool::PEN;
    QPolygon lassoFill;
    QPolygon penStroke;
    QPolygon eraserStroke;
    Qt::BrushStyle lassoFillPattern = Qt::SolidPattern;
    Qt::PenStyle penPattern = Qt::SolidLine;
    QColor penColor = Qt::black;
    QColor eraserColor = bgColor;
    int penWidth = 3;
    int eraserWidth = 15;
};

#endif
