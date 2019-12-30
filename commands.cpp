#include "animation.h"
#include "preview.h"
#include "commands.h"
#include "mainWidgets/editor.h"
#include "mainWidgets/timeline.h"
#include "mainWidgets/titlebar.h"

ModifyImageCommand::ModifyImageCommand(QImage i, QImage j, int l, int p, Animation* o, QUndoCommand* parent): QUndoCommand(parent)
{
    oldImg = i;
    newImg = j;
    layer = l;
    pos = p;
    animation = o;
}

void ModifyImageCommand::undo()
{
    animation->addKeyAt(layer, pos, oldImg);
    animation->editor()->update();
    animation->timeline()->update();
}

void ModifyImageCommand::redo()
{
    animation->addKeyAt(layer, pos, newImg);
    animation->editor()->update();
    animation->timeline()->update();
    setText("modify image");
}

AddImageCommand::AddImageCommand(QImage i, int l, int p, Animation* o, QUndoCommand* parent): QUndoCommand(parent)
{
    newImg = i;
    layer = l;
    pos = p;
    animation = o;
}

void AddImageCommand::undo()
{
    animation->timeline()->getFrameWidgetAt(layer, pos)->toggleIsKey();
    animation->removeKeyAt(layer, pos);
    animation->editor()->update();
    animation->timeline()->update();
}

void AddImageCommand::redo()
{
    animation->timeline()->getFrameWidgetAt(layer, pos)->toggleIsKey();
    animation->addKeyAt(layer, pos, newImg);
    animation->editor()->update();
    animation->timeline()->update();
    setText("add image");
}

RemoveImageCommand::RemoveImageCommand(QImage i, int l, int p, Animation* o, QUndoCommand* parent): QUndoCommand(parent)
{
    oldImg = i;
    layer = l;
    pos = p;
    animation = o;
}

void RemoveImageCommand::undo()
{
    animation->timeline()->getFrameWidgetAt(layer, pos)->toggleIsKey();
    animation->addKeyAt(layer, pos, oldImg);
    animation->editor()->update();
    animation->timeline()->update();
}

void RemoveImageCommand::redo()
{
    animation->timeline()->getFrameWidgetAt(layer, pos)->toggleIsKey();
    animation->removeKeyAt(layer, pos);
    animation->editor()->update();
    animation->timeline()->update();
    setText("remove image");
}

InsertFrameCommand::InsertFrameCommand(int l, int p, Animation* o, QUndoCommand* parent): QUndoCommand(parent)
{
    layer = l;
    pos = p;
    animation = o;
}

void InsertFrameCommand::undo()
{
    animation->foreachKey(layer, [this](int i){
        if (i > 198) return;
        animation->timeline()->getFrameWidgetAt(layer, i)->toggleIsKeyNoAnim();
        animation->timeline()->getFrameWidgetAt(layer, i - 1)->toggleIsKeyNoAnim();
        QImage img = animation->copyImageAt(layer, i);
        animation->removeKeyAt(layer, i);
        animation->addKeyAt(layer, i - 1, img);
    }, animation->isKey(layer, pos) ? pos + 1 : pos);
    animation->editor()->update();
    animation->timeline()->update();
}

void InsertFrameCommand::redo()
{
    animation->foreachKeyRevert(layer, [this](int i){
        if (i > 198) return;
        animation->timeline()->getFrameWidgetAt(layer, i + 1)->toggleIsKeyNoAnim();
        animation->timeline()->getFrameWidgetAt(layer, i)->toggleIsKeyNoAnim();
        QImage img = animation->copyImageAt(layer, i);
        animation->addKeyAt(layer, i + 1, img);
        animation->removeKeyAt(layer, i);
    }, animation->isKey(layer, pos) ? pos + 1 : pos);

    animation->editor()->update();
    animation->timeline()->update();
    setText("insert frame");
}

RemoveFrameCommand::RemoveFrameCommand(int l, int p, Animation* o, QUndoCommand* parent): QUndoCommand(parent)
{
    layer = l;
    pos = p;
    animation = o;
}

void RemoveFrameCommand::undo()
{
    animation->foreachKeyRevert(layer, [this](int i){
        if (i > 198) return;
        animation->timeline()->getFrameWidgetAt(layer, i + 1)->toggleIsKeyNoAnim();
        animation->timeline()->getFrameWidgetAt(layer, i)->toggleIsKeyNoAnim();
        QImage img = animation->copyImageAt(layer, i);
        animation->addKeyAt(layer, i + 1, img);
        animation->removeKeyAt(layer, i);
    }, animation->isKey(layer, pos) ? pos + 1 : pos);
    animation->editor()->update();
    animation->timeline()->update();
}

void RemoveFrameCommand::redo()
{
    animation->foreachKey(layer, [this](int i){
        if (i > 198) return;
        animation->timeline()->getFrameWidgetAt(layer, i)->toggleIsKeyNoAnim();
        animation->timeline()->getFrameWidgetAt(layer, i - 1)->toggleIsKeyNoAnim();
        QImage img = animation->copyImageAt(layer, i);
        animation->removeKeyAt(layer, i);
        animation->addKeyAt(layer, i - 1, img);
    }, animation->isKey(layer, pos) ? pos + 1 : pos);
    animation->editor()->update();
    animation->timeline()->update();
    setText("remove frame");
}

