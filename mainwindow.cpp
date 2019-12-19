#include <QtWidgets>
#include <QWindow>
#include "mainwindow.h"
#include "object.h"
#include "editor.h"
#include "timeline.h"
#include "preview.h"
#include "commands.h"

MainWindow::MainWindow()
{
    // Init UndoStack & Widgets
    undoStack = new QUndoStack(this);
    object = new Object(undoStack);
    editor = new Editor(object, undoStack);
    timeline = new Timeline(object, undoStack);
    editor->setTimeline(timeline);
    timeline->setEditor(editor);
    object->setEditor(editor);
    object->setTimeline(timeline);

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    // Init Layout
    QGridLayout *layout = new QGridLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(editor, 0, 0, 6, 1);
    layout->addWidget(timeline, 7, 0, 1, 1);

    // Init Window
    QWidget *window = new QWidget();
    window->setLayout(layout);
    setCentralWidget(window);
    setWindowTitle(tr("4n1m"));

    // Create Actions
    QAction *changePenColorAct = new QAction(tr("Color..."), this);
    QAction *changeBackgroundColorAct = new QAction(tr("Background Color..."), this);
    QAction *changePenWidthAct = new QAction(tr("Pen Width..."), this);
    QAction *changeLassoFillStyleAct = new QAction(tr("LassoFill Style..."), this);
    QAction *setToolAsPenAct = new QAction(tr("Pen"), this);
    QAction *setToolAsLineAct = new QAction(tr("Line"), this);
    QAction *setToolAsLassoFillAct = new QAction(tr("LassoFill"), this);
    QAction *setToolAsEraserAct = new QAction(tr("Eraser"), this);
    QAction *clearScreenAct = new QAction(tr("Clear Screen"), this);
    QAction *gotoNextFrameAct = new QAction(tr("Next frame"), this);
    QAction *gotoPrevFrameAct = new QAction(tr("Prev frame"), this);
    QAction *gotoNextLayerAct = new QAction(tr("Next layer"), this);
    QAction *gotoPrevLayerAct = new QAction(tr("Prev layer"), this);
    QAction *addKeyframeAct = new QAction(tr("Add Key"), this);
    QAction *removeKeyframeAct = new QAction(tr("Remove Key"), this);
    QAction *insertFrameAct = new QAction(tr("Insert frame"), this);
    QAction *removeFrameAct = new QAction(tr("Remove frame"), this);
    QAction *openPreviewWindowAct = new QAction(tr("Preview"), this);
    QAction *openUndoStackWindowAct = new QAction(tr("Open Undo Stack"), this);
    QAction *copyFrameAct = new QAction(tr("Copy frame"), this);
    QAction *cutFrameAct = new QAction(tr("Cut frame"), this);
    QAction *pasteFrameAct = new QAction(tr("Paste frame"), this);
    QAction *undoAct = undoStack->createUndoAction(this, tr("&Undo"));
    QAction *redoAct = undoStack->createRedoAction(this, tr("&Redo"));
    toggleOnionskinAct = new QAction(tr("Toggle onionskin"), this);
    toggleOnionskinAct->setCheckable(true);
    toggleOnionskinAct->setChecked(true);
    toggleLayerTransparencyAct = new QAction(tr("Toggle layer transparency"), this);
    toggleLayerTransparencyAct->setCheckable(true);
    toggleLayerTransparencyAct->setChecked(true);
    toggleStayOnTopAct = new QAction("Stay on top", this);
    toggleStayOnTopAct->setCheckable(true);

    // Shortcuts
    setToolAsPenAct->setShortcut(Qt::Key_1);
    setToolAsLassoFillAct->setShortcut(Qt::Key_2);
    setToolAsEraserAct->setShortcut(Qt::Key_3);
    setToolAsLineAct->setShortcut(Qt::Key_4);
    changePenColorAct->setShortcut(Qt::Key_5);
    changePenWidthAct->setShortcut(Qt::Key_6);
    changeLassoFillStyleAct->setShortcut(Qt::Key_7);
    changeBackgroundColorAct->setShortcut(Qt::Key_8);
    gotoNextFrameAct->setShortcut(Qt::Key_Right);
    gotoPrevFrameAct->setShortcut(Qt::Key_Left);
    gotoNextLayerAct->setShortcut(Qt::Key_Down);
    gotoPrevLayerAct->setShortcut(Qt::Key_Up);
    clearScreenAct->setShortcut(tr("Z"));
    removeKeyframeAct->setShortcut(tr("W"));
    addKeyframeAct->setShortcut(tr("X"));
    insertFrameAct->setShortcut(tr("C"));
    removeFrameAct->setShortcut(tr("V"));
    openPreviewWindowAct->setShortcut(tr("P"));
    openUndoStackWindowAct->setShortcut(tr("O"));
    toggleOnionskinAct->setShortcut(tr("Y"));
    toggleLayerTransparencyAct->setShortcut(tr("T"));
    toggleStayOnTopAct->setShortcut(tr("U"));
    copyFrameAct->setShortcut(QKeySequence::Copy);
    cutFrameAct->setShortcut(QKeySequence::Cut);
    pasteFrameAct->setShortcut(QKeySequence::Paste);
    undoAct->setShortcuts(QKeySequence::Undo);
    redoAct->setShortcuts(QKeySequence::Redo);

    // Connect Signals & SLots
    connect(changePenColorAct, SIGNAL(triggered()), this, SLOT(openPenColorWindow()));
    connect(changePenWidthAct, SIGNAL(triggered()), this, SLOT(openPenWidthWindow()));
    connect(changeBackgroundColorAct, SIGNAL(triggered()), this, SLOT(openBackgroundColorWindow()));
    connect(changeLassoFillStyleAct, SIGNAL(triggered()), this, SLOT(openFillStyleWindow()));
    connect(openPreviewWindowAct, SIGNAL(triggered()), this, SLOT(openPreviewWindow()));
    connect(openUndoStackWindowAct, SIGNAL(triggered()), this, SLOT(openUndoStackWindow()));
    connect(toggleStayOnTopAct, SIGNAL(triggered()), this, SLOT(toggleStayOnTop()));
    connect(setToolAsPenAct, SIGNAL(triggered()), editor, SLOT(setToolAsPen()));
    connect(setToolAsLineAct, SIGNAL(triggered()), editor, SLOT(setToolAsLine()));
    connect(setToolAsLassoFillAct, SIGNAL(triggered()), editor, SLOT(setToolAsLassoFill()));
    connect(setToolAsEraserAct, SIGNAL(triggered()), editor, SLOT(setToolAsEraser()));
    connect(clearScreenAct, SIGNAL(triggered()), editor, SLOT(clearImage()));
    connect(toggleOnionskinAct, SIGNAL(triggered()), editor, SLOT(toggleOnionskin()));
    connect(toggleLayerTransparencyAct, SIGNAL(triggered()), editor, SLOT(toggleLayerTransparency()));
    connect(gotoNextFrameAct, SIGNAL(triggered()), timeline, SLOT(gotoNextFrame()));
    connect(gotoPrevFrameAct, SIGNAL(triggered()), timeline, SLOT(gotoPrevFrame()));
    connect(gotoNextLayerAct, SIGNAL(triggered()), timeline, SLOT(gotoNextLayer()));
    connect(gotoPrevLayerAct, SIGNAL(triggered()), timeline, SLOT(gotoPrevLayer()));
    connect(addKeyframeAct, SIGNAL(triggered()), timeline, SLOT(addKeyframe()));
    connect(removeKeyframeAct, SIGNAL(triggered()), timeline, SLOT(removeKeyframe()));
    connect(insertFrameAct, SIGNAL(triggered()), timeline, SLOT(insertFrame()));
    connect(removeFrameAct, SIGNAL(triggered()), timeline, SLOT(removeFrame()));
    connect(copyFrameAct, SIGNAL(triggered()), timeline, SLOT(copyFrame()));
    connect(cutFrameAct, SIGNAL(triggered()), timeline, SLOT(cutFrame()));
    connect(pasteFrameAct, SIGNAL(triggered()), timeline, SLOT(pasteFrame()));

    // Create Menus
    QMenu *optionMenu = new QMenu(tr("Menu"), this);
    optionMenu->addAction(gotoNextFrameAct);
    optionMenu->addAction(gotoPrevFrameAct);
    optionMenu->addAction(gotoNextLayerAct);
    optionMenu->addAction(gotoPrevLayerAct);
    optionMenu->addAction(addKeyframeAct);
    optionMenu->addAction(removeKeyframeAct);
    optionMenu->addAction(insertFrameAct);
    optionMenu->addAction(removeFrameAct);
    optionMenu->addAction(clearScreenAct);
    optionMenu->addSeparator();
    optionMenu->addAction(copyFrameAct);
    optionMenu->addAction(cutFrameAct);
    optionMenu->addAction(pasteFrameAct);
    optionMenu->addAction(undoAct);
    optionMenu->addAction(redoAct);
    optionMenu->addSeparator();
    menuBar()->addMenu(optionMenu);

    QMenu *toolsMenu = new QMenu(tr("Tools"), this);
    toolsMenu->addAction(changePenColorAct);
    toolsMenu->addAction(changePenWidthAct);
    toolsMenu->addAction(changeLassoFillStyleAct);
    toolsMenu->addAction(changeBackgroundColorAct);
    toolsMenu->addSeparator();
    toolsMenu->addAction(openPreviewWindowAct);
    toolsMenu->addAction(openUndoStackWindowAct);
    toolsMenu->addSeparator();
    toolsMenu->addAction(setToolAsPenAct);
    toolsMenu->addAction(setToolAsLineAct);
    toolsMenu->addAction(setToolAsLassoFillAct);
    toolsMenu->addAction(setToolAsEraserAct);
    toolsMenu->addSeparator();
    toolsMenu->addAction(toggleOnionskinAct);
    toolsMenu->addAction(toggleLayerTransparencyAct);
    toolsMenu->addAction(toggleStayOnTopAct);
    menuBar()->addMenu(toolsMenu);
}

void MainWindow::openBackgroundColorWindow()
{
    QColor newColor = QColorDialog::getColor(editor->getBackgroundColor(), nullptr, QString("Color"), QColorDialog::ShowAlphaChannel);
    if (newColor.isValid()) editor->setBackgroundColor(newColor);
    editor->update();
}

void MainWindow::openPenColorWindow()
{
    QColor newColor = QColorDialog::getColor(editor->getLinePen()->color(), nullptr, QString("Color"), QColorDialog::ShowAlphaChannel);
    if (!newColor.isValid()) return;
    editor->getLinePen()->setColor(newColor);
    editor->getLassoFillBrush()->setColor(newColor);
}

void MainWindow::openPenWidthWindow()
{
    bool ok;
    int newWidth = QInputDialog::getInt(this, tr("Scribble"), tr("Select pen width:"), editor->getLinePen()->width(), 1, 50, 1, &ok);
    if (ok) editor->getLinePen()->setWidth(newWidth);
}

void MainWindow::openFillStyleWindow()
{
    bool ok;
    int newFillStyle = QInputDialog::getInt(this, tr("Fill Style"), tr("Select fill style"), editor->getLassoFillBrush()->style(), 1, 14, 1, &ok);
    if (ok) editor->getLassoFillBrush()->setStyle(static_cast<Qt::BrushStyle>(newFillStyle));
}

void MainWindow::openPreviewWindow()
{
    if (preview) preview->close();
    preview = new Preview(object);
    preview->setWindowTitle(tr("Preview"));
    preview->setAttribute(Qt::WA_QuitOnClose, false);
    preview->show();
}

void MainWindow::openUndoStackWindow()
{
    undoView = new QUndoView(undoStack);
    undoView->setWindowTitle(tr("Undo Stack"));
    undoView->setAttribute(Qt::WA_QuitOnClose, false);
    undoView->show();
}

void MainWindow::toggleStayOnTop()
{
    if (toggleStayOnTopAct->isChecked()) setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    else setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
    show();
}
