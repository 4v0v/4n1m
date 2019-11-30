#include <QtWidgets>
#include "editor.h"

Editor::Editor(QWidget *parent): QWidget(parent)
{
    setCursor(Qt::CrossCursor);
    resizeImage(&keyframes[currentPosition], QSize(width(), height()), 1);
    update();
}

void Editor::clearImage()
{
    keyframes[currentPosition].fill(Qt::transparent);
    update();
}

void Editor::mousePressEvent(QMouseEvent *event)
{
    scribbling = true;
    if (currentTool == Tool::LASSOFILL) lassoFillPoly << QPoint(event->pos().x(), event->pos().y());
    if (currentTool == Tool::PEN) lastPoint = event->pos();

    update();
}

void Editor::mouseMoveEvent(QMouseEvent *event)
{
    if (!scribbling) return;
    if (currentTool == Tool::LASSOFILL) lassoFillPoly << QPoint(event->pos().x(), event->pos().y());
    if (currentTool == Tool::PEN) drawLineTo(event->pos());

    update();
}

void Editor::mouseReleaseEvent(QMouseEvent *event)
{
    scribbling = false;
    if (currentTool == Tool::LASSOFILL) drawLassoFill() ;
    if (currentTool == Tool::PEN) drawLineTo(event->pos());

    update();
}

void Editor::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, bgImage, dirtyRect);
    painter.drawImage(dirtyRect, keyframes[currentPosition], dirtyRect);

    // Preview of lassofill
    if (currentTool == Tool::LASSOFILL)
    {
        QBrush fillbrush;
        fillbrush.setStyle(lassoFillPattern);
        fillbrush.setColor(penColor);
        QPainterPath path;
        path.addPolygon(lassoFillPoly);
        painter.fillPath(path, fillbrush);
    }
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

    painter.setPen(QPen(penColor, myPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawLine(lastPoint, endPoint);
    int rad = (myPenWidth / 2) + 2;
    update(QRect(lastPoint, endPoint).normalized().adjusted(-rad, -rad, +rad, +rad));
    lastPoint = endPoint;
}


void Editor::drawLassoFill()
{
    QPainter p(&keyframes[currentPosition]);

    QBrush fillbrush;
    fillbrush.setStyle(lassoFillPattern);
    fillbrush.setColor(penColor);
    QPainterPath path;
    path.addPolygon(lassoFillPoly);
    p.fillPath(path, fillbrush);

    lassoFillPoly.clear();
}


void Editor::resizeImage(QImage *image, const QSize &newSize, int type)
{
    if (image->size() == newSize) return;

    QImage newImage(newSize, QImage::Format_ARGB32);
    if(type == 0)newImage.fill(bgColor);
    if(type == 1)newImage.fill(Qt::transparent);

    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}
