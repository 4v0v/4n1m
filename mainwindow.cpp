#include <QtWidgets>

#include "mainwindow.h"
#include "editor.h"
#include "timeline.h"

MainWindow::MainWindow()
{
    editor = new Editor;
    timeline = new Timeline;

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(editor, 0, 0, 6, 1);
    layout->addWidget(timeline, 7, 0, 1, 1);

    QWidget *window = new QWidget();
    window->setLayout(layout);
    setCentralWidget(window);

    setWindowTitle(tr("Scribble"));
    createActions();
    createMenus();
}

void MainWindow::createActions()
{
    penColorAct = new QAction(tr("&Color..."), this);
    penWidthAct = new QAction(tr("Pen &Width..."), this);
    toolAct = new QAction(tr("Tool..."), this);
    fillStyleAct = new QAction(tr("Fill style..."), this);
    clearScreenAct = new QAction(tr("&Clear Screen"), this);

    clearScreenAct->setShortcut(tr("W"));

    connect(penColorAct, SIGNAL(triggered()), this, SLOT(penColor()));
    connect(penWidthAct, SIGNAL(triggered()), this, SLOT(penWidth()));
    connect(toolAct, SIGNAL(triggered()), this, SLOT(selectTool()));
    connect(fillStyleAct, SIGNAL(triggered()), this, SLOT(selectFillStyle()));
    connect(clearScreenAct, SIGNAL(triggered()), editor, SLOT(clearImage()));
}

void MainWindow::createMenus()
{
    optionMenu = new QMenu(tr("&Options"), this);
    optionMenu->addAction(penColorAct);
    optionMenu->addAction(penWidthAct);
    optionMenu->addAction(toolAct);
    optionMenu->addAction(fillStyleAct);
    optionMenu->addSeparator();
    optionMenu->addAction(clearScreenAct);

    menuBar()->addMenu(optionMenu);
}

void MainWindow::penColor()
{
    QColor newColor = QColorDialog::getColor(editor->getPenColor());
    if (newColor.isValid()) editor->setPenColor(newColor);
}

void MainWindow::penWidth()
{
    bool ok;
    int newWidth = QInputDialog::getInt(this, tr("Scribble"), tr("Select pen width:"), editor->getPenWidth(), 1, 50, 1, &ok);
    if (ok) editor->setPenWidth(newWidth);
}

void MainWindow::selectTool()
{
    bool ok;
    int newTool = QInputDialog::getInt(this, tr("S"), tr("Select tool"), editor->getTool(), 0, 1, 1, &ok);
    if (ok) editor->setTool(static_cast<Editor::Tool>(newTool));
}

void MainWindow::selectFillStyle()
{
    bool ok;
    int newFillStyle = QInputDialog::getInt(this, tr("S"), tr("Select fill style"), editor->getFillStyle(), 1, 14, 1, &ok);
    if (ok) editor->setFillStyle(static_cast<Qt::BrushStyle>(newFillStyle));
}
