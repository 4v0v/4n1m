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
    toolbar->setCurrentTool(TOOL1);

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
    QAction* undoAct = new QAction(tr("Undo"), this);
    QAction* redoAct = new QAction(tr("Redo"), this);
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
    connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()));
    connect(redoAct, SIGNAL(triggered()), this, SLOT(redo()));
    connect(saveAnimationAct, SIGNAL(triggered()), animation, SLOT(saveAnimation()));
    connect(clearScreenAct, SIGNAL(triggered()), editor, SLOT(clearImage()));
    connect(knockbackAct, SIGNAL(triggered()), editor, SLOT(knockback()));
    connect(addKeyAct, SIGNAL(triggered()), timeline, SLOT(addKey()));
    connect(removeKeyAct, SIGNAL(triggered()), timeline, SLOT(removeKey()));
    connect(insertFrameAct, SIGNAL(triggered()), timeline, SLOT(insertFrame()));
    connect(removeFrameAct, SIGNAL(triggered()), timeline, SLOT(removeFrame()));
    connect(copyFrameAct, SIGNAL(triggered()), this, SLOT(copy()));
    connect(cutFrameAct, SIGNAL(triggered()), this, SLOT(cut()));
    connect(pasteFrameAct, SIGNAL(triggered()), this, SLOT(paste()));
    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, [this]{ this->externalCopy(); });

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

void MainWindow::externalCopy()
{
    QImage* img = new QImage(QApplication::clipboard()->image());

    if (img->width() > 0 && img->height() > 0)
    {
        clipboardState = STATE_QCLIPBOARD;
        clipboard = img->copy();
    }
}

void MainWindow::copy()
{
    if (editor->isScribbling() || !animation->isKey(timeline->getLayer(), timeline->getPos())) return;
    if (editor->selectTool->state == STATE_SELECTED)
    {
        clipboard = editor->selectTool->deltaImage.copy();
        QApplication::clipboard()->setImage(clipboard, QClipboard::Clipboard);
        clipboardState = STATE_QCLIPBOARD;
    } else {
        clipboardState = STATE_FRAME;
        clipboard = animation->copyImageAt(timeline->getLayer(), timeline->getPos());
    }
}

void MainWindow::cut()
{
    if (editor->isScribbling() || !animation->isKey(timeline->getLayer(), timeline->getPos())) return;
    copy();
    if (clipboardState == STATE_FRAME){
        timeline->removeKey();
    } else if (clipboardState == STATE_QCLIPBOARD) {

    }
}

void MainWindow::paste()
{
    if (editor->isScribbling() || (clipboard.width() <= 1 && clipboard.height() <= 1) ) return;
    if (clipboardState == STATE_FRAME){
        if (!animation->isKey(timeline->getLayer(), timeline->getPos())) {
            undostack->push(new AddImageCommand(clipboard.copy(), timeline->getLayer(), timeline->getPos(), animation));
        } else {
            QImage i = animation->copyImageAt(timeline->getLayer(), timeline->getPos());
            QImage j = i.copy();
            QPainter p(&j);
            p.drawImage(QPoint(0,0), clipboard.copy());
            undostack->push(new ModifyImageCommand(i, j, timeline->getLayer(), timeline->getPos(), animation));
        }
    } else if (clipboardState == STATE_QCLIPBOARD){
        if (!animation->isKey(timeline->getLayer(), timeline->getPos())) timeline->addKey();
        int tempX = editor->width()/2 - clipboard.width()/2;
        int tempY = editor->height()/2 - clipboard.height()/2;
        if (editor->selectTool->state == STATE_SELECTED)
        {
            QRect* drz = &editor->selectTool->deltaRectZone;
            tempX = drz->x() > drz->x() + drz->width() ? drz->x() + drz->width(): drz->x();
            tempY = drz->y() > drz->y() + drz->height() ? drz->y() + drz->height(): drz->y();
            editor->drawSelect();
        }

        editor->setToolAsSelect();
        editor->selectTool->subtool = RECTANGLE;
        editor->selectTool->state = STATE_SELECTED;
        editor->selectTool->pasted = true;
        editor->selectTool->initialImage = clipboard.copy();
        editor->selectTool->deltaImage = clipboard.copy();
        editor->selectTool->deltaRectZone = QRect(tempX, tempY, clipboard.width(), clipboard.height());
        editor->update();
    }
}

void MainWindow::undo()
{
    editor->drawSelect();
    undostack->undo();
}

void MainWindow::redo()
{
    editor->drawSelect();
    undostack->redo();
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
        case Qt::Key_Up: timeline->gotoPrevLayer();    break;
        case Qt::Key_Down: timeline->gotoNextLayer();  break;
        case Qt::Key_Left: timeline->gotoPrevFrame();  break;
        case Qt::Key_Right: timeline->gotoNextFrame(); break;
        case Qt::Key_1: editor->setToolAsPen();        break;
        case Qt::Key_2: editor->setToolAsShape();      break;
        case Qt::Key_3: editor->setToolAsLassoFill();  break;
        case Qt::Key_4: editor->setToolAsEraser();     break;
        case Qt::Key_5: editor->setToolAsSelect();     break;
        case Qt::Key_6: editor->setToolAsEmpty();      break;
        case Qt::Key_7: editor->toggleOnionskin();     if(!editor->isScribbling()) toolbar->setCurrentTool(TOOL5); break;
        case Qt::Key_8: editor->toggleOnionskinloop(); if(!editor->isScribbling()) toolbar->setCurrentTool(TOOL5); break;
        case Qt::Key_9: toggleStayOnTop();             if(!editor->isScribbling()) toolbar->setCurrentTool(TOOL5); break;
    }
}
