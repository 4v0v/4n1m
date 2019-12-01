#include <QtWidgets>
#include "editor.h"

Editor::Editor(QWidget *parent): QWidget(parent)
{
    setCursor(Qt::CrossCursor);
    resizeImage(&keyframes[currentPosition], QSize(width(), height()), 1);
    update();
}

void Editor::mousePressEvent(QMouseEvent *event)
{
    scribbling = true;
    switch (currentTool)
    {
        case Tool::PEN: penLines << QPoint(event->pos().x(), event->pos().y()); break;
        case Tool::LASSOFILL: lassoFill << QPoint(event->pos().x(), event->pos().y()); break;
    }
    update();
}

void Editor::mouseReleaseEvent(QMouseEvent *event)
{
    scribbling = false;
    switch (currentTool)
    {
        case Tool::PEN: drawPenLines(); break;
        case Tool::LASSOFILL: drawLassoFill(); break;
    }
    update();
}

void Editor::mouseMoveEvent(QMouseEvent *event)
{
    if (!scribbling) return;
    switch (currentTool)
    {
        case Tool::PEN: penLines << QPoint(event->pos().x(), event->pos().y()); break;
        case Tool::LASSOFILL: lassoFill << QPoint(event->pos().x(), event->pos().y()); break;
    }
    update();
}

void Editor::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.drawImage(event->rect(), bgImage, event->rect());
    painter.drawImage(event->rect(), keyframes[currentPosition], event->rect());

    // Previews
    switch (currentTool) {
        case Tool::PEN:
        {
            painter.setPen(QPen(penColor, penWidth, penPattern, Qt::RoundCap, Qt::RoundJoin));
            painter.drawPolyline(penLines);
            break;
        }
        case Tool::LASSOFILL:
        {
            QPainterPath path;
            path.addPolygon(lassoFill);
            painter.fillPath(path, QBrush(penColor, lassoFillPattern));
            break;
        }
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

void Editor::drawPenLines()
{
    QPainter painter(&keyframes[currentPosition]);
    painter.setPen(QPen(penColor, penWidth, penPattern, Qt::RoundCap, Qt::RoundJoin));
    painter.drawPolyline(penLines);
    penLines.clear();
}

void Editor::drawLassoFill()
{
    QPainter painter(&keyframes[currentPosition]);
    QPainterPath path;
    path.addPolygon(lassoFill);
    painter.fillPath(path, QBrush(penColor, lassoFillPattern));
    lassoFill.clear();
}

void Editor::clearImage()
{
    keyframes[currentPosition].fill(Qt::transparent);
    update();
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
