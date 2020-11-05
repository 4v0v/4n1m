#pragma once

#include "mw.h"
#include "animation.h"
#include "editor.h"
#include "timeline.h"

class ModifyFrameCommand : public QUndoCommand
{
public:
    ModifyFrameCommand(Animation::frame i, Animation::frame j, int l, int p);
    void undo() override;
    void redo() override;
    Animation::frame old_frame;
    Animation::frame new_frame;
    int layer;
    int pos;
};

class AddFrameCommand : public QUndoCommand
{
public:
    AddFrameCommand(Animation::frame, int l, int p);
    void undo() override;
    void redo() override;
    Animation::frame new_frame;
    int layer;
    int pos;
};

class RemoveFrameCommand : public QUndoCommand
{
public:
    RemoveFrameCommand(int l, int p);
    void undo() override;
    void redo() override;
    Animation::frame old_frame;
    int layer;
    int pos;
};

class InsertFrameCommand : public QUndoCommand
{
public:
    InsertFrameCommand(int l, int p);
    void undo() override;
    void redo() override;
    int layer;
    int pos;
};

class UninsertFrameCommand : public QUndoCommand
{
public:
    UninsertFrameCommand(int l, int p);
    void undo() override;
    void redo() override;
    int layer;
    int pos;
};



