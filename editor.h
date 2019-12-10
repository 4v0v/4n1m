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

class Timeline;
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

    Editor(Object* = nullptr, QUndoStack* = nullptr, QWidget* = nullptr);
    void setTimeline(Timeline* t) { timeline = t; }
    Timeline* getTimeline() { return timeline; }
    int getTool(){ return currentTool; }
    bool isScribbling() { return scribbling; }
    int getPos() { return editorPosition; }
    void setPos(int i) { editorPosition = i; }
    void setBackgroundColor(QColor &newColor){ backgroundColor = newColor; }
    QColor getBackgroundColor(){ return backgroundColor; }
    QPen* getLinePen() { return &linePen; }
    QBrush* getLassoFillBrush() { return &lassoBrush; }

public slots:
    void toggleOnionskin();
    void setToolAsPen() { if (!scribbling) currentTool = Tool::PEN; }
    void setToolAsLassoFill() { if (!scribbling) currentTool = Tool::LASSOFILL; }
    void setToolAsEraser() { if (!scribbling) currentTool = Tool::ERASER; }

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void paintEvent(QPaintEvent*) override;

private:
    void drawPenStroke();
    void drawLassoFill();
    void clearImage();

    Object* object;
    Timeline* timeline;
    QUndoStack* undoStack;

    int editorPosition = 0;
    bool scribbling = false;
    bool onionskinVisible = true;
    int currentTool = Tool::PEN;
    QPolygon lassoFill;
    QPolygon penStroke;
    QPolygon eraserStroke;
    QColor backgroundColor = qRgba(238, 198, 148, 255);

    QPen linePen = QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPen eraserPen = QPen(Qt::blue, 15, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QBrush lassoBrush = QBrush(Qt::black, Qt::SolidPattern);
};

#endif
