#include <QtWidgets>
#include "editor.h"

Editor::Editor(Object *o, QWidget *parent): QWidget(parent)
{
    setObject(o);
    setCursor(Qt::CrossCursor);
    object->resizeImage(currentKeyframePosition, width(), height());
    update();
}

void Editor::mousePressEvent(QMouseEvent *event)
{
    if (!object->isKeyframe(currentPosition))
    {
        object->addKeyframeAt(currentPosition);
//        qDebug() << object->isKeyframe(currentPosition);
        object->resizeImage(currentPosition, width(), height());
        currentKeyframePosition = currentPosition;
        update();
    }

    scribbling = true;
    switch (currentTool)
    {
        case Tool::PEN: penStroke << QPoint(event->pos().x(), event->pos().y()); break;
        case Tool::LASSOFILL: lassoFill << QPoint(event->pos().x(), event->pos().y()); break;
        case Tool::ERASER: eraserStroke << QPoint(event->pos().x(), event->pos().y()); break;
    }
    update();
}

void Editor::mouseReleaseEvent(QMouseEvent *event)
{
    scribbling = false;
    switch (currentTool)
    {
        case Tool::PEN:
            object->drawPenStroke(
                        currentKeyframePosition,
                        QPen(penColor, penWidth, penPattern, Qt::RoundCap, Qt::RoundJoin),
                        penStroke
                    );
            penStroke.clear();
            break;
        case Tool::LASSOFILL:
            object->drawLassoFill(
                        currentKeyframePosition,
                        QBrush(penColor, lassoFillPattern),
                        lassoFill
                    );
            lassoFill.clear();
            break;
        case Tool::ERASER:
            object->drawPenStroke(
                        currentKeyframePosition,
                        QPen(eraserColor, eraserWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin),
                        eraserStroke
                    );
            eraserStroke.clear();
            break;
    }
    update();
}

void Editor::mouseMoveEvent(QMouseEvent *event)
{
    if (!scribbling) return;
    switch (currentTool)
    {
        case Tool::PEN: penStroke << QPoint(event->pos().x(), event->pos().y()); break;
        case Tool::LASSOFILL: lassoFill << QPoint(event->pos().x(), event->pos().y()); break;
        case Tool::ERASER: eraserStroke << QPoint(event->pos().x(), event->pos().y()); break;
    }
    update();
}

void Editor::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.drawImage(event->rect(), bgImage, event->rect());
    painter.drawImage(event->rect(), *object->getKeyframeAt(currentKeyframePosition), event->rect());

    // Previews
    switch (currentTool) {
        case Tool::PEN:
        {
            painter.setPen(QPen(penColor, penWidth, penPattern, Qt::RoundCap, Qt::RoundJoin));
            painter.drawPolyline(penStroke);
            break;
        }
        case Tool::LASSOFILL:
        {
            QPainterPath path;
            path.addPolygon(lassoFill);
            painter.fillPath(path, QBrush(penColor, lassoFillPattern));
            break;
        }
        case Tool::ERASER:
        {
            painter.setPen(QPen(eraserColor, eraserWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter.drawPolyline(eraserStroke);
            break;
        }
    }
}

void Editor::resizeEvent(QResizeEvent *event)
{
    // Resize background
    if (width() > bgImage.width() || height() > bgImage.height()) {
        int newWidth = qMax(width() + 128, bgImage.width());
        int newHeight = qMax(height() + 128, bgImage.height());
        QSize newSize = QSize(newWidth, newHeight);
        if (bgImage.size() == newSize) return;
        QImage newImage(newSize, QImage::Format_ARGB32);
        newImage.fill(bgColor);
        QPainter painter(&newImage);
        painter.drawImage(QPoint(0, 0), bgImage);
        bgImage = newImage;

        update();
    }

    QWidget::resizeEvent(event);
}

void Editor::clearImage()
{
    if (scribbling) return;

    object->clearImage(currentKeyframePosition);
    update();
}

void Editor::gotoNextFrame()
{
    if (scribbling) return;

    currentPosition += 1;
    if (object->isKeyframe(currentPosition)) currentKeyframePosition = currentPosition;

    update();
}

void Editor::gotoPreviousFrame()
{
    if (scribbling) return;
    if (currentPosition == 0) return;

    currentPosition -= 1;
    if (object->isKeyframe(currentPosition)) currentKeyframePosition = currentPosition;

    update();
}

void Editor::gotoKeyframe(int pos)
{
    if (scribbling) return;

    currentPosition = pos;
}
