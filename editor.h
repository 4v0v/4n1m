#pragma once

#include "mw.h"
#include "animation.h"
#include "toolbar.h"
#include "timeline.h"
#include "commands.h"
#include "tool_pen.h"
#include "tool_lassofill.h"
#include "tool_eraser.h"
#include "tool_colorpicker.h"
#include "tool_move.h"
#include "tool_knockback.h"

class Editor : public QWidget
{
    Q_OBJECT
public:
    Editor();
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);
    virtual void paintEvent(QPaintEvent* e);
    virtual void wheelEvent(QWheelEvent* e);
    virtual void resizeEvent(QResizeEvent* e);

    void set_pen_color(QColor c) { tool_pen->pen_tool.setColor(c); }
    void set_brush_color(QColor c) { tool_lassofill->lassofill_tool.setColor(c); }
    void set_pen_size(int s) { if (state != IDLE) return; tool_pen->pen_tool.setWidth(s); }
    void set_eraser_size(int s) { if (state != IDLE) return; tool_eraser->eraser_tool.setWidth(s); }
    void set_tool(Tool t) { if (state != IDLE) return; tool = t; }
    void toggle_copy_prev_frame() { if (state != IDLE) return;  is_copy_prev_frame = !is_copy_prev_frame; Mw::update_all(); }
    void toggle_onion_skin() { if (state != IDLE) return; is_os_enabled = !is_os_enabled; Mw::update_all(); }
    void toggle_onion_skin_loop() { if (state != IDLE) return;is_os_loop_enabled = !is_os_loop_enabled; Mw::update_all(); }
    void toggle_onion_skin_prev() { if (state != IDLE) return;is_os_prev_enabled = !is_os_prev_enabled; Mw::update_all(); }
    void toggle_onion_skin_next() { if (state != IDLE) return;is_os_next_enabled = !is_os_next_enabled; Mw::update_all(); }
    void goto_pos(int l, int p);
    void goto_next_pos() { goto_pos(layer_pos, frame_pos+1); }
    void goto_prev_pos() { goto_pos(layer_pos, frame_pos-1); }
    void goto_next_layer() { goto_pos(layer_pos+1, frame_pos); }
    void goto_prev_layer() { goto_pos(layer_pos-1, frame_pos); }
    void clear_frame_at_current_pos();
    void clear_current_layer();
    void remove_frame_at_current_pos();
    void insert_frame_at_current_pos();
    void uninsert_frame_at_current_pos();
    void create_onions_at_current_pos();
    void stop();
    void copy();
    void cut();
    void paste();

    bool is_os_enabled = true;
    bool is_os_loop_enabled = false;
    bool is_os_prev_enabled = true;
    bool is_os_next_enabled = true;
    bool is_copy_prev_frame = false;
    bool is_play_loop_enabled = false;
    bool is_internal_clipboard_empty = true;

    int nb_prev_os = 2;
    int nb_next_os = 2;

    int frame_pos = 0;
    int layer_pos = 0;
    State state = IDLE;
    Tool tool = PEN;
    Tool previous_tool;

    double scale = 1.75;
    QPoint offset = QPoint(300, 50);

    QColor bg_color = Qt::black;
    QColor paper_color = Qt::lightGray;

    QImage onion_skins;
    QImage tools_preview;

    Animation::frame clipboard;

    Tool_pen* tool_pen;
    Tool_lassofill* tool_lassofill;
    Tool_eraser* tool_eraser;
    Tool_colorpicker* tool_colorpicker;
    Tool_move* tool_move;
    Tool_knockback* tool_knockback;
};
