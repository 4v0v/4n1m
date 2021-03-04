#include "editor.h"

Editor::Editor(): QWidget(nullptr)
{
    setCursor(Qt::CrossCursor);
    onion_skins = QImage(Mw::animation->dimensions, QImage::Format_ARGB32);
    tools_preview = QImage(QSize(Mw::animation->dimensions.width()+1, Mw::animation->dimensions.height() +1), QImage::Format_ARGB32);
    tools_preview.fill(Qt::transparent);

    tool_pen = new Tool_pen();
    tool_lassofill = new Tool_lassofill();
    tool_eraser = new Tool_eraser();
    tool_colorpicker  = new Tool_colorpicker();
}

void Editor::mousePressEvent(QMouseEvent* e)
{
    if (state != IDLE) return;

    switch (e->button())
    {
        case Qt::LeftButton:
            if (!QRect(offset, Mw::animation->dimensions*scale).contains(e->pos())) return;
            state = SCRIBBLING;

            switch (tool)
            {
                case PEN: tool_pen->press(e); break;
                case LASSOFILL: tool_lassofill->press(e); break;
                case ERASER: tool_eraser->press(e); break;
                case COLORPICKER: tool_colorpicker->press(e); break;
            }
            break;

        case Qt::RightButton:
        case Qt::MiddleButton:
            state = MOVING;
            moving_offset = e->pos();
            moving_offset_delta = e->pos();
            break;

        default:
            break;
    }

    update();
    Mw::timeline->update();
}

void Editor::mouseMoveEvent(QMouseEvent* e)
{
    switch (state)
    {
        case SCRIBBLING:
            switch (tool)
            {
                case PEN: tool_pen->move(e); break;
                case LASSOFILL: tool_lassofill->move(e); break;
                case ERASER: tool_eraser->move(e); break;
                case COLORPICKER: tool_colorpicker->move(e); break;
            }
            break;

        case MOVING:
            moving_offset_delta = e->pos();
            update(rect());
            break;

        case IDLE:
            break;

        default:
            break;
    }
}

void Editor::mouseReleaseEvent(QMouseEvent* e)
{
    switch (state)
    {
        case SCRIBBLING:
            switch (tool)
            {
                case PEN: tool_pen->release(e); break;
                case LASSOFILL: tool_lassofill->release(e); break;
                case ERASER: tool_eraser->release(e); break;
                case COLORPICKER: tool_colorpicker->release(e); break;
            }

            tools_preview.fill(Qt::transparent); // reset tools_preview
            state = IDLE;
            break;

        case MOVING:
            offset += moving_offset_delta - moving_offset;
            moving_offset = moving_offset_delta;
            state = IDLE;
            break;

        case IDLE:
            break;

        default:
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
        if (e->angleDelta().y() < 0) goto_next_pos();
        else if (e->angleDelta().y() > 0) goto_prev_pos();
    }
}

void Editor::paintEvent(QPaintEvent*)
{
    widget_painter.begin(this);

    //editor background
    Mw::set_painter_colors(&widget_painter, bg_color);
    widget_painter.drawRect(rect());

    //transform editor
    widget_painter.translate(offset + moving_offset_delta - moving_offset);
    widget_painter.scale(scale, scale);

    //editor background frame
    Mw::set_painter_colors(&widget_painter, bg_color, img_bg_color);
    widget_painter.drawRect(0, 0, Mw::animation->dimensions.width() + 1, Mw::animation->dimensions.height() + 1);

    //onionskins
    if (is_os_enabled) widget_painter.drawImage(0,0, onion_skins);

    //sorted frames from all layers at current position
    QList<int> layer_keys = Mw::animation->layers.keys();
    QList<int>::const_reverse_iterator ri = layer_keys.crbegin();
    while(ri != layer_keys.crend()) {
        auto layer_pos = *ri;
        widget_painter.setOpacity(Mw::animation->get_layer_at(layer_pos).opacity/100.0);
        Animation::frame frame = Mw::animation->is_frame_at(layer_pos, frame_pos)?
            Mw::animation->get_frame_at(layer_pos, frame_pos):
            Mw::animation->get_prev_frame_at(layer_pos, frame_pos);
        widget_painter.drawImage(frame.dimensions.topLeft(), frame.image);
        ri++;
    }

    //tool preview
    if (state == SCRIBBLING) {
        tools_preview.fill(Qt::transparent);

        switch (tool) {
            case PEN: tool_pen->preview(); break;
            case LASSOFILL: tool_lassofill->preview(); break;
            case ERASER: tool_eraser->preview(); break;
            case COLORPICKER: tool_colorpicker->preview(); break;
        }

        widget_painter.drawImage(0,0, tools_preview);
    }

    //reset painter
    widget_painter.setOpacity(1);
    widget_painter.resetTransform();
    widget_painter.end();
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

void Editor::clear_current_layer()
{
    if (state != IDLE || Mw::animation->is_layer_empty(layer_pos)) return;
    Mw::animation->clear_layer_at(layer_pos);
    Mw::undostack->clear();
    Mw::update_all();
}

void Editor::clear_frame_at_current_pos()
{
    if (state != IDLE || !Mw::animation->is_frame_at(layer_pos, frame_pos)) return;
    Mw::undostack->push(new ModifyFrameCommand(Mw::animation->get_frame_at(layer_pos, frame_pos), Animation::frame{}, layer_pos, frame_pos));
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
        Mw::animation->is_animation_empty() ||
        Mw::animation->is_layer_empty(layer_pos) ||
        frame_pos >= Mw::animation->get_last_pos(layer_pos)
    ) return;

    Mw::undostack->push(new InsertFrameCommand(layer_pos, Mw::animation->is_frame_at(layer_pos, frame_pos) ? frame_pos + 1 : frame_pos));
}

void Editor::uninsert_frame_at_current_pos()
{
    if (
        state != IDLE ||
        Mw::animation->is_animation_empty() ||
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
    if (p < 0) return;
    frame_pos = p;
    Mw::update_all();
}

void Editor::create_onions_at_current_pos()
{
    int f_pos = Mw::animation->is_frame_at(layer_pos, frame_pos) ? frame_pos : Mw::animation->get_prev_pos(layer_pos, frame_pos);
    onion_skins = Mw::animation->create_onionskins_at(
        layer_pos,
        f_pos != -1 ? f_pos : 0,
        is_os_loop_enabled,
        is_os_prev_enabled ? nb_prev_os: 0,
        is_os_next_enabled ? nb_next_os: 0
    );
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
