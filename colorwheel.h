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

#pragma once
#include "mw.h"

class ColorWheel : public QWidget
{
    Q_OBJECT
public:
    ColorWheel(QWidget*);

    virtual void mousePressEvent  (QMouseEvent* );
    virtual void mouseMoveEvent   (QMouseEvent* );
    virtual void mouseReleaseEvent(QMouseEvent* );
    virtual void resizeEvent      (QResizeEvent*);
    virtual void paintEvent       (QPaintEvent* );

    void change_hue(const int& hue);
    void change_saturation(const int& sat);
    void change_value(const int& value);
    void draw_wheel_image(const QSize& newSize);
    void draw_square_image(const int& hue);
    void draw_color_wheel(QPixmap& pixmap);
    QColor pick_color(const QPoint& point);
    QColor color() { return current_color; };

    QImage  wheel_image;
    QPixmap wheel_pixmap;
    QRect   wheel_rect;
    QImage  square_image;
    QRect   square_rect;

    QColor current_color = QColor(1,0,0);
    int  wheel_thickness = 15;
    bool is_in_wheel     = false;
    bool is_in_square    = false;

signals:
    void select_color(const QColor& color);

public slots:
    void set_color(QColor color);

};

