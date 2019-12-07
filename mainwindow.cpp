#include <QtWidgets>
#include <QWindow>
#include "mainwindow.h"
#include "object.h"
#include "editor.h"
#include "timeline.h"
#include "preview.h"

MainWindow::MainWindow()
{
    // Init Widgets
    object = new Object;
    editor = new Editor(object);
    timeline = new Timeline(object);
    editor->setTimeline(timeline);
    timeline->setEditor(editor);

    //Init Layout
    QGridLayout *layout = new QGridLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(editor, 0, 0, 6, 1);
    layout->addWidget(timeline, 7, 0, 1, 1);

    //Init Window
    QWidget *window = new QWidget();
    window->setLayout(layout);
    setCentralWidget(window);
    setWindowTitle(tr("4n1m"));

    // Create Actions
    QAction *penColorAct = new QAction(tr("Color..."), this);
    QAction *penWidthAct = new QAction(tr("Pen Width..."), this);
    QAction *fillStyleAct = new QAction(tr("LassoFill Style..."), this);
    QAction *toolAsPenAct = new QAction(tr("Pen"), this);
    QAction *toolAsLassoFillAct = new QAction(tr("LassoFill"), this);
    QAction *toolAsEraserAct = new QAction(tr("Eraser"), this);
    QAction *clearScreenAct = new QAction(tr("Clear Screen"), this);
    QAction *nextFrameAct = new QAction(tr("Next"), this);
    QAction *previousFrameAct = new QAction(tr("Prev"), this);
    QAction *addKeyframeAct = new QAction(tr("Add Key"), this);
    QAction *removeKeyframeAct = new QAction(tr("Remove Key"), this);
    QAction *insertFrameAct = new QAction(tr("Insert frame"), this);
    QAction *removeFrameAct = new QAction(tr("Remove frame"), this);
    QAction *previewAct = new QAction(tr("Preview"), this);
    QAction *copyFrameAct = new QAction(tr("Copy frame"), this);
    QAction *pasteFrameAct = new QAction(tr("Paste frame"), this);
    QAction *toggleOnionskinAct = new QAction(tr("Toggle onionskin"), this);

    // Shortcuts
    toolAsPenAct->setShortcut(Qt::Key_1);
    toolAsLassoFillAct->setShortcut(Qt::Key_2);
    toolAsEraserAct->setShortcut(Qt::Key_3);
    penColorAct->setShortcut(Qt::Key_4);
    penWidthAct->setShortcut(Qt::Key_5);
    fillStyleAct->setShortcut(Qt::Key_6);
    nextFrameAct->setShortcut(Qt::Key_Right);
    previousFrameAct->setShortcut(Qt::Key_Left);
    clearScreenAct->setShortcut(tr("Z"));
    removeKeyframeAct->setShortcut(tr("W"));
    addKeyframeAct->setShortcut(tr("X"));
    insertFrameAct->setShortcut(tr("C"));
    removeFrameAct->setShortcut(tr("V"));
    previewAct->setShortcut(tr("P"));
    copyFrameAct->setShortcut(tr("E"));
    pasteFrameAct->setShortcut(tr("R"));
    toggleOnionskinAct->setShortcut(tr("Y"));

    // Connect Signals & SLots
    connect(penColorAct, SIGNAL(triggered()), this, SLOT(openPenColorWindow()));
    connect(penWidthAct, SIGNAL(triggered()), this, SLOT(openPenWidthWindow()));
    connect(fillStyleAct, SIGNAL(triggered()), this, SLOT(openFillStyleWindow()));
    connect(previewAct, SIGNAL(triggered()), this, SLOT(openPreviewWindow()));
    connect(toolAsPenAct, SIGNAL(triggered()), editor, SLOT(setToolAsPen()));
    connect(toolAsLassoFillAct, SIGNAL(triggered()), editor, SLOT(setToolAsLassoFill()));
    connect(toolAsEraserAct, SIGNAL(triggered()), editor, SLOT(setToolAsEraser()));
    connect(clearScreenAct, SIGNAL(triggered()), editor, SLOT(clearImage()));
    connect(toggleOnionskinAct, SIGNAL(triggered()), editor, SLOT(toggleOnionskin()));
    connect(nextFrameAct, SIGNAL(triggered()), timeline, SLOT(gotoNextFrame()));
    connect(previousFrameAct, SIGNAL(triggered()), timeline, SLOT(gotoPrevFrame()));
    connect(addKeyframeAct, SIGNAL(triggered()), timeline, SLOT(addKeyframe()));
    connect(removeKeyframeAct, SIGNAL(triggered()), timeline, SLOT(removeKeyframe()));
    connect(insertFrameAct, SIGNAL(triggered()), timeline, SLOT(insertFrame()));
    connect(removeFrameAct, SIGNAL(triggered()), timeline, SLOT(removeFrame()));
    connect(copyFrameAct, SIGNAL(triggered()), timeline, SLOT(copyFrame()));
    connect(pasteFrameAct, SIGNAL(triggered()), timeline, SLOT(pasteFrame()));

    // Create Menu
    QMenu *optionMenu = new QMenu(tr("Menu"), this);
    optionMenu->addAction(penColorAct);
    optionMenu->addAction(penWidthAct);
    optionMenu->addAction(fillStyleAct);
    optionMenu->addSeparator();
    optionMenu->addAction(toolAsPenAct);
    optionMenu->addAction(toolAsLassoFillAct);
    optionMenu->addAction(toolAsEraserAct);
    optionMenu->addSeparator();
    optionMenu->addAction(clearScreenAct);
    optionMenu->addSeparator();
    optionMenu->addAction(nextFrameAct);
    optionMenu->addAction(previousFrameAct);
    optionMenu->addAction(addKeyframeAct);
    optionMenu->addAction(removeKeyframeAct);
    optionMenu->addAction(insertFrameAct);
    optionMenu->addAction(removeFrameAct);
    optionMenu->addAction(copyFrameAct);
    optionMenu->addAction(pasteFrameAct);
    optionMenu->addSeparator();
    optionMenu->addAction(previewAct);
    optionMenu->addAction(toggleOnionskinAct);
    menuBar()->addMenu(optionMenu);
}

void MainWindow::openPenColorWindow()
{
    QColor newColor = QColorDialog::getColor(editor->getPenColor());
    if (newColor.isValid()) editor->setPenColor(newColor);
}

void MainWindow::openPenWidthWindow()
{
    bool ok;
    int newWidth = QInputDialog::getInt(this, tr("Scribble"), tr("Select pen width:"), editor->getPenWidth(), 1, 50, 1, &ok);
    if (ok) editor->setPenWidth(newWidth);
}

void MainWindow::openFillStyleWindow()
{
    bool ok;
    int newFillStyle = QInputDialog::getInt(this, tr("Fill Style"), tr("Select fill style"), editor->getFillStyle(), 1, 14, 1, &ok);
    if (ok) editor->setFillStyle(static_cast<Qt::BrushStyle>(newFillStyle));
}

void MainWindow::openPreviewWindow()
{
    preview = new Preview(object);
    preview->show();
}
