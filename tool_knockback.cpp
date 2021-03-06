#include "tool_knockback.h"
#include "editor.h"
#include "animation.h"

void Tool_knockback::press(QMouseEvent* e) {
    if (!Mw::animation->is_frame_at(Mw::editor->layer_pos, Mw::editor->frame_pos)) {
        if (Mw::editor->is_copy_prev_frame)
            Mw::undostack->push(new AddFrameCommand(Mw::animation->get_prev_frame_at(Mw::editor->layer_pos, Mw::editor->frame_pos), Mw::editor->layer_pos, Mw::editor->frame_pos));
        else
            Mw::undostack->push(new AddFrameCommand(Animation::frame{}, Mw::editor->layer_pos, Mw::editor->frame_pos));
    }

    position       = QVector2D(e->pos());
    delta_position = position;
    knockback_amount = (position - delta_position).length()/3;
}

void Tool_knockback::move(QMouseEvent* e) {
    delta_position = QVector2D(e->pos());
    knockback_amount = (position - delta_position).length()/3;
    Mw::editor->update();
};

void Tool_knockback::release(QMouseEvent*) {
    Animation::frame i = Mw::animation->get_frame_at(Mw::editor->layer_pos, Mw::editor->frame_pos);
    Animation::frame j = Mw::animation->get_frame_at(Mw::editor->layer_pos, Mw::editor->frame_pos);

    for (int y = 0; y < j.image.height(); y++) {
        QRgb* rgb = (QRgb*)j.image.scanLine(y);
        for (int x = 0; x < j.image.width(); x++) {
            rgb[x] = qRgba(
                qRed(rgb[x]),
                qGreen(rgb[x]),
                qBlue(rgb[x]),
                qAlpha(rgb[x]) > knockback_amount ? qAlpha(rgb[x]) - knockback_amount : 0
            );
        }
    }

    Mw::undostack->push(new ModifyFrameCommand(i, j, Mw::editor->layer_pos, Mw::editor->frame_pos));
};

void Tool_knockback::preview(QImage* preview) {
    QPainter preview_painter(preview);

    preview_painter.translate(-Mw::editor->offset/Mw::editor->scale);
    preview_painter.scale(1/Mw::editor->scale, 1/Mw::editor->scale);

    // background
    preview_painter.setBrush(Mw::editor->paper_color);
    preview_painter.drawRect(Mw::editor->rect());

    // frames
    preview_painter.resetTransform();
    QList<int> layer_keys = Mw::animation->layers.keys();
    QList<int>::const_reverse_iterator ri = layer_keys.crbegin();
    while(ri != layer_keys.crend()) {
        auto layer_pos = *ri;

        if (layer_pos != Mw::editor->layer_pos) {
            preview_painter.setOpacity(Mw::animation->get_layer_at(layer_pos).opacity/100.0);
            Animation::frame frame = Mw::animation->is_frame_at(layer_pos, Mw::editor->frame_pos)?
                Mw::animation->get_frame_at(layer_pos, Mw::editor->frame_pos):
                Mw::animation->get_prev_frame_at(layer_pos, Mw::editor->frame_pos);
            preview_painter.drawImage(frame.dimensions.topLeft(), frame.image);
        } else {
            Animation::frame frame = Mw::animation->get_frame_at(layer_pos, Mw::editor->frame_pos);

            for (int y = 0; y < frame.image.height(); y++) {
                QRgb* rgb = (QRgb*)frame.image.scanLine(y);
                for (int x = 0; x < frame.image.width(); x++) {
                    rgb[x] = qRgba(
                        qRed(rgb[x]),
                        qGreen(rgb[x]),
                        qBlue(rgb[x]),
                        qAlpha(rgb[x]) > knockback_amount ? qAlpha(rgb[x]) - knockback_amount : 0
                    );
                }
            }
            preview_painter.drawImage(frame.dimensions.topLeft(), frame.image);
        }

        ri++;
    }

    preview_painter.translate(-Mw::editor->offset/Mw::editor->scale);
    preview_painter.scale(1/Mw::editor->scale, 1/Mw::editor->scale);

    preview_painter.drawLine(position.x(), position.y(), delta_position.x(), delta_position.y());
};
