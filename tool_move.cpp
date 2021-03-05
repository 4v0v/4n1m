#include "tool_move.h"
#include "editor.h"

void Tool_move::press(QMouseEvent *e) {
    moving_offset = e->pos();
    moving_offset_delta = e->pos();
}

void Tool_move::move(QMouseEvent *e) {
    moving_offset_delta = e->pos();
    Mw::editor->update();
};

void Tool_move::release(QMouseEvent *) {
    Mw::editor->offset += moving_offset_delta - moving_offset;
    moving_offset = moving_offset_delta;
};

void Tool_move::preview(QImage*) {

};
