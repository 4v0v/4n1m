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

//AddImageCommand::AddImageCommand(QImage i, int p, Object* o, QUndoCommand* parent): QUndoCommand(parent)
//{
//    newImg = i;
//    pos = p;
//    object = o;
//}

//void AddImageCommand::undo()
//{
//    object->removeKeyframeAt(pos);
//    object->getEditor()->update();
//    object->getTimeline()->update();
//}

//void AddImageCommand::redo()
//{
//    object->addKeyframeAt(pos, newImg);
//    object->getEditor()->update();
//    object->getTimeline()->update();
//    setText("add image");
//}

//RemoveImageCommand::RemoveImageCommand(QImage i, int p, Object* o, QUndoCommand* parent): QUndoCommand(parent)
//{
//    oldImg = i;
//    pos = p;
//    object = o;
//}

//void RemoveImageCommand::undo()
//{
//    object->addKeyframeAt(pos, oldImg);
//    object->getEditor()->update();
//    object->getTimeline()->update();
//}

//void RemoveImageCommand::redo()
//{
//    object->removeKeyframeAt(pos);
//    object->getEditor()->update();
//    object->getTimeline()->update();
//    setText("remove image");
//}

//ChangeFrameCommand::ChangeFrameCommand(int i, int j, Timeline* t, QUndoCommand* parent): QUndoCommand(parent)
//{
//    oldPos = i;
//    newPos = j;
//    timeline = t;
//}
//void ChangeFrameCommand::undo(){}
//void ChangeFrameCommand::redo(){}

