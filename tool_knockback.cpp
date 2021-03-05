#include "tool_knockback.h"
#include "editor.h"
#include "animation.h"

void Tool_knockback::press(QMouseEvent*) {
    if (!Mw::animation->is_frame_at(Mw::editor->layer_pos, Mw::editor->frame_pos)) {
        if (Mw::editor->is_copy_prev_frame)
            Mw::undostack->push(new AddFrameCommand(Mw::animation->get_prev_frame_at(Mw::editor->layer_pos, Mw::editor->frame_pos), Mw::editor->layer_pos, Mw::editor->frame_pos));
        else
            Mw::undostack->push(new AddFrameCommand(Animation::frame{}, Mw::editor->layer_pos, Mw::editor->frame_pos));
    }

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
}

void Tool_knockback::move(QMouseEvent*) {

};

void Tool_knockback::release(QMouseEvent*) {

};

void Tool_knockback::preview(QImage*) {

};
