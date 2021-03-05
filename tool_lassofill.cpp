#include "tool_lassofill.h"
#include "editor.h"
#include "animation.h"

void Tool_lassofill::press(QMouseEvent *e) {
    stroke << e->pos();
    if (!Mw::animation->is_frame_at(Mw::editor->layer_pos, Mw::editor->frame_pos)) {
        if (Mw::editor->is_copy_prev_frame)
            Mw::undostack->push(new AddFrameCommand(Mw::animation->get_prev_frame_at(Mw::editor->layer_pos, Mw::editor->frame_pos), Mw::editor->layer_pos, Mw::editor->frame_pos));
        else
            Mw::undostack->push(new AddFrameCommand(Animation::frame{}, Mw::editor->layer_pos, Mw::editor->frame_pos));
    }
}

void Tool_lassofill::move(QMouseEvent *e) {
    stroke << e->pos();
    // TODO: understand why the fuck i can't put that inside .h, fuck C++
    Mw::editor->update(
        stroke.boundingRect().x() - 50,
        stroke.boundingRect().y() - 50,
        stroke.boundingRect().width() + 100,
        stroke.boundingRect().height() + 100
    );
};

void Tool_lassofill::release(QMouseEvent *) {
    Animation::frame i = Mw::animation->get_frame_at(Mw::editor->layer_pos, Mw::editor->frame_pos);
    Animation::frame j = Mw::animation->get_frame_at(Mw::editor->layer_pos, Mw::editor->frame_pos);

    // Init
    if (j.is_empty) Mw::animation->init_frame(&j, (stroke.first() - Mw::editor->offset) / Mw::editor->scale);

    // Resize frame
    QRect bb((stroke.boundingRect().topLeft() - Mw::editor->offset) / Mw::editor->scale,stroke.boundingRect().size() / Mw::editor->scale);
    if (bb.right()  > j.dimensions.right())  Mw::animation->resize_frame(&j, RIGHT,  bb.right());
    if (bb.bottom() > j.dimensions.bottom()) Mw::animation->resize_frame(&j, BOTTOM, bb.bottom());
    if (bb.left()   < j.dimensions.left())   Mw::animation->resize_frame(&j, LEFT,   bb.left());
    if (bb.top()    < j.dimensions.top())    Mw::animation->resize_frame(&j, TOP,    bb.top());

    // Draw on key
    QPainter frame_painter(&j.image);

    frame_painter.translate(-Mw::editor->offset/Mw::editor->scale - j.dimensions.topLeft());
    frame_painter.scale(1/Mw::editor->scale, 1/Mw::editor->scale);
    frame_painter.setPen(Qt::transparent);
    frame_painter.setBrush(lassofill_tool);
    frame_painter.drawPolygon(stroke);

    stroke.clear();
    Mw::undostack->push(new ModifyFrameCommand(i, j, Mw::editor->layer_pos, Mw::editor->frame_pos));
};

void Tool_lassofill::preview(QImage* preview) {
    QPainter preview_painter(preview);

    preview_painter.translate(-Mw::editor->offset/Mw::editor->scale);
    preview_painter.scale(1/Mw::editor->scale, 1/Mw::editor->scale);

    preview_painter.setPen(Qt::transparent);
    preview_painter.setBrush(lassofill_tool);
    preview_painter.drawPolygon(stroke);
};
