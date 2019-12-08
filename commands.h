#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>

class ChangeNbrCommand : public QUndoCommand
{
public:
    enum { Id = 1234 };

    ChangeNbrCommand(int, int*, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;
    int id() const override { return Id; }

private:
    int *myNbr;
    int oldNbr;
    int newNbr;
};

#endif
