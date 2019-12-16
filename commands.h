#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>
#include <QImage>

class Object;
class Timeline;
class Editor;
class ModifyImageCommand : public QUndoCommand
{
public:
    ModifyImageCommand(QImage, QImage, int, int, Object*, QUndoCommand* = nullptr);
    void undo() override;
    void redo() override;

private:
    Object *object;
    QImage oldImg;
    QImage newImg;
    int layer;
    int pos;
};

#endif
