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
    ModifyImageCommand(QImage, QImage, int, Object*, QUndoCommand* = nullptr);
    void undo() override;
    void redo() override;

private:
    Object *object;
    QImage oldImg;
    QImage newImg;
    int pos;
};

class AddImageCommand : public QUndoCommand
{
public:
    AddImageCommand(QImage, int, Object*, QUndoCommand* = nullptr);
    void undo() override;
    void redo() override;

private:
    Object* object;
    QImage newImg;
    int pos;
};

class RemoveImageCommand : public QUndoCommand
{
public:
    RemoveImageCommand(QImage, int, Object*, QUndoCommand* = nullptr);
    void undo() override;
    void redo() override;

private:
    Object* object;
    QImage oldImg;
    int pos;
};

class ChangeFrameCommand : public QUndoCommand
{
public:
    ChangeFrameCommand(int, int, Timeline*, QUndoCommand* = nullptr);
    void undo() override;
    void redo() override;

private:
    Timeline *timeline;
    int oldPos;
    int newPos;
};

#endif
