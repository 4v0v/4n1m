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
    is_mirrored_h           = false;
    is_mirrored_v           = false;
    is_copying              = false;
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

            // TODO: si on a déjà sélectionné et qu'on appuie sur CTRL il faut réinitialiser la sélection pour faire une nouvelle copie
            if (QApplication::keyboardModifiers() == Qt::CTRL) is_copying = true;

            selected_zone_delta_pos.setX(e->x() - selected_zone.x());
            selected_zone_delta_pos.setY(e->y() - selected_zone.y());
        } else {
            draw_on_frame();
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
        ) is_mirrored_h = !is_mirrored_h;

        if (
            (selected_zone_before_move.height() >= 0 && selected_zone.height()  < 0) ||
            (selected_zone_before_move.height()  < 0 && selected_zone.height() >= 0)
        ) is_mirrored_v = !is_mirrored_v;
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

            if (x < Mw::editor->offset.x()) {
                w -= Mw::editor->offset.x() - x;
                x = Mw::editor->offset.x();
            }

            if (y < Mw::editor->offset.y()) {
                h -= Mw::editor->offset.y() - y;
                y = Mw::editor->offset.y();
            }

            if (w > (Mw::editor->offset.x() + Mw::animation->dimensions.width() * Mw::editor->scale) - x) {
                w = (Mw::editor->offset.x() + Mw::animation->dimensions.width() * Mw::editor->scale) - x;
            }

            if (h > (Mw::editor->offset.y() + Mw::animation->dimensions.height() * Mw::editor->scale) - y) {
                h = (Mw::editor->offset.y() + Mw::animation->dimensions.height() * Mw::editor->scale) - y;
            }

            selected_zone.setRect(x, y, w, h);

            Animation::frame frame = Mw::animation->get_frame_at(Mw::editor->layer_pos, Mw::editor->frame_pos);
            Mw::animation->resize_frame(&frame, LEFT  , 0);
            Mw::animation->resize_frame(&frame, RIGHT , 10000);
            Mw::animation->resize_frame(&frame, TOP   , 0);
            Mw::animation->resize_frame(&frame, BOTTOM, 10000);

            QRect r = QRect(selected_zone);
            r.setX((r.x() - Mw::editor->offset.x())/ Mw::editor->scale);
            r.setY((r.y() - Mw::editor->offset.y())/ Mw::editor->scale);
            r.setWidth(w / Mw::editor->scale);
            r.setHeight(h / Mw::editor->scale);

            selected_image        = frame.image.copy(r);

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

    // background
    painter.setBrush(Mw::editor->paper_color);
    painter.translate(-Mw::editor->offset/Mw::editor->scale);
    painter.scale(1/Mw::editor->scale, 1/Mw::editor->scale);
    painter.drawRect(Mw::editor->rect());
    painter.resetTransform();

    // complete current frame
    Animation::frame frame = Mw::animation->get_frame_at(Mw::editor->layer_pos, Mw::editor->frame_pos);
    painter.drawImage(frame.dimensions.topLeft(), frame.image);

    //
    painter.translate(-Mw::editor->offset/Mw::editor->scale);
    painter.scale(1/Mw::editor->scale, 1/Mw::editor->scale);

    // initial zone
    if (!is_copying) {
        painter.setPen(Mw::editor->paper_color);
        painter.setBrush(Mw::editor->paper_color);
        painter.drawRect(initial_selected_zone);
    }

    // selection zone
    if (state == SELECTION_SELECTING) {
        painter.setPen(QPen(Qt::red, 2, Qt::DashLine));
        painter.setBrush(Qt::transparent);
        painter.drawRect(selected_zone);

    } else if (state == SELECTION_MOVING) {
        QPoint real_top_left = selected_zone.topLeft();
        if (selected_zone.right()  < selected_zone.left()) real_top_left.setX(selected_zone.right());
        if (selected_zone.bottom() < selected_zone.top())  real_top_left.setY(selected_zone.bottom());

        painter.drawImage(real_top_left, selected_image.scaled(std::abs(selected_zone.width()), std::abs(selected_zone.height())).mirrored(is_mirrored_h, is_mirrored_v));

        painter.setPen(QPen(Qt::green, 2, Qt::DashLine));
        painter.setBrush(Qt::transparent);
        painter.drawRect(selected_zone);

    } else if (state == SELECTION_SELECTED) {
        QPoint real_top_left = selected_zone.topLeft();
        if (selected_zone.right()  < selected_zone.left()) real_top_left.setX(selected_zone.right());
        if (selected_zone.bottom() < selected_zone.top())  real_top_left.setY(selected_zone.bottom());

        painter.drawImage(real_top_left, selected_image.scaled(std::abs(selected_zone.width()), std::abs(selected_zone.height())).mirrored(is_mirrored_h, is_mirrored_v));

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

        painter.drawImage(real_top_left, selected_image.scaled(std::abs(selected_zone.width()), std::abs(selected_zone.height())).mirrored(is_mirrored_h, is_mirrored_v));

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

void Tool_selection::draw_on_frame()
{
    Animation::frame i = Mw::animation->get_frame_at(Mw::editor->layer_pos, Mw::editor->frame_pos);
    Animation::frame j = Mw::animation->get_frame_at(Mw::editor->layer_pos, Mw::editor->frame_pos);

    // redimentionner frame
    Mw::animation->resize_frame(&j, LEFT  , 0);
    Mw::animation->resize_frame(&j, RIGHT , 10000);
    Mw::animation->resize_frame(&j, TOP   , 0);
    Mw::animation->resize_frame(&j, BOTTOM, 10000);

    QPainter painter(&j.image);

    // dessiner le trou initial_selected_zone
    painter.translate(-Mw::editor->offset/Mw::editor->scale);
    painter.scale(1/Mw::editor->scale, 1/Mw::editor->scale);
    if (!is_copying) {
        painter.setPen(Mw::editor->paper_color);
        painter.setBrush(Mw::editor->paper_color);
        painter.drawRect(initial_selected_zone);
    }

    // dessiner selected_image au bon endroit
    QPoint real_top_left = selected_zone.topLeft();
    if (selected_zone.right()  < selected_zone.left()) real_top_left.setX(selected_zone.right());
    if (selected_zone.bottom() < selected_zone.top())  real_top_left.setY(selected_zone.bottom());

    painter.drawImage(real_top_left, selected_image.scaled(std::abs(selected_zone.width()), std::abs(selected_zone.height())).mirrored(is_mirrored_h, is_mirrored_v));

    Mw::undostack->push(new ModifyFrameCommand(i, j, Mw::editor->layer_pos, Mw::editor->frame_pos));
    init_values();
}

