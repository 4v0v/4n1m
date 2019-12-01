#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H

#include <QMap>
#include <QString>
#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QDebug>

class Editor : public QWidget
{
    Q_OBJECT

public:
    enum Tool {
        PEN,
        LASSOFILL,
    };

    Editor(QWidget *parent = nullptr);
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

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void drawLassoFill();
    void drawPenLines();
    void resizeImage(QImage *image, const QSize &newSize, int type);

    QImage bgImage;
    QRgb bgColor = qRgba(238, 198, 148, 255);
    int currentPosition = 0;
    QList<QImage> keyframes = QList<QImage>() << QImage();

    bool scribbling = false;
    int currentTool = Tool::PEN;
    QPolygon lassoFill;
    QPolygon penLines;
    Qt::BrushStyle lassoFillPattern = Qt::SolidPattern;
    Qt::PenStyle penPattern = Qt::SolidLine;
    QColor penColor = Qt::black;
    int penWidth = 3;
};

#endif
