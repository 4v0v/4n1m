#include <QtWidgets>
#include "editor.h"
#include "timeline.h"
#include "object.h"

Editor::Editor(Object *o, QWidget *parent): QWidget(parent)
{
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
    painter.drawImage(event->rect(), bgImage, event->rect());

    // Prev onion
    if (object->getPrevKeyframePos(object->getPos()) < object->getPos())
    {
        painter.setOpacity(0.2);
        int prev = object->getPrevKeyframePos(object->getPos());
        QImage img = *object->getKeyframeImageAt(prev);
        QPainter p(&img);
        QPainterPath path;
        p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        path.addRect(0, 0, width(), height());
        p.fillPath(path, Qt::red);
        painter.drawImage(event->rect(), img, event->rect());
        painter.setOpacity(1.00);
    }

    // Next onion
    if (object->getNextKeyframePos(object->getPos()) > object->getPos())
    {
        painter.setOpacity(0.2);
        int next = object->getNextKeyframePos(object->getPos());
        QImage img = *object->getKeyframeImageAt(next);
        QPainter p(&img);
        QPainterPath path;
        p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        path.addRect(0, 0, width(), height());
        p.fillPath(path, Qt::blue);
        painter.drawImage(event->rect(), img, event->rect());
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

    object->clearImage(object->getPos());
    update();
}
