#include "commands.h"

ModifyFrameCommand::ModifyFrameCommand(Animation::frame i, Animation::frame j, int l, int p): QUndoCommand()
{
    old_frame.image      = i.image.copy();
    old_frame.dimensions = i.dimensions;
    old_frame.is_empty   = i.is_empty;

    new_frame.image      = j.image.copy();
    new_frame.dimensions = j.dimensions;
    new_frame.is_empty   = j.is_empty;

    layer = l;
    pos   = p;
}

void ModifyFrameCommand::undo()
{
    Mw::animation->remove_frame_at(layer, pos);
    Mw::animation->add_frame_at(layer, pos, old_frame);
    Mw::update_all();
}

void ModifyFrameCommand::redo()
{
    Mw::animation->remove_frame_at(layer, pos);
    Mw::animation->add_frame_at(layer, pos, new_frame);
    Mw::update_all();
}

AddFrameCommand::AddFrameCommand(Animation::frame i, int l, int p): QUndoCommand()
{
    new_frame.image      = i.image.copy();
    new_frame.dimensions = i.dimensions;
    new_frame.is_empty   = i.is_empty;

    layer = l;
    pos   = p;
}

void AddFrameCommand::undo()
{
    Mw::animation->remove_frame_at(layer, pos);
    Mw::update_all();
}

void AddFrameCommand::redo()
{
    Mw::animation->add_frame_at(layer, pos, new_frame);
    Mw::update_all();
}

RemoveFrameCommand::RemoveFrameCommand(int l, int p): QUndoCommand()
{
    Animation::frame i = Mw::animation->get_frame_at(l, p);

    old_frame.image      = i.image.copy();
    old_frame.dimensions = i.dimensions;
    old_frame.is_empty   = i.is_empty;

    layer = l;
    pos   = p;
}

void RemoveFrameCommand::undo()
{
    Mw::animation->add_frame_at(layer, pos, old_frame);
    Mw::update_all();
}

void RemoveFrameCommand::redo()
{
    Mw::animation->remove_frame_at(layer, pos);
    Mw::update_all();
}

InsertFrameCommand::InsertFrameCommand(int l, int p): QUndoCommand()
{
    layer = l;
    pos   = p;
}

void InsertFrameCommand::undo()
{
    Mw::animation->foreach_frame_pos(layer, [this](int i) {
        Animation::frame f = Mw::animation->get_frame_at(layer, i);
        Mw::animation->add_frame_at(layer, i - 1, f);
        Mw::animation->remove_frame_at(layer, i);
    }, pos);

    Mw::update_all();
}

void InsertFrameCommand::redo()
{
    Mw::animation->foreach_frame_pos_revert(layer, [this](int i) {
        Animation::frame f = Mw::animation->get_frame_at(layer, i);
        Mw::animation->add_frame_at(layer, i + 1, f);
        Mw::animation->remove_frame_at(layer, i);
    }, pos);

    Mw::update_all();
}

UninsertFrameCommand::UninsertFrameCommand(int l, int p): QUndoCommand()
{
    layer = l;
    pos   = p;
}

void UninsertFrameCommand::undo()
{
    Mw::animation->foreach_frame_pos_revert(layer, [this](int i) {
        Animation::frame f = Mw::animation->get_frame_at(layer, i);
        Mw::animation->add_frame_at(layer, i + 1, f);
        Mw::animation->remove_frame_at(layer, i);
    }, pos);

    Mw::update_all();
}

void UninsertFrameCommand::redo()
{
    Mw::animation->foreach_frame_pos(layer, [this](int i) {
        Animation::frame f = Mw::animation->get_frame_at(layer, i);
        Mw::animation->add_frame_at(layer, i - 1, f);
        Mw::animation->remove_frame_at(layer, i);
    }, pos);

    Mw::update_all();
}
