#include "commands.h"
#include <QDebug>

ChangeNbrCommand::ChangeNbrCommand(int n, int* m, QUndoCommand *parent): QUndoCommand(parent)
{
    myNbr = m;
    oldNbr = int(*m);
    newNbr = n;
}

void ChangeNbrCommand::undo()
{
    *myNbr = oldNbr;
}

void ChangeNbrCommand::redo()
{
    *myNbr = newNbr;
    setText(QObject::tr("Nbr = %1").arg(newNbr));
}
