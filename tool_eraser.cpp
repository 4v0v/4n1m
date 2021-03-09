#include "tool_eraser.h"
#include "editor.h"
#include "animation.h"

Tool_eraser::Tool_eraser() {
    preview_image = QImage(QSize(Mw::animation->dimensions.width()+1, Mw::animation->dimensions.height() +1), QImage::Format_ARGB32);
}

void Tool_eraser::press(QMouseEvent *e) {
    Mw::editor->state = SCRIBBLING;

    if (!Mw::animation->is_frame_at(Mw::editor->layer_pos, Mw::editor->frame_pos)) {
        if (Mw::editor->is_copy_prev_frame)
            Mw::undostack->push(new AddFrameCommand(Mw::animation->get_prev_frame_at(Mw::editor->layer_pos, Mw::editor->frame_pos), Mw::editor->layer_pos, Mw::editor->frame_pos));
        else
            Mw::undostack->push(new AddFrameCommand(Animation::frame{}, Mw::editor->layer_pos, Mw::editor->frame_pos));
    }

    stroke << e->pos();

    Mw::editor->update();
    Mw::timeline->update();
}

void Tool_eraser::move(QMouseEvent *e) {
    stroke << e->pos();
    // TODO: understand why the fuck i can't put that inside .h, fuck C++
    Mw::editor->update(
        stroke.boundingRect().x() - 50,
        stroke.boundingRect().y() - 50,
        stroke.boundingRect().width() + 100,
        stroke.boundingRect().height() + 100
    );
};

void Tool_eraser::release(QMouseEvent *) {
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

    QImage k = j.image.copy();
    k.fill(Qt::transparent);

    QPainter eraser_painter(&k);
    eraser_painter.translate(-Mw::editor->offset/Mw::editor->scale - j.dimensions.topLeft());
    eraser_painter.scale(1/Mw::editor->scale, 1/Mw::editor->scale);
    eraser_painter.setPen(QPen(eraser_tool.color(), eraser_tool.width() * Mw::editor->scale, eraser_tool.style(), eraser_tool.capStyle(), eraser_tool.joinStyle()));

    if (stroke.count() == 1)
        eraser_painter.drawPoint(stroke.first());
    else if (stroke.count() > 1)
        eraser_painter.drawPolyline(stroke);

    frame_painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    frame_painter.drawImage(0, 0, k);

    stroke.clear();
    Mw::undostack->push(new ModifyFrameCommand(i, j, Mw::editor->layer_pos, Mw::editor->frame_pos));

    Mw::editor->state = IDLE;
    Mw::editor->update();
};

QImage* Tool_eraser::preview() {
    preview_image.fill(Qt::transparent);

    QPainter painter(&preview_image);

    painter.translate(-Mw::editor->offset/Mw::editor->scale);
    painter.scale(1/Mw::editor->scale, 1/Mw::editor->scale);

    painter.setPen(QPen(
        eraser_tool.color(),
        eraser_tool.width() * Mw::editor->scale,
        eraser_tool.style(), eraser_tool.capStyle(),
        eraser_tool.joinStyle()
    ));

    if (stroke.count() == 1)
        painter.drawPoint(stroke.first());
    else if (stroke.count() > 1)
        painter.drawPolyline(stroke);

    return &preview_image;
};
