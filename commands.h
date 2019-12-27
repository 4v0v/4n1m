#ifndef COMMANDS_H
#define COMMANDS_H

#include "mainwindow.h"

class Animation;
class Timeline;
class Editor;
class ModifyImageCommand : public QUndoCommand
{
public:
    ModifyImageCommand(QImage, QImage, int, int, Animation*, QUndoCommand* = nullptr);
    void undo() override;
    void redo() override;

private:
    Animation *animation;
    QImage oldImg;
    QImage newImg;
    int layer;
    int pos;
};

class AddImageCommand : public QUndoCommand
{
public:
    AddImageCommand(QImage, int, int, Animation*, QUndoCommand* = nullptr);
    void undo() override;
    void redo() override;

private:
    Animation *animation;
    QImage newImg;
    int layer;
    int pos;
};

class RemoveImageCommand : public QUndoCommand
{
public:
    RemoveImageCommand(QImage, int, int, Animation*, QUndoCommand* = nullptr);
    void undo() override;
    void redo() override;

private:
    Animation *animation;
    QImage oldImg;
    int layer;
    int pos;
};

class InsertFrameCommand : public QUndoCommand
{
public:
    InsertFrameCommand(int, int, Animation*, QUndoCommand* = nullptr);
    void undo() override;
    void redo() override;

private:
    Animation *animation;
    int layer;
    int pos;
};

class RemoveFrameCommand : public QUndoCommand
{
public:
    RemoveFrameCommand(int, int, Animation*, QUndoCommand* = nullptr);
    void undo() override;
    void redo() override;

private:
    Animation *animation;
    int layer;
    int pos;
};

#endif
