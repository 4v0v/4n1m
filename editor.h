#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H

#include <QMap>
#include <QString>
#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>

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
    void setPenWidth(int newWidth){ myPenWidth = newWidth; }
    void setTool(Tool tool){ currentTool = tool; }
    int getTool(){ return currentTool; }

    void setFillStyle(Qt::BrushStyle b){ lassoFillPattern = b; }
    int getFillStyle(){ return lassoFillPattern; }


    QColor getPenColor() const { return penColor; }
    int getPenWidth() const { return myPenWidth; }

public slots:
    void clearImage();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void drawLineTo(const QPoint &endPoint);
    void drawLassoFill();
    void resizeImage(QImage *image, const QSize &newSize, int type);

    QImage bgImage;
    QList<QImage> keyframes = QList<QImage>() << QImage();
    QRgb bgColor = qRgba(238, 198, 148, 255);
    QColor penColor = Qt::black;
    QPoint lastPoint;
    QPolygon lassoFillPoly;

    int currentTool = Tool::PEN;
    Qt::BrushStyle lassoFillPattern = Qt::SolidPattern;

    int myPenWidth = 3;
    int currentPosition = 0;
    bool scribbling = false;
};

#endif
