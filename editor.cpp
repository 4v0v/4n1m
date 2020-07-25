#include "editor.h"

Editor::Editor(Mw* mw): QWidget(mw)
{
    setCursor(Qt::CrossCursor);
    onion_skins = QImage(Mw::animation->dimensions, QImage::Format_ARGB32);
    playing_timer->connect(playing_timer, &QTimer::timeout, this, [this]{ this->play_step();});
}

void Editor::mousePressEvent(QMouseEvent* e)
{
    if (state != IDLE) return;
    if (e->button() == Qt::LeftButton)
    {
        if (!QRect(offset, Mw::animation->dimensions*scale).contains(e->pos())) return;
        state = SCRIBBLING;
        stroke << e->pos();
        if (!Mw::animation->is_frame_at(layer_pos, frame_pos)) {
             Mw::undostack->push(new AddFrameCommand(Animation::frame{}, layer_pos, frame_pos));
        }
    } else if (e->button() == Qt::RightButton || e->button() == Qt::MiddleButton ) {
        state = MOVING;
        moving_offset = e->pos();
        moving_offset_delta = e->pos();
    }

    update();
    Mw::timeline->update();
}

void Editor::mouseMoveEvent(QMouseEvent* e)
{
    switch (state)
    {
        case SCRIBBLING:
            stroke << e->pos();
            update(
                stroke.boundingRect().x() - 50,
                stroke.boundingRect().y() - 50,
                stroke.boundingRect().width() + 100,
                stroke.boundingRect().height() + 100
            );
            break;
        case MOVING :
            moving_offset_delta = e->pos();
            update(rect());
            break;
        case PLAYING:
        case IDLE:
            break;
    }
}

void Editor::mouseReleaseEvent(QMouseEvent*)
{
    switch (state)
    {
        case SCRIBBLING:
            draw_on_key();
            state = IDLE;
            break;
        case MOVING:
            offset += moving_offset_delta - moving_offset;
            moving_offset = moving_offset_delta;
            state = IDLE;
            break;
        case PLAYING:
        case IDLE:
            break;
    }
}

void Editor::resizeEvent(QResizeEvent *e)
{
    int w = Mw::animation->dimensions.width() * scale;
    int h = Mw::animation->dimensions.height() * scale;

    offset.setX(width()/2 - w/2);
    offset.setY(height()/2 - h/2);

    QWidget::resizeEvent(e);
}

void Editor::wheelEvent(QWheelEvent* e){
    if (state != IDLE) return;

    if (QApplication::keyboardModifiers() == Qt::CTRL) {
        if (e->angleDelta().y() < 0 && scale > .7) {
            scale -= .1;
            offset += QPoint(Mw::animation->dimensions.width()/2*.1, Mw::animation->dimensions.height()/2*.1);
        } else if (e->angleDelta().y() > 0 && scale < 3.) {
            scale += .1;
            offset -= QPoint(Mw::animation->dimensions.width()/2*.1, Mw::animation->dimensions.height()/2*.1);
        }
        update();
    } else {
        Mw::timeline->wheelEvent(e);
    }
}

void Editor::paintEvent(QPaintEvent*)
{
    widget_painter.begin(this);

    //background
    Mw::set_painter_colors(&widget_painter, bg_color);
    widget_painter.drawRect(rect());
    //transform editor
    widget_painter.translate(offset + moving_offset_delta - moving_offset);
    widget_painter.scale(scale, scale);
    //editor frame
    Mw::set_painter_colors(&widget_painter, bg_color, img_bg_color);
    widget_painter.drawRect(0, 0, Mw::animation->dimensions.width() + 1, Mw::animation->dimensions.height() + 1);
    //onionskins
    if (is_os_enabled) widget_painter.drawImage(0,0, onion_skins);
    //visible frame

    // TODO: find better way to reverse iterate qlist
    QList<int> revert_list = Mw::animation->layers.keys();
    for(int k=0, s=Mw::animation->layers.keys().size(), max=(s/2); k<max; k++) revert_list.swap(k,s-(1+k));
    foreach(int i, revert_list) {
        if (Mw::animation->is_frame_at(i, frame_pos))
            widget_painter.drawImage(
                Mw::animation->get_frame_at(i, frame_pos).dimensions.topLeft(),
                Mw::animation->get_frame_at(i, frame_pos).image
            );
        else if (Mw::animation->is_frame_at(i, Mw::animation->get_prev_pos(i, frame_pos)))
            widget_painter.drawImage(
                Mw::animation->get_prev_frame_at(i, frame_pos).dimensions.topLeft(),
                Mw::animation->get_prev_frame_at(i, frame_pos).image
            );
    }

    //reset transform
    widget_painter.resetTransform();
    
    //tool preview
    switch (tool) {
        case PEN :
            widget_painter.setPen(QPen(pen_tool.color(), pen_tool.width() * scale));
            if (stroke.count() == 1) widget_painter.drawPoint(stroke.first());
            else if (stroke.count() > 1) widget_painter.drawPolyline(stroke);
            break;
        case LASSOFILL:
            widget_painter.setPen(Qt::transparent);
            widget_painter.setBrush(lassofill_tool);
            widget_painter.drawPolygon(stroke);
            break;
    }

    widget_painter.end();
}

void Editor::clear_current_layer()
{
    if (state != IDLE || Mw::animation->is_layer_empty(layer_pos)) return;
    Mw::animation->clear_layer_at(layer_pos);
    Mw::undostack->clear();
    Mw::update_editor_and_timeline();
}

void Editor::clear_frame_at_current_pos()
{
    if (state != IDLE || !Mw::animation->is_frame_at(layer_pos, frame_pos)) return;

    Animation::frame i = Mw::animation->get_frame_at(layer_pos, frame_pos);
    Mw::undostack->push(new ModifyFrameCommand(i, Animation::frame{}, layer_pos, frame_pos));
}

void Editor::remove_frame_at_current_pos()
{
    if (state != IDLE || !Mw::animation->is_frame_at(layer_pos, frame_pos)) return;
    Mw::undostack->push(new RemoveFrameCommand(layer_pos, frame_pos));
}

void Editor::insert_frame_at_current_pos()
{
    if (
        state != IDLE ||
        Mw::animation->is_anim_empty() ||
        frame_pos >= Mw::animation->get_last_pos(layer_pos)
    ) return;

    if (Mw::animation->is_frame_at(layer_pos, frame_pos))
        Mw::undostack->push(new InsertFrameCommand(layer_pos, frame_pos + 1));
    else if (!Mw::animation->is_frame_at(layer_pos, frame_pos))
        Mw::undostack->push(new InsertFrameCommand(layer_pos, frame_pos));
}

void Editor::uninsert_frame_at_current_pos()
{
    if (
        state != IDLE ||
        Mw::animation->is_anim_empty() ||
        Mw::animation->is_layer_empty(layer_pos) ||
        frame_pos >= Mw::animation->get_last_pos(layer_pos)
    ) return;

    if (Mw::animation->is_frame_at(layer_pos, frame_pos) && !Mw::animation->is_frame_at(layer_pos, frame_pos + 1))
        Mw::undostack->push(new UninsertFrameCommand(layer_pos, frame_pos + 1));
    else if (!Mw::animation->is_frame_at(layer_pos, frame_pos))
        Mw::undostack->push(new UninsertFrameCommand(layer_pos, frame_pos));
}

void Editor::goto_pos(int l, int p)
{
    if (state != IDLE || l < 0 || !Mw::animation->layers.contains(l)) return;
    layer_pos = l;

    if (state != IDLE || p < 0) return;
    frame_pos = p;
    Mw::update_editor_and_timeline();
}


void Editor::create_onions_at_current_pos()
{
    int fp;
    if (Mw::animation->is_frame_at(layer_pos, frame_pos))
        fp = frame_pos;
    else if (!Mw::animation->is_frame_at(layer_pos, frame_pos) && Mw::animation->get_prev_pos(layer_pos, frame_pos) != -1)
        fp = Mw::animation->get_prev_pos(layer_pos, frame_pos);
    else
        fp = 0;

    onion_skins = Mw::animation->create_onions_at(
        layer_pos,
        fp,
        is_os_loop_enabled,
        is_os_prev_enabled,
        is_os_next_enabled
    );
}

void Editor::draw_on_key()
{
    Animation::frame i = Mw::animation->get_frame_at(layer_pos, frame_pos);
    Animation::frame j = Mw::animation->get_frame_at(layer_pos, frame_pos);

    // Init
    if (j.is_empty) Mw::animation->init_frame(&j, (stroke.first() - offset) / scale);

    // Resize frame
    QRect bb(
        (stroke.boundingRect().topLeft() - offset) / scale,
        stroke.boundingRect().size() / scale
    );
    if (bb.right() > j.dimensions.right()) Mw::animation->resize_frame(&j, RIGHT, bb.right());
    if (bb.bottom() > j.dimensions.bottom()) Mw::animation->resize_frame(&j, BOTTOM, bb.bottom());
    if (bb.left() < j.dimensions.left()) Mw::animation->resize_frame(&j, LEFT, bb.left());
    if (bb.top() < j.dimensions.top()) Mw::animation->resize_frame(&j, TOP, bb.top());

    // Draw on key
    frame_painter.begin(&j.image);
    frame_painter.translate(-offset/scale - j.dimensions.topLeft());
    frame_painter.scale(1/scale, 1/scale);

    switch (tool) {
        case PEN :
            frame_painter.setPen(QPen(pen_tool.color(), pen_tool.width() * scale));
            if (stroke.count() == 1)
                frame_painter.drawPoint(stroke.first());
            else if (stroke.count() > 1)
                frame_painter.drawPolyline(stroke);
            break;
        case LASSOFILL:
            frame_painter.setPen(Qt::transparent);
            frame_painter.setBrush(lassofill_tool);
            frame_painter.drawPolygon(stroke);
            break;
    }

    frame_painter.end();
    stroke.clear();

    Mw::undostack->push(new ModifyFrameCommand(i, j, layer_pos, frame_pos));
}

void Editor::knockback()
{
    if (state != IDLE || !Mw::animation->is_frame_at(layer_pos, frame_pos)) return;
    Animation::frame i = Mw::animation->get_frame_at(layer_pos, frame_pos);
    Animation::frame j = Mw::animation->get_frame_at(layer_pos, frame_pos);

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

    Mw::undostack->push(new ModifyFrameCommand(i, j, layer_pos, frame_pos));
}

void Editor::play_step()
{
    if (frame_pos + 1 <= Mw::animation->get_last_anim_pos())
    {
        frame_pos += 1;
        Mw::editor->update();
        Mw::timeline->update_all_frames();
        Mw::timeline->update();
    } else {
        if (is_play_loop_enabled)
        {
            frame_pos = 0;
            Mw::editor->update();
            Mw::timeline->update_all_frames();
            Mw::timeline->update();
        }
        else stop();
    }
}

void Editor::play_from(int begin, bool loop)
{
    if (Mw::animation->is_anim_empty() || state != IDLE) return;

    if (loop) is_play_loop_enabled = true;
    temp_is_os_enabled = is_os_enabled;
    frame_pos = begin;
    is_os_enabled = false;
    state = PLAYING;
    playing_timer->start(1000/Mw::animation->FPS);
    Mw::editor->update();
    Mw::timeline->update();
}

void Editor::stop()
{
    if (state != PLAYING) return;
    is_os_enabled = temp_is_os_enabled;
    state = IDLE;
    is_play_loop_enabled = false;
    playing_timer->stop();
    Mw::update_editor_and_timeline();
}

void Editor::copy()
{
    if (state != IDLE || !Mw::animation->is_frame_at(layer_pos, frame_pos)) return;
    clipboard = Mw::animation->get_frame_at(layer_pos, frame_pos);
    is_internal_clipboard_empty = false;
}

void Editor::cut()
{
    if (state != IDLE || !Mw::animation->is_frame_at(layer_pos, frame_pos)) return;
    clipboard = Mw::animation->get_frame_at(layer_pos, frame_pos);
    is_internal_clipboard_empty = false;
    Mw::undostack->push(new RemoveFrameCommand(layer_pos, frame_pos));
}

void Editor::paste()
{
    if (state != IDLE || is_internal_clipboard_empty) return;
    if (!Mw::animation->is_frame_at(layer_pos, frame_pos))
        Mw::undostack->push(new AddFrameCommand(clipboard, layer_pos, frame_pos));
    else {
        Animation::frame i = Mw::animation->get_frame_at(layer_pos, frame_pos);
        Mw::undostack->push(new ModifyFrameCommand(i, clipboard, layer_pos, frame_pos));
    }
}
