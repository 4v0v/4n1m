#include "commands.h"
#include "object.h"
#include "timeline.h"
#include "editor.h"
#include <QDebug>

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
    object->getEditor()->update();
    object->getTimeline()->update();
}

void ModifyImageCommand::redo()
{
    object->addKeyframeAt(layer, pos, newImg);
    object->getEditor()->update();
    object->getTimeline()->update();
    setText("modify image");
}
