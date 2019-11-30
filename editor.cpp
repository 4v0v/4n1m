#include <QtWidgets>
#include "editor.h"

Editor::Editor(QWidget *parent): QWidget(parent)
{
    resizeImage(&keyframes[currentPosition], QSize(width(), height()), 1);
    update();


}

void Editor::clearImage()
{
    keyframes[currentPosition].fill(kfColor);
    update();
}

void Editor::mousePressEvent(QMouseEvent *event)
{
    scribbling = true;
    poly << QPoint(event->pos().x(), event->pos().y());
    update();
}

void Editor::mouseMoveEvent(QMouseEvent *event)
{
    if (scribbling)
    {
        poly << QPoint(event->pos().x(), event->pos().y());
        update();
    }
}

void Editor::mouseReleaseEvent(QMouseEvent *event)
{
    clearPoly = true;
    scribbling = false;
    update();
}

void Editor::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    if (clearPoly)
    {
        QPainter p(&keyframes[currentPosition]);

        QBrush fillbrush;
        fillbrush.setStyle(Qt::SolidPattern);
        fillbrush.setColor(Qt::red);
        QPainterPath path;
        path.addPolygon(poly);
        p.fillPath(path, fillbrush);

        poly.clear();
        clearPoly = false;
    }

    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, bgImage, dirtyRect);
    painter.drawImage(dirtyRect, keyframes[currentPosition], dirtyRect);


    QBrush fillbrush;
    fillbrush.setStyle(Qt::SolidPattern);
    QPainterPath path;
    path.addPolygon(poly);
    painter.fillPath(path, fillbrush);
}

void Editor::resizeEvent(QResizeEvent *event)
{
    if (width() > bgImage.width() || height() > bgImage.height()) {
        int newWidth = qMax(width() + 128, bgImage.width());
        int newHeight = qMax(height() + 128, bgImage.height());
        resizeImage(&bgImage, QSize(newWidth, newHeight), 0);
        update();
    }

    QWidget::resizeEvent(event);
}

void Editor::drawLineTo(const QPoint &endPoint)
{
    QPainter painter(&keyframes[currentPosition]);

    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawLine(lastPoint, endPoint);
    int rad = (myPenWidth / 2) + 2;
    update(QRect(lastPoint, endPoint).normalized().adjusted(-rad, -rad, +rad, +rad));
    lastPoint = endPoint;
}


void Editor::resizeImage(QImage *image, const QSize &newSize, int type)
{
    if (image->size() == newSize) return;

    QImage newImage(newSize, QImage::Format_ARGB32);
    if(type == 0)newImage.fill(bgColor);
    if(type == 1)newImage.fill(kfColor);

    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}
