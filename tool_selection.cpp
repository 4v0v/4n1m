#include "tool_selection.h"
#include "editor.h"
#include "animation.h"

Tool_selection::Tool_selection() {
    init_values();
}

void Tool_selection::init_values() {
    preview_image           = QImage(QSize(Mw::animation->dimensions.width()+1, Mw::animation->dimensions.height() +1), QImage::Format_ARGB32);
    selected_image          = QImage(1, 1, QImage::Format_ARGB32);
    state                   = SELECTION_EMPTY;
    selected_zone           = QRect(0, 0, 1, 1);;
    initial_selected_zone   = QRect(0, 0, 1, 1);;
    selected_zone_delta_pos = QPoint(0, 0);
    mirrored_h              = false;
    mirrored_v              = false;
}

void Tool_selection::press(QMouseEvent* e) {
    Mw::editor->state = SCRIBBLING;

    if (state == SELECTION_EMPTY) {
        state = SELECTION_SELECTING;
        selected_zone.setRect(e->x(), e->y(), 0, 0);

    } else if (state == SELECTION_SELECTED) {
        QRect top_right    = QRect(selected_zone.topRight().x()    - 10, selected_zone.topRight().y()    - 10, 20, 20);
        QRect top_left     = QRect(selected_zone.topLeft().x()     - 10, selected_zone.topLeft().y()     - 10, 20, 20);
        QRect bottom_right = QRect(selected_zone.bottomRight().x() - 10, selected_zone.bottomRight().y() - 10, 20, 20);
        QRect bottom_left  = QRect(selected_zone.bottomLeft().x()  - 10, selected_zone.bottomLeft().y()  - 10, 20, 20);

        if      (top_right.contains(e->pos()))    state = SELECTION_SCALING_TR;
        else if (top_left.contains(e->pos()))     state = SELECTION_SCALING_TL;
        else if (bottom_right.contains(e->pos())) state = SELECTION_SCALING_BR;
        else if (bottom_left.contains(e->pos()))  state = SELECTION_SCALING_BL;
        else if (selected_zone.contains(e->pos())) {
            state = SELECTION_MOVING;
            selected_zone_delta_pos.setX(e->x() - selected_zone.x());
            selected_zone_delta_pos.setY(e->y() - selected_zone.y());
        } else {
            init_values();
            state = SELECTION_SELECTING;
            selected_zone.setRect(e->x(), e->y(), 0, 0);
        }
    }

    Mw::editor->update();
    Mw::timeline->update();
}

void Tool_selection::move(QMouseEvent* e) {
    QRect selected_zone_before_move = QRect(selected_zone);

    switch(state) {
        case SELECTION_SELECTING:  selected_zone.setSize(QSize(e->x() - selected_zone.x(), e->y() - selected_zone.y())); break;
        case SELECTION_MOVING:     selected_zone.moveTo(e->pos() - selected_zone_delta_pos); break;
        case SELECTION_SCALING_TR: selected_zone.setTopRight(e->pos());    break;
        case SELECTION_SCALING_TL: selected_zone.setTopLeft(e->pos());     break;
        case SELECTION_SCALING_BR: selected_zone.setBottomRight(e->pos()); break;
        case SELECTION_SCALING_BL: selected_zone.setBottomLeft(e->pos());  break;
        default: break;
    }

    if (state == SELECTION_SCALING_TR || state == SELECTION_SCALING_TL || state == SELECTION_SCALING_BR || state == SELECTION_SCALING_BL) {
        if (
            (selected_zone_before_move.width() >= 0 && selected_zone.width()  < 0) ||
            (selected_zone_before_move.width()  < 0 && selected_zone.width() >= 0)
        ) mirrored_h = !mirrored_h;

        if (
            (selected_zone_before_move.height() >= 0 && selected_zone.height()  < 0) ||
            (selected_zone_before_move.height()  < 0 && selected_zone.height() >= 0)
        ) mirrored_v = !mirrored_v;
    }

    Mw::editor->update();
};

void Tool_selection::release(QMouseEvent*) {
    Mw::editor->state = IDLE;

    if (state == SELECTION_SELECTING) {
        if ( std::abs(selected_zone.width()) < 2 || std::abs(selected_zone.height()) < 2) {
            init_values();
        } else {
            state = SELECTION_SELECTED;
            int x = selected_zone.topLeft().x();
            int y = selected_zone.topLeft().y();
            int w = std::abs(selected_zone.width());
            int h = std::abs(selected_zone.height());

            if (selected_zone.width()  < 0) x -= w;
            if (selected_zone.height() < 0) y -= h;

            selected_zone.setRect(x, y, w, h);
            selected_image        = Mw::editor->grab().toImage().copy(selected_zone); // TODO: use another method to get the image
            initial_selected_zone = selected_zone;
        }

    } else if (state == SELECTION_MOVING || state == SELECTION_SCALING_TR || state == SELECTION_SCALING_TL || state == SELECTION_SCALING_BR || state == SELECTION_SCALING_BL) {
        state = SELECTION_SELECTED;
    }

    Mw::editor->update();
};

QImage* Tool_selection::preview() {
    if (state == SELECTION_EMPTY) return nullptr;

    preview_image.fill(Qt::transparent);

    QPainter painter(&preview_image);

    painter.translate(-Mw::editor->offset/Mw::editor->scale);
    painter.scale(1/Mw::editor->scale, 1/Mw::editor->scale);

    // initial zone
    painter.setBrush(Qt::blue);
    painter.drawRect(initial_selected_zone);

    //selection zone
    if (state == SELECTION_SELECTING) {
        painter.setPen(QPen(Qt::red, 2, Qt::DashLine));
        painter.setBrush(Qt::transparent);
        painter.drawRect(selected_zone);

    } else if (state == SELECTION_MOVING) {
        QPoint real_top_left = selected_zone.topLeft();
        if (selected_zone.right()  < selected_zone.left()) real_top_left.setX(selected_zone.right());
        if (selected_zone.bottom() < selected_zone.top())  real_top_left.setY(selected_zone.bottom());

        painter.drawImage(real_top_left, selected_image.scaled(std::abs(selected_zone.width()), std::abs(selected_zone.height())).mirrored(mirrored_h, mirrored_v));

        painter.setPen(QPen(Qt::green, 2, Qt::DashLine));
        painter.setBrush(Qt::transparent);
        painter.drawRect(selected_zone);

    } else if (state == SELECTION_SELECTED) {
        QPoint real_top_left = selected_zone.topLeft();
        if (selected_zone.right()  < selected_zone.left()) real_top_left.setX(selected_zone.right());
        if (selected_zone.bottom() < selected_zone.top())  real_top_left.setY(selected_zone.bottom());

        painter.drawImage(real_top_left, selected_image.scaled(std::abs(selected_zone.width()), std::abs(selected_zone.height())).mirrored(mirrored_h, mirrored_v));

        painter.setPen(QPen(Qt::yellow, 2, Qt::DashLine));
        painter.setBrush(Qt::transparent);
        painter.drawRect(selected_zone);

        painter.setPen(Qt::black);
        painter.setBrush(Qt::red);
        painter.drawRect(selected_zone.topRight().x()    - 5, selected_zone.topRight().y()    - 5, 10, 10);
        painter.drawRect(selected_zone.topLeft().x()     - 5, selected_zone.topLeft().y()     - 5, 10, 10);
        painter.drawRect(selected_zone.bottomRight().x() - 5, selected_zone.bottomRight().y() - 5, 10, 10);
        painter.drawRect(selected_zone.bottomLeft().x()  - 5, selected_zone.bottomLeft().y()  - 5, 10, 10);

    } else if (state == SELECTION_SCALING_TR || state == SELECTION_SCALING_TL || state == SELECTION_SCALING_BR || state == SELECTION_SCALING_BL) {
        QPoint real_top_left = selected_zone.topLeft();
        if (selected_zone.right()  < selected_zone.left()) real_top_left.setX(selected_zone.right());
        if (selected_zone.bottom() < selected_zone.top())  real_top_left.setY(selected_zone.bottom());

        painter.drawImage(real_top_left, selected_image.scaled(std::abs(selected_zone.width()), std::abs(selected_zone.height())).mirrored(mirrored_h, mirrored_v));

        painter.setPen(QPen(Qt::red, 2, Qt::DashLine));
        painter.setBrush(Qt::transparent);
        painter.drawRect(selected_zone);

        painter.setPen(Qt::black);
        painter.setBrush(Qt::red);
        painter.drawRect(selected_zone.topRight().x()    - 5, selected_zone.topRight().y()    - 5, 10, 10);
        painter.drawRect(selected_zone.topLeft().x()     - 5, selected_zone.topLeft().y()     - 5, 10, 10);
        painter.drawRect(selected_zone.bottomRight().x() - 5, selected_zone.bottomRight().y() - 5, 10, 10);
        painter.drawRect(selected_zone.bottomLeft().x()  - 5, selected_zone.bottomLeft().y()  - 5, 10, 10);
    }

    return &preview_image;
};

