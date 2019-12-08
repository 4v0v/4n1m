#include <QtWidgets>
#include "editor.h"
#include "timeline.h"
#include "object.h"

Editor::Editor(Object *o, QWidget *parent): QWidget(parent)
{
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
    }

    setObject(o);
    setCursor(Qt::CrossCursor);
    object->resizeImage(object->getPos(), width(), height());
    update();
}

void Editor::mousePressEvent(QMouseEvent *event)
{
    if (!object->isKeyframe(timeline->getPos())) timeline->addKeyframe();

    scribbling = true;
    switch (currentTool)
    {
        case Tool::PEN: penStroke << QPoint(event->pos().x(), event->pos().y()); break;
        case Tool::LASSOFILL: lassoFill << QPoint(event->pos().x(), event->pos().y()); break;
        case Tool::ERASER: eraserStroke << QPoint(event->pos().x(), event->pos().y()); break;
    }
    update();
}

void Editor::mouseReleaseEvent(QMouseEvent*)
{

    scribbling = false;
    switch (currentTool)
    {
        case Tool::PEN:
            object->drawPenStroke(
                        object->getPos(),
                        QPen(penColor, penWidth, penPattern, Qt::RoundCap, Qt::RoundJoin),
                        penStroke
                    );
            penStroke.clear();
            break;
        case Tool::LASSOFILL:
            object->drawLassoFill(
                        object->getPos(),
                        QBrush(penColor, lassoFillPattern),
                        lassoFill
                    );
            lassoFill.clear();
            break;
        case Tool::ERASER:
            object->drawPenStroke(
                        object->getPos(),
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

    // Background
    painter.drawImage(event->rect(), bgImage, event->rect());

    // Onionskin
    if (isOnionskinVisible)
    {
        int prev = object->getPrevKeyframePos(object->getPos());
        int next = object->getNextKeyframePos(object->getPos());
        QPainterPath path;
        path.addRect(0, 0, width(), height());

        if (prev < object->getPos())
        {
            painter.setOpacity(0.2);
            QImage img = *object->getKeyframeImageAt(prev);
            QPainter p(&img);
            p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            p.fillPath(path, Qt::red);
            painter.drawImage(event->rect(), img, event->rect());
        }
        if (object->getPrevKeyframePos(prev) < object->getPos())
        {
            painter.setOpacity(0.1);
            QImage img = *object->getKeyframeImageAt(object->getPrevKeyframePos(prev));
            QPainter p(&img);
            p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            p.fillPath(path, Qt::red);
            painter.drawImage(event->rect(), img, event->rect());
        }
        if (next > object->getPos())
        {
            painter.setOpacity(0.2);
            QImage img = *object->getKeyframeImageAt(next);
            QPainter p(&img);
            p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            p.fillPath(path, Qt::blue);
            painter.drawImage(event->rect(), img, event->rect());
        }
        if (object->getNextKeyframePos(next) > object->getPos())
        {
            painter.setOpacity(0.1);
            QImage img = *object->getKeyframeImageAt(object->getNextKeyframePos(next));
            QPainter p(&img);
            p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            p.fillPath(path, Qt::blue);
            painter.drawImage(event->rect(), img, event->rect());
        }
        if (object->getPos() == object->getFirstKeyframePos())
        {
            painter.setOpacity(0.3);
            QImage img = *object->getKeyframeImageAt(object->getLastKeyframePos());
            QPainter p(&img);
            p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            p.fillPath(path, Qt::darkGreen);
            painter.drawImage(event->rect(), img, event->rect());
        }
        if (object->getPos() == object->getLastKeyframePos())
        {
            painter.setOpacity(0.3);
            QImage img = *object->getKeyframeImageAt(object->getFirstKeyframePos());
            QPainter p(&img);
            p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            p.fillPath(path, Qt::darkGreen);
            painter.drawImage(event->rect(), img, event->rect());
        }

        painter.setOpacity(1.00);
    }

    // Current image
    painter.drawImage(event->rect(), *object->getKeyframeImageAt(object->getPos()), event->rect());

    // Tool previews
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

void Editor::resizeEvent(QResizeEvent*){}

void Editor::clearImage()
{
    if (scribbling) return;
    object->clearImage(object->getPos());
    update();
}

void Editor::toggleOnionskin()
{
    isOnionskinVisible = !isOnionskinVisible;
    update();
}
