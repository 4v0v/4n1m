#include "editor.h"

Editor::Editor(): QWidget(nullptr)
{
    setCursor(Qt::CrossCursor);

    onion_skins = QImage(Mw::animation->dimensions, QImage::Format_ARGB32);

    tool_pen         = new Tool_pen        ();
    tool_lassofill   = new Tool_lassofill  ();
    tool_eraser      = new Tool_eraser     ();
    tool_knockback   = new Tool_knockback  ();
    tool_colorpicker = new Tool_colorpicker();
    tool_move        = new Tool_move       ();
}

void Editor::mousePressEvent(QMouseEvent* e)
{
    bool is_mouse_in_drawable_region = QRect(offset, Mw::animation->dimensions*scale).contains(e->pos());

    if (!is_mouse_in_drawable_region) return;

    // quick tool switch
    previous_tool = current_tool;

    if      (e->button() == Qt::RightButton ) current_tool = COLORPICKER;
    else if (e->button() == Qt::MiddleButton) current_tool = MOVE;

    switch (current_tool)
    {
        case PEN        : tool_pen        ->press(e); break;
        case LASSOFILL  : tool_lassofill  ->press(e); break;
        case ERASER     : tool_eraser     ->press(e); break;
        case KNOCKBACK  : tool_knockback  ->press(e); break;
        case COLORPICKER: tool_colorpicker->press(e); break;
        case MOVE       : tool_move       ->press(e); break;
    }
}

void Editor::mouseMoveEvent(QMouseEvent* e)
{
    switch (current_tool)
    {
        case PEN        : tool_pen        ->move(e); break;
        case LASSOFILL  : tool_lassofill  ->move(e); break;
        case ERASER     : tool_eraser     ->move(e); break;
        case KNOCKBACK  : tool_knockback  ->move(e); break;
        case COLORPICKER: tool_colorpicker->move(e); break;
        case MOVE       : tool_move       ->move(e); break;
    }
}

void Editor::mouseReleaseEvent(QMouseEvent* e)
{
    switch (current_tool)
    {
        case PEN        : tool_pen        ->release(e); break;
        case LASSOFILL  : tool_lassofill  ->release(e); break;
        case ERASER     : tool_eraser     ->release(e); break;
        case KNOCKBACK  : tool_knockback  ->release(e); break;
        case COLORPICKER: tool_colorpicker->release(e); break;
        case MOVE       : tool_move       ->release(e); break;
    }

    // quick tool switch
    if (previous_tool != current_tool) current_tool = previous_tool;
}

void Editor::resizeEvent(QResizeEvent *e)
{
    int w = Mw::animation->dimensions.width()  * scale;
    int h = Mw::animation->dimensions.height() * scale;

    offset.setX(width() /2 - w/2);
    offset.setY(height()/2 - h/2);

    QWidget::resizeEvent(e);
}

void Editor::wheelEvent(QWheelEvent* e){
    if (state != IDLE) return;

    static float MIN_SCALE = 0.7;
    static float MAX_SCALE = 3.0;

    float w = Mw::animation->dimensions.width()  /2 * .1;
    float h = Mw::animation->dimensions.height() /2 * .1;
    auto  o = QPoint(w, h);

    if (QApplication::keyboardModifiers() == Qt::CTRL)
    {
        if        (e->angleDelta().y() < 0 && scale > MIN_SCALE) {
            scale  -= .1;
            offset +=  o;
        } else if (e->angleDelta().y() > 0 && scale < MAX_SCALE) {
            scale  += .1;
            offset -=  o;
        }

        update();

    } else {
        if      (e->angleDelta().y() < 0) goto_next_pos();
        else if (e->angleDelta().y() > 0) goto_prev_pos();
    }
}

void Editor::paintEvent(QPaintEvent*)
{
    static QPainter painter;

    painter.begin(this);

    // editor background
    painter.setPen  (bg_color);
    painter.setBrush(bg_color);
    painter.drawRect(rect());

    // transform editor
    painter.translate(offset + tool_move->moving_offset_delta - tool_move->moving_offset);
    painter.scale(scale, scale);

    // editor background frame
    painter.setPen  (bg_color);
    painter.setBrush(paper_color);
    painter.drawRect(-1, -1, Mw::animation->dimensions.width() + 2, Mw::animation->dimensions.height() + 2);

    // onionskins
    if (is_os_enabled) painter.drawImage(0,0, onion_skins);

    // sorted frames from all layers at current position
    auto keys = Mw::animation->layers.keys();
    auto rit  = keys.crbegin();

    while (rit != keys.crend())
    {
        auto frame = Mw::animation->has_frame_at(*rit, frame_pos) ?
            Mw::animation->get_frame_at(*rit, frame_pos) :
            Mw::animation->get_prev_frame_at(*rit, frame_pos);

        painter.setOpacity(Mw::animation->get_layer_at(*rit).opacity/100.0);
        painter.drawImage(frame.dimensions.topLeft(), frame.image);

        rit++;
    }

    // tool preview
    QImage* preview;

    switch (current_tool)
    {
        case PEN        : preview = tool_pen        ->preview(); break;
        case LASSOFILL  : preview = tool_lassofill  ->preview(); break;
        case ERASER     : preview = tool_eraser     ->preview(); break;
        case KNOCKBACK  : preview = tool_knockback  ->preview(); break;
        case COLORPICKER: preview = tool_colorpicker->preview(); break;
        case MOVE       : preview = tool_move       ->preview(); break;
    }

    if (preview) painter.drawImage(0, 0, *preview);

    painter.end();
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
    if (state != IDLE || !Mw::animation->has_frame_at(layer_pos, frame_pos)) return;

    Mw::undostack->push(new ModifyFrameCommand(Mw::animation->get_frame_at(layer_pos, frame_pos), Animation::frame{}, layer_pos, frame_pos));
}

void Editor::remove_frame_at_current_pos()
{
    if (state != IDLE || !Mw::animation->has_frame_at(layer_pos, frame_pos)) return;

    Mw::undostack->push(new RemoveFrameCommand(layer_pos, frame_pos));
}

void Editor::insert_frame_at_current_pos()
{
    if ( state != IDLE || Mw::animation->is_animation_empty() || Mw::animation->is_layer_empty(layer_pos) || frame_pos >= Mw::animation->get_last_pos(layer_pos)) return;

    Mw::undostack->push(new InsertFrameCommand(layer_pos, Mw::animation->has_frame_at(layer_pos, frame_pos) ? frame_pos + 1 : frame_pos));
}

void Editor::uninsert_frame_at_current_pos()
{
    if ( state != IDLE || Mw::animation->is_animation_empty() || Mw::animation->is_layer_empty(layer_pos) || frame_pos >= Mw::animation->get_last_pos(layer_pos)) return;

    if (Mw::animation->has_frame_at(layer_pos, frame_pos) && !Mw::animation->has_frame_at(layer_pos, frame_pos + 1))
        Mw::undostack->push(new UninsertFrameCommand(layer_pos, frame_pos + 1));
    else if (!Mw::animation->has_frame_at(layer_pos, frame_pos))
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
    int current_or_prev_pos = Mw::animation->has_frame_at(layer_pos, frame_pos) ? frame_pos : Mw::animation->get_prev_pos(layer_pos, frame_pos);

    onion_skins = Mw::animation->create_onionskins_at(
        layer_pos,
        current_or_prev_pos != -1 ? current_or_prev_pos : 0,
        is_os_loop_enabled,
        is_os_prev_enabled ? nb_prev_os: 0,
        is_os_next_enabled ? nb_next_os: 0
    );
}

void Editor::cut()
{
    if (state != IDLE || !Mw::animation->has_frame_at(layer_pos, frame_pos)) return;

    clipboard                   = Mw::animation->get_frame_at(layer_pos, frame_pos);
    is_internal_clipboard_empty = false;

    Mw::undostack->push(new RemoveFrameCommand(layer_pos, frame_pos));
}

void Editor::copy()
{
    if (state != IDLE || !Mw::animation->has_frame_at(layer_pos, frame_pos)) return;

    clipboard                   = Mw::animation->get_frame_at(layer_pos, frame_pos);
    is_internal_clipboard_empty = false;
}

void Editor::paste()
{
    if (state != IDLE || is_internal_clipboard_empty) return;

    if (!Mw::animation->has_frame_at(layer_pos, frame_pos))
        Mw::undostack->push(new AddFrameCommand(clipboard, layer_pos, frame_pos));
    else {
        Animation::frame i = Mw::animation->get_frame_at(layer_pos, frame_pos);
        Mw::undostack->push(new ModifyFrameCommand(i, clipboard, layer_pos, frame_pos));
    }
}
