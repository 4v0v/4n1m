#ifndef COMMANDS_H
#define COMMANDS_H

#include "mainwindow.h"

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

class AddImageCommand : public QUndoCommand
{
public:
    AddImageCommand(QImage, int, int, Object*, QUndoCommand* = nullptr);
    void undo() override;
    void redo() override;

private:
    Object *object;
    QImage newImg;
    int layer;
    int pos;
};

class RemoveImageCommand : public QUndoCommand
{
public:
    RemoveImageCommand(QImage, int, int, Object*, QUndoCommand* = nullptr);
    void undo() override;
    void redo() override;

private:
    Object *object;
    QImage oldImg;
    int layer;
    int pos;
};

#endif
