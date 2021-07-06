#include "tool_knockback.h"
#include "editor.h"
#include "animation.h"

Tool_knockback::Tool_knockback()
{
    preview_image = QImage(QSize(Mw::animation->dimensions.width()+1, Mw::animation->dimensions.height() +1), QImage::Format_ARGB32);
}

void Tool_knockback::press(QMouseEvent* e)
{
    Mw::editor->state = SCRIBBLING;

    if (!Mw::animation->has_frame_at(Mw::editor->layer_pos, Mw::editor->frame_pos))
    {
        Animation::frame frame;

        if (Mw::editor->is_copy_prev_enabled) frame = Mw::animation->get_prev_frame_at(Mw::editor->layer_pos, Mw::editor->frame_pos);

        Mw::undostack->push(new AddFrameCommand(frame, Mw::editor->layer_pos, Mw::editor->frame_pos));
    }

    position         = QVector2D(e->pos());
    delta_position   = position;
    knockback_amount = (position - delta_position).length()/3;

    Mw::editor  ->update();
    Mw::timeline->update();
}

void Tool_knockback::move(QMouseEvent* e)
{
    delta_position   = QVector2D(e->pos());
    knockback_amount = (position - delta_position).length()/3;

    Mw::editor->update();
};

void Tool_knockback::release(QMouseEvent*)
{
    Animation::frame i = Mw::animation->get_frame_at(Mw::editor->layer_pos, Mw::editor->frame_pos);
    Animation::frame j = Mw::animation->get_frame_at(Mw::editor->layer_pos, Mw::editor->frame_pos);

    for (int y = 0; y < j.image.height(); y++)
    {
        QRgb* rgb = (QRgb*)j.image.scanLine(y);

        for (int x = 0; x < j.image.width(); x++)
        {
            auto alpha = qAlpha(rgb[x]) > knockback_amount ? qAlpha(rgb[x]) - knockback_amount : 0;

            rgb[x] = qRgba(qRed(rgb[x]), qGreen(rgb[x]), qBlue(rgb[x]), alpha);
        }
    }

    Mw::undostack->push(new ModifyFrameCommand(i, j, Mw::editor->layer_pos, Mw::editor->frame_pos));

    preview_image.fill(Qt::transparent);

    Mw::editor->state = IDLE;
    Mw::editor->update();
};

QImage* Tool_knockback::preview()
{
    preview_image.fill(Qt::transparent);

    if (Mw::editor->state == IDLE) return nullptr;

    QPainter painter(&preview_image);

    painter.translate(-Mw::editor->offset/Mw::editor->scale);
    painter.scale(1/Mw::editor->scale, 1/Mw::editor->scale);

    // background
    painter.setBrush(Mw::editor->paper_color);
    painter.drawRect(Mw::editor->rect());

    // frames
    painter.resetTransform();
    QList<int> layer_keys = Mw::animation->layers.keys();
    QList<int>::const_reverse_iterator ri = layer_keys.crbegin();
    while(ri != layer_keys.crend()) {
        auto layer_pos = *ri;

        if (layer_pos != Mw::editor->layer_pos) {
            painter.setOpacity(Mw::animation->get_layer_at(layer_pos).opacity/100.0);
            Animation::frame frame = Mw::animation->has_frame_at(layer_pos, Mw::editor->frame_pos)?
                Mw::animation->get_frame_at(layer_pos, Mw::editor->frame_pos):
                Mw::animation->get_prev_frame_at(layer_pos, Mw::editor->frame_pos);
            painter.drawImage(frame.dimensions.topLeft(), frame.image);
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
            painter.drawImage(frame.dimensions.topLeft(), frame.image);
        }

        ri++;
    }

    painter.translate(-Mw::editor->offset/Mw::editor->scale);
    painter.scale(1/Mw::editor->scale, 1/Mw::editor->scale);

    painter.drawLine(position.x(), position.y(), delta_position.x(), delta_position.y());

    return &preview_image;
};
