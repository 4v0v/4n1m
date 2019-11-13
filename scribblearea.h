#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H

#include <QMap>
#include <QString>
#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>

class ScribbleArea : public QWidget
{
    Q_OBJECT

public:
    ScribbleArea(QWidget *parent = nullptr);

    void setPenColor(const QColor &newColor){ myPenColor = newColor; }
    void setPenWidth(int newWidth){ myPenWidth = newWidth; }
    bool isModified() const { return modified; }
    QColor getPenColor() const { return myPenColor; }
    int getPenWidth() const { return myPenWidth; }

    bool openImage(const QString &fileName);
    bool saveImage(const QString &fileName, const char *fileFormat);

public slots:
    void clearImage();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QMap<QString, int> backgroundColor {
        {"red", 238},
        {"green", 198},
        {"blue", 148}
    };

    QColor myPenColor = Qt::black;
    int myPenWidth = 3;
    bool modified = false;
    bool scribbling = false;

    QImage image;
    QPoint lastPoint;

    void drawLineTo(const QPoint &endPoint);
    void resizeImage(QImage *image, const QSize &newSize);
};

#endif
