/*

Pencil - Traditional Animation Software
Copyright (C) 2013-2018 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "colorwheel.h"

ColorWheel::ColorWheel(QWidget* parent) : QWidget(parent)
{
    current_color = current_color.toHsv();
}

void ColorWheel::mousePressEvent(QMouseEvent* e)
{
    if (square_rect.contains(e->pos()))
    {
        is_in_square = true;

        QColor color = pick_color(e->pos());
        change_saturation(color.saturation());
        change_value     (color.value()     );

    } else if (wheel_rect.contains(e->pos())) {
        is_in_wheel  = true;

        QColor color = pick_color(e->pos());
        change_hue(color.hue());
    }
}

void ColorWheel::mouseMoveEvent(QMouseEvent* e)
{
    if (e->buttons() == Qt::NoButton) return;

    auto pos = e->pos();

    if (square_rect.contains(pos) && is_in_square)
    {
        if      (pos.x() < square_rect.topLeft().x()    ) pos.setX(square_rect.topLeft().x()    );
        else if (pos.x() > square_rect.bottomRight().x()) pos.setX(square_rect.bottomRight().x());

        if      (pos.y() < square_rect.topLeft().y()    ) pos.setY(square_rect.topLeft().y()    );
        else if (pos.y() > square_rect.bottomRight().y()) pos.setY(square_rect.bottomRight().y());

        QColor color = pick_color(pos);

        change_saturation(color.saturation());
        change_value     (color.value()     );

    } else if (wheel_rect.contains(pos) && is_in_wheel) {
        QColor color = pick_color(pos);

        change_hue(color.hue());
    }
}

void ColorWheel::mouseReleaseEvent(QMouseEvent*)
{
    is_in_wheel  = false;
    is_in_square = false;

    emit select_color(current_color);
}

void ColorWheel::resizeEvent(QResizeEvent* e)
{
    wheel_pixmap = QPixmap(e->size());
    wheel_pixmap.fill(Qt::transparent);

    draw_wheel_image(e->size());
    draw_square_image(current_color.hue());

    update();
}

void ColorWheel::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    draw_color_wheel(wheel_pixmap);
    painter.translate(width() / 2, height() / 2);
    painter.translate(-wheel_pixmap.width() / 2, -wheel_pixmap.height() / 2);
    painter.drawPixmap(0, 0, wheel_pixmap);
}

void ColorWheel::set_color(QColor color)
{
    color = color.toHsv();

    if (color == current_color) return;

    // grayscale color, keep the current hue
    if (color.hue() == -1)
        color.setHsv(current_color.hue(), color.saturation(), color.value(), color.alpha());

    current_color = color;

    draw_square_image(color.hue());

    update();
}

QColor ColorWheel::pick_color(const QPoint& point)
{
    if (!wheel_pixmap.rect().contains(point)) return QColor();

    if (is_in_wheel)
    {
        qreal  hue = 0;
        QPoint center(width() / 2, height() / 2);
        QPoint diff = point - center;

        hue = qAtan2(-diff.y(), diff.x()) / M_PI * 180;
        hue = fmod((hue + 360), 360); // shift -180~180 to 0~360
        hue = (hue > 359) ? 359 : hue;
        hue = (hue < 0  ) ? 0   : hue;

        return QColor::fromHsv(static_cast<int>(hue), current_color.saturation(), current_color.value());
        
    } else if (is_in_square) {
        QPointF p = point - square_rect.topLeft();

        return QColor::fromHsvF(current_color.hueF(), p.x() / (square_rect.width() - 1), 1.0 - (p.y() / (square_rect.height()-1)));
    }

    return QColor();
}

void ColorWheel::draw_wheel_image(const QSize &newSize)
{
    int r = qMin(newSize.width(), newSize.height());

    QBrush backgroundBrush = QBrush(Qt::black);

    wheel_image = QImage(newSize, QImage::Format_ARGB32_Premultiplied);

    QPainter painter(&wheel_image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(wheel_image.rect(), backgroundBrush);

    QConicalGradient conicalGradient(0, 0, 0);

    for (int hue = 0; hue < 360; hue += 1)
        conicalGradient.setColorAt(hue / 360.0, QColor::fromHsv(hue, 255, 255));

    qreal ir = r - wheel_thickness;

    // outer circle
    painter.translate(width() / 2, height() / 2);
    QBrush brush(conicalGradient);
    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawEllipse(QPoint(0, 0), r / 2, r / 2);

    // inner circle
    painter.setBrush(backgroundBrush);
    painter.drawEllipse(QPoint(0, 0), r / 2 - wheel_thickness, r / 2 - wheel_thickness);

    // Center of wheel
    qreal m1 = (wheel_pixmap.width()  / 2) - (ir / qSqrt(2));
    qreal m2 = (wheel_pixmap.height() / 2) - (ir / qSqrt(2));

    // calculate size of wheel width
    qreal wheelWidth = 2 * ir / qSqrt(2);

    // Calculate wheel region
    wheel_rect = QRectF(m1, m2, wheelWidth, wheelWidth).toAlignedRect();
}

void ColorWheel::draw_square_image(const int &hue)
{
    int   widget_region       = qMin(width(), height());
    qreal outer_circle_radius = widget_region / 2.0;
    qreal inner_circle_radius = outer_circle_radius - wheel_thickness;

    // top left of square
    qreal m1 = (width()  / 2) - (inner_circle_radius / qSqrt(2.1));
    qreal m2 = (height() / 2) - (inner_circle_radius / qSqrt(2.1));

    QImage square(255, 255, QImage::Format_ARGB32);

    QLinearGradient colorGradient = QLinearGradient(0, 0, square.width(), 0);
    colorGradient.setColorAt(0, QColor(255, 255, 255));
    colorGradient.setColorAt(1, QColor::fromHsv(hue, 255, 255)); // color square should always use full value and saturation

    QLinearGradient blackGradient = QLinearGradient(0, 0, 0, square.height());
    blackGradient.setColorAt(0, QColor(0, 0, 0, 0  ));
    blackGradient.setColorAt(1, QColor(0, 0, 0, 255));

    QBrush colorGradiantBrush = QBrush(colorGradient);
    QBrush blackGradiantBrush = QBrush(blackGradient);

    QPainter painter(&square);
    painter.fillRect(square.rect(), colorGradiantBrush);
    painter.fillRect(square.rect(), blackGradiantBrush);

    qreal square_width = 2 * inner_circle_radius / qSqrt(2.1);
    square_rect  = QRectF(m1, m2, square_width, square_width).toAlignedRect();
    square_image = square.scaled(square_rect.size());
}


void ColorWheel::draw_color_wheel(QPixmap& pixmap)
{
    QPainter compose_painter(&pixmap);
    pixmap.fill(QColor(175,175,175));
    compose_painter.drawImage(0, 0, wheel_image);
    compose_painter.drawImage(square_rect, square_image);
    compose_painter.end();

    // draw hue indicator
    QPainter painter(&wheel_pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    if (current_color.hsvHue() > 20 && current_color.hsvHue() < 200)
        painter.setPen(Qt::black);
    else
        painter.setPen(Qt::white);

    painter.setBrush(Qt::NoBrush);

    QPen pen = painter.pen();
    pen.setWidth(3);

    painter.setPen(pen);
    qreal r = qMin(height(), width());

    painter.translate(width() / 2, height() / 2);
    painter.rotate(-current_color.hsvHue());

    r = r / 2.0 - wheel_thickness / 2;

    painter.drawEllipse(QPointF(r, 0), 7, 7);
    painter.end();

    // draw picker
    QPainter painter2(&wheel_pixmap);
    painter2.setRenderHint(QPainter::Antialiasing);
    int ellipseSize = 9;

    QPoint squareTopLeft = square_rect.topLeft();
    QSize squareSize = square_rect.size();

    qreal S = current_color.hsvSaturationF() * (squareSize.width()-1);
    qreal V = (squareSize.height() - (current_color.valueF() * squareSize.height()-1));

    QPen pen2;
    pen2.setWidth(1);
    if (current_color.hsvSaturation() > 30 || current_color.value() < 50)
        pen2.setColor(Qt::white);
    painter2.setPen(pen2);

    QTransform transform;
    transform.translate(-ellipseSize/2,-ellipseSize/2);
    transform.translate(squareTopLeft.x(),squareTopLeft.y()-1);
    painter2.setTransform(transform);
    painter2.drawEllipse(static_cast<int>(S), static_cast<int>(V), ellipseSize, ellipseSize);
    painter2.end();
}

void ColorWheel::change_hue(const int &hue)
{
    if (hue < 0 || hue > 359) return;

    int s = current_color.hsvSaturation();
    int v = current_color.value();
    int a = current_color.alpha();

    current_color.setHsv(hue, s, v, a);

    if (!isVisible()) return;

    draw_square_image(hue);

    update();
}

void ColorWheel::change_saturation(const int &s)
{
    int h = current_color.hsvHue();
    int v = current_color.value();
    int a = current_color.alpha();

    current_color.setHsv(h, s, v, a);

    update();
}

void ColorWheel::change_value(const int &v)
{
    int h = current_color.hsvHue();
    int s = current_color.hsvSaturation();
    int a = current_color.alpha();

    current_color.setHsv(h, s, v, a);

    update();
}
