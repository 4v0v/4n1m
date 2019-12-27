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
    layer = l;
    pos = p;
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
    layer = l;
    pos = p;
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
    layer = l;
    pos = p;
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

InsertFrameCommand::InsertFrameCommand(int l, int p, Object* o, QUndoCommand* parent): QUndoCommand(parent)
{
    layer = l;
    pos = p;
    object = o;
}

void InsertFrameCommand::undo()
{
    object->foreachKeyframe(layer, [this](int i){
        QImage img = object->getKeyframeImageAt(layer, i)->copy();
        object->removeKeyframeAt(layer, i);
        object->addKeyframeAt(layer, i - 1, img);
    }, object->isKeyframe(layer, pos) ? pos + 1 : pos);
    object->editor()->update();
    object->timeline()->update();
}

void InsertFrameCommand::redo()
{
    object->foreachKeyframeRevert(layer, [this](int i){
            QImage img = object->getKeyframeImageAt(layer, i)->copy();
            object->addKeyframeAt(layer, i + 1, img);
            object->removeKeyframeAt(layer, i);
    }, object->isKeyframe(layer, pos) ? pos + 1 : pos);
    object->editor()->update();
    object->timeline()->update();
    setText("insert frame");
}

RemoveFrameCommand::RemoveFrameCommand(int l, int p, Object* o, QUndoCommand* parent): QUndoCommand(parent)
{
    layer = l;
    pos = p;
    object = o;
}

void RemoveFrameCommand::undo()
{
    object->foreachKeyframeRevert(layer, [this](int i){
        QImage img = object->getKeyframeImageAt(layer, i)->copy();
        object->addKeyframeAt(layer, i + 1, img);
        object->removeKeyframeAt(layer, i);
    }, object->isKeyframe(layer, pos) ? pos + 1 : pos);
    object->editor()->update();
    object->timeline()->update();
}

void RemoveFrameCommand::redo()
{
    object->foreachKeyframe(layer, [this](int i){
        QImage img = object->getKeyframeImageAt(layer, i)->copy();
        object->removeKeyframeAt(layer, i);
        object->addKeyframeAt(layer, i - 1, img);
    }, object->isKeyframe(layer, pos) ? pos + 1 : pos);
    object->editor()->update();
    object->timeline()->update();
    setText("remove frame");
}

