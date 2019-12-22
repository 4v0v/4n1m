#include "object.h"
#include "editor.h"
#include "timeline.h"
#include "preview.h"
#include "commands.h"
#include "titlebar.h"
#include "menubar.h"

ModifyImageCommand::ModifyImageCommand(QImage i, QImage j, int l, int p, Object* o, QUndoCommand* parent): QUndoCommand(parent)
{
    oldImg = i;
    newImg = j;
    pos = p;
    layer = l;
    object = o;
}

void ModifyImageCommand::undo()
{
    object->addKeyframeAt(layer, pos, oldImg);
    object->editor()->update();
    object->timeline()->update();
}

void ModifyImageCommand::redo()
{
    object->addKeyframeAt(layer, pos, newImg);
    object->editor()->update();
    object->timeline()->update();
    setText("modify image");
}

AddImageCommand::AddImageCommand(QImage i, int l, int p, Object* o, QUndoCommand* parent): QUndoCommand(parent)
{
    newImg = i;
    pos = p;
    layer = l;
    object = o;
}

void AddImageCommand::undo()
{
    object->removeKeyframeAt(layer, pos);
    object->editor()->update();
    object->timeline()->update();
}

void AddImageCommand::redo()
{
    object->addKeyframeAt(layer, pos, newImg);
    object->editor()->update();
    object->timeline()->update();
    setText("add image");
}

RemoveImageCommand::RemoveImageCommand(QImage i, int l, int p, Object* o, QUndoCommand* parent): QUndoCommand(parent)
{
    oldImg = i;
    pos = p;
    layer = l;
    object = o;
}

void RemoveImageCommand::undo()
{
    object->addKeyframeAt(layer, pos, oldImg);
    object->editor()->update();
    object->timeline()->update();
}

void RemoveImageCommand::redo()
{
    object->removeKeyframeAt(layer, pos);
    object->editor()->update();
    object->timeline()->update();
    setText("remove image");
}
