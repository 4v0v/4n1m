#include "tool_selection.h"
#include "editor.h"
#include "animation.h"

Tool_selection::Tool_selection() {
    preview_image = QImage(QSize(Mw::animation->dimensions.width()+1, Mw::animation->dimensions.height() +1), QImage::Format_ARGB32);
}

void Tool_selection::press(QMouseEvent* e) {
    Mw::editor->state = SCRIBBLING;

    if (state == SELECTION_EMPTY) {
        state = SELECTION_SELECTING;
        selection_zone.setRect(e->x(), e->y(), 0, 0);
        initial_selection_zone = selection_zone;
        selected_image.fill(Qt::transparent);

    } else if (state == SELECTION_SELECTED) {
        if (selection_zone.contains(e->pos())) {
            state = SELECTION_MOVING;
            selection_zone_delta_pos.setX(e->x() - selection_zone.x());
            selection_zone_delta_pos.setY(e->y() - selection_zone.y());

        } else {
            state = SELECTION_SELECTING;
            selection_zone.setRect(e->x(), e->y(), 0, 0);
        }
    }

    Mw::editor->update();
    Mw::timeline->update();
}

void Tool_selection::move(QMouseEvent* e) {

    if (state == SELECTION_SELECTING) {
        selection_zone.setSize(QSize(e->x() - selection_zone.x(), e->y() - selection_zone.y()));

    } else if (state == SELECTION_MOVING) {
        selection_zone.moveTo(e->pos() - selection_zone_delta_pos);
    }

    Mw::editor->update();
};

void Tool_selection::release(QMouseEvent*) {
    Mw::editor->state = IDLE;

    if (state == SELECTION_SELECTING) {
        if (selection_zone.width() == 0 || selection_zone.height() == 0) {
            state = SELECTION_EMPTY;
        } else {
            state = SELECTION_SELECTED;
            selected_image         = Mw::editor->grab().toImage().copy(selection_zone);
            initial_selection_zone = selection_zone;
        }

    } else if (state == SELECTION_MOVING) {
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
    painter.drawRect(initial_selection_zone);

    //image inside selection zone
    painter.drawImage(selection_zone.topLeft(), selected_image);

    //selection zone
    if (state == SELECTION_SELECTING) {
        painter.setPen(QPen(Qt::red, 2, Qt::DashLine));

    } else if (state == SELECTION_SELECTED) {
        painter.setPen(QPen(Qt::yellow, 2, Qt::DashLine));

    } else if (state == SELECTION_MOVING) {
        painter.setPen(QPen(Qt::green, 2, Qt::DashLine));
    }
    painter.setBrush(Qt::transparent);
    painter.drawRect(selection_zone);

    return &preview_image;
};
