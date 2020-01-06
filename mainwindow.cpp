#include "mainwindow.h"
#include "animation.h"
#include "preview.h"
#include "commands.h"
#include "mainWidgets/editor.h"
#include "mainWidgets/timeline.h"
#include "mainWidgets/titlebar.h"
#include "mainWidgets/toolbar.h"

MainWindow::MainWindow()
{
    // Init window
    setGeometry(windowDimensions);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowTitle(tr("4n1m"));
    setFocusPolicy(Qt::StrongFocus);

    // Init Widgets
    QWidget* window = new QWidget();
    undostack = new QUndoStack(this);
    animation = new Animation(this);
    titlebar = new Titlebar(this);
    editor = new Editor(this);
    timeline = new Timeline(this);
    undostack->setUndoLimit(undostackSize);

    // Init Layout
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(titlebar);
    layout->addWidget(editor, 5);
    layout->addWidget(timeline, 1);
    window->setLayout(layout);
    setCentralWidget(window);
    toolbar = new Toolbar(this, window);
    subtoolbar = new Subtoolbar(this, window);
    toolbar->setSuboolbar(subtoolbar);
    subtoolbar->setToolbar(toolbar);
    toolbar->setCurrentTool(ToolbarTool::TOOL_pen);

    // Create Actions
    QAction* saveAnimationAct = new QAction(tr("Save animation"), this);
    QAction* changeKnockbackAct = new QAction(tr("Knockback amount..."), this);
    QAction* changeFPSAct = new QAction(tr("FPS..."), this);
    QAction* clearScreenAct = new QAction(tr("Clear Screen"), this);
    QAction* knockbackAct = new QAction(tr("Knockback"), this);
    QAction* addKeyAct = new QAction(tr("Add Key"), this);
    QAction* removeKeyAct = new QAction(tr("Remove Key"), this);
    QAction* insertFrameAct = new QAction(tr("Insert frame"), this);
    QAction* removeFrameAct = new QAction(tr("Remove frame"), this);
    QAction* openPreviewWindowAct = new QAction(tr("Preview"), this);
    QAction* changeUndoAmountAct = new QAction(tr("Undo stack size ..."), this);
    QAction* openUndoStackWindowAct = new QAction(tr("UndoStack"), this);
    QAction* copyFrameAct = new QAction(tr("Copy"), this);
    QAction* cutFrameAct = new QAction(tr("Cut"), this);
    QAction* pasteFrameAct = new QAction(tr("Paste"), this);
    QAction* undoAct = undostack->createUndoAction(this, tr("&Undo"));
    QAction* redoAct = undostack->createRedoAction(this, tr("&Redo"));

    // Shortcuts
    knockbackAct->setShortcut(tr("A"));
    clearScreenAct->setShortcut(tr("Z"));
    removeKeyAct->setShortcut(tr("W"));
    addKeyAct->setShortcut(tr("X"));
    insertFrameAct->setShortcut(tr("C"));
    removeFrameAct->setShortcut(tr("V"));
    openPreviewWindowAct->setShortcut(tr("P"));
    openUndoStackWindowAct->setShortcut(tr("O"));
    copyFrameAct->setShortcut(QKeySequence::Copy);
    cutFrameAct->setShortcut(QKeySequence::Cut);
    pasteFrameAct->setShortcut(QKeySequence::Paste);
    undoAct->setShortcuts(QKeySequence::Undo);
    redoAct->setShortcuts(QKeySequence::Redo);

    // Connect Signals & SLots
    connect(changeKnockbackAct, SIGNAL(triggered()), this, SLOT(openKnockbackAmountWindow()));
    connect(openPreviewWindowAct, SIGNAL(triggered()), this, SLOT(openPreviewWindow()));
    connect(openUndoStackWindowAct, SIGNAL(triggered()), this, SLOT(openUndoStackWindow()));
    connect(changeUndoAmountAct, SIGNAL(triggered()), this, SLOT(openUndoAmountWindow()));
    connect(changeFPSAct, SIGNAL(triggered()), this, SLOT(openChangeFPSWindow()));
    connect(saveAnimationAct, SIGNAL(triggered()), animation, SLOT(saveAnimation()));
    connect(clearScreenAct, SIGNAL(triggered()), editor, SLOT(clearImage()));
    connect(knockbackAct, SIGNAL(triggered()), editor, SLOT(knockback()));
    connect(addKeyAct, SIGNAL(triggered()), timeline, SLOT(addKey()));
    connect(removeKeyAct, SIGNAL(triggered()), timeline, SLOT(removeKey()));
    connect(insertFrameAct, SIGNAL(triggered()), timeline, SLOT(insertFrame()));
    connect(removeFrameAct, SIGNAL(triggered()), timeline, SLOT(removeFrame()));
    connect(copyFrameAct, SIGNAL(triggered()), timeline, SLOT(copyFrame()));
    connect(cutFrameAct, SIGNAL(triggered()), timeline, SLOT(cutFrame()));
    connect(pasteFrameAct, SIGNAL(triggered()), timeline, SLOT(pasteFrame()));

    // Create Menus
    titlebar->getMenubar()->getOptionsMenu()->addAction(saveAnimationAct);
    titlebar->getMenubar()->getOptionsMenu()->addSeparator();
    titlebar->getMenubar()->getOptionsMenu()->addAction(addKeyAct);
    titlebar->getMenubar()->getOptionsMenu()->addAction(removeKeyAct);
    titlebar->getMenubar()->getOptionsMenu()->addAction(insertFrameAct);
    titlebar->getMenubar()->getOptionsMenu()->addAction(removeFrameAct);
    titlebar->getMenubar()->getOptionsMenu()->addAction(clearScreenAct);
    titlebar->getMenubar()->getOptionsMenu()->addAction(knockbackAct);
    titlebar->getMenubar()->getOptionsMenu()->addSeparator();
    titlebar->getMenubar()->getOptionsMenu()->addAction(openUndoStackWindowAct);
    titlebar->getMenubar()->getOptionsMenu()->addAction(openPreviewWindowAct);
    titlebar->getMenubar()->getOptionsMenu()->addSeparator();
    titlebar->getMenubar()->getOptionsMenu()->addAction(copyFrameAct);
    titlebar->getMenubar()->getOptionsMenu()->addAction(cutFrameAct);
    titlebar->getMenubar()->getOptionsMenu()->addAction(pasteFrameAct);
    titlebar->getMenubar()->getOptionsMenu()->addAction(undoAct);
    titlebar->getMenubar()->getOptionsMenu()->addAction(redoAct);
    titlebar->getMenubar()->getOptionsMenu()->addSeparator();
    titlebar->getMenubar()->getOptionsMenu()->addAction(changeKnockbackAct);
    titlebar->getMenubar()->getOptionsMenu()->addAction(changeFPSAct);
    titlebar->getMenubar()->getOptionsMenu()->addAction(changeUndoAmountAct);
}

void MainWindow::openKnockbackAmountWindow()
{
    bool onTop = isOnTop;
    if (onTop) toggleStayOnTop();
    bool ok;
    int newK = QInputDialog::getInt(this, tr("Knockback amount"), tr("Knockback amount:"), editor->getKnockbackAmount(), 1, 255, 1, &ok);
    if (onTop) toggleStayOnTop();
    if (ok) editor->setKnockbackAmount(newK);
}

void MainWindow::openChangeFPSWindow()
{
    bool onTop = isOnTop;
    if (onTop) toggleStayOnTop();
    bool ok;
    int newFPS = QInputDialog::getInt(this, tr("Change FPS"), tr("Change FPS"), FPS, 1, 72, 1, &ok);
    if (onTop) toggleStayOnTop();
    if (ok) FPS = newFPS;
}

void MainWindow::openUndoAmountWindow()
{
    bool onTop = isOnTop;
    if (onTop) toggleStayOnTop();
    bool ok;
    int undoAmount = QInputDialog::getInt(this, tr("Undo size "), tr("Undo size (This will delete the current undo stack)"), undostackSize, 0, 100, 1, &ok);
    if (onTop) toggleStayOnTop();
    if (ok) undostackSize = undoAmount; undostack->clear(); undostack->setUndoLimit(undostackSize);
}

void MainWindow::openPreviewWindow()
{
    Preview* preview = new Preview(this);
    preview->setWindowTitle(tr("Preview"));
    preview->setAttribute(Qt::WA_QuitOnClose, false);
    preview->setGeometry(window()->x(), window()->y(), editor->width(), editor->height());
    preview->show();
}

void MainWindow::openUndoStackWindow()
{
    QUndoView* undoView = new QUndoView(undostack);
    undoView->setWindowTitle(tr("Undo Stack"));
    undoView->setAttribute(Qt::WA_QuitOnClose, false);
    undoView->show();
}

void MainWindow::toggleStayOnTop()
{
    isOnTop = !isOnTop;
    #ifdef Q_OS_WIN
        if (isOnTop) SetWindowPos(reinterpret_cast<HWND>(winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        else SetWindowPos(reinterpret_cast<HWND>(winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    #else
        if (isOnTop) setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        else setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
        show();
    #endif
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        case Qt::Key_Up: timeline->gotoPrevLayer(); break;
        case Qt::Key_Down: timeline->gotoNextLayer(); break;
        case Qt::Key_Left: timeline->gotoPrevFrame(); break;
        case Qt::Key_Right: timeline->gotoNextFrame(); break;
        case Qt::Key_1: editor->setToolAsPen();        break;
        case Qt::Key_2: editor->setToolAsLine();       break;
        case Qt::Key_3: editor->setToolAsLassoFill();  break;
        case Qt::Key_4: editor->setToolAsEraser();     break;
        case Qt::Key_5: editor->setToolAsSelect();     break;
        case Qt::Key_6: editor->setToolAsEmpty();      break;
        case Qt::Key_7: editor->toggleOnionskin();     if(!editor->isScribbling()) toolbar->setCurrentTool(ToolbarTool::TOOL_other); break;
        case Qt::Key_8: editor->toggleOnionskinloop(); if(!editor->isScribbling()) toolbar->setCurrentTool(ToolbarTool::TOOL_other); break;
        case Qt::Key_9: toggleStayOnTop();             if(!editor->isScribbling()) toolbar->setCurrentTool(ToolbarTool::TOOL_other); break;
    }
}
