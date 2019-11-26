#include <QtWidgets>
#include "editor.h"

Editor::Editor(QWidget *parent): QWidget(parent)
{
    resizeImage(&keyframes[currentPosition], QSize(1, 1), 1);
    update();
}

void Editor::clearImage()
{
    keyframes[currentPosition].fill(kfColor);
    resizeImage(&keyframes[currentPosition], QSize(1, 1), 1);
    update();
}

void Editor::mousePressEvent(QMouseEvent *event)
{
    if (!(event->pos().x() > width() || event->pos().y() > height()))
    {
        if (event->pos().x() > keyframes[currentPosition].width())
            resizeImage(&keyframes[currentPosition], QSize(event->pos().x() + 50, keyframes[currentPosition].height()), 1);
        if (event->pos().y() > keyframes[currentPosition].height())
            resizeImage(&keyframes[currentPosition], QSize(keyframes[currentPosition].width(), event->pos().y() + 50), 1);
        update();
    }

    if (event->button() == Qt::LeftButton) {
        lastPoint = event->pos();
        scribbling = true;
    }
}

void Editor::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->pos().x() > width() || event->pos().y() > height()))
    {
        if (event->pos().x() > keyframes[currentPosition].width())
            resizeImage(&keyframes[currentPosition], QSize(event->pos().x() + 50, keyframes[currentPosition].height()), 1);
        if (event->pos().y() > keyframes[currentPosition].height())
            resizeImage(&keyframes[currentPosition], QSize(keyframes[currentPosition].width(), event->pos().y() + 50), 1);
        update();
    }

    if ((event->buttons() & Qt::LeftButton) && scribbling)
        drawLineTo(event->pos());
}

void Editor::mouseReleaseEvent(QMouseEvent *event)
{
    if (!(event->pos().x() > width() || event->pos().y() > height()))
    {
        if (event->pos().x() > keyframes[currentPosition].width())
            resizeImage(&keyframes[currentPosition], QSize(event->pos().x() + 50, keyframes[currentPosition].height()), 1);
        if (event->pos().y() > keyframes[currentPosition].height())
            resizeImage(&keyframes[currentPosition], QSize(keyframes[currentPosition].width(), event->pos().y() + 50), 1);
        update();
    }

    if (event->button() == Qt::LeftButton && scribbling) {
        drawLineTo(event->pos());
        scribbling = false;
    }
}

void Editor::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    QRect dirtyRect = event->rect();

    painter.drawImage(dirtyRect, bgImage, dirtyRect);
    painter.drawImage(dirtyRect, keyframes[currentPosition], dirtyRect);
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
