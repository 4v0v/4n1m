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
    undostack->setUndoLimit(undostackAmount);

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

    toggleOnionskinAct = new QAction(tr("Toggle onionskin"), this);
    toggleOnionskinloopAct = new QAction(tr("Toggle onionskin loop"), this);
    toggleStayOnTopAct = new QAction("Stay on top", this);

    toggleOnionskinAct->setCheckable(true);
    toggleOnionskinloopAct->setCheckable(true);
    toggleStayOnTopAct->setCheckable(true);
    toggleOnionskinAct->setChecked(true);

    // Shortcuts
    changeKnockbackAct->setShortcut(Qt::Key_0);
    changeFPSAct->setShortcut(Qt::Key_9);
    knockbackAct->setShortcut(tr("A"));
    clearScreenAct->setShortcut(tr("Z"));
    removeKeyAct->setShortcut(tr("W"));
    addKeyAct->setShortcut(tr("X"));
    insertFrameAct->setShortcut(tr("C"));
    removeFrameAct->setShortcut(tr("V"));
    openPreviewWindowAct->setShortcut(tr("P"));
    openUndoStackWindowAct->setShortcut(tr("O"));
    toggleOnionskinAct->setShortcut(tr("Y"));
    toggleOnionskinloopAct->setShortcut(tr("H"));
    toggleStayOnTopAct->setShortcut(tr("U"));
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
    connect(toggleStayOnTopAct, SIGNAL(triggered()), this, SLOT(toggleStayOnTop()));
    connect(saveAnimationAct, SIGNAL(triggered()), animation, SLOT(saveAnimation()));
    connect(toggleOnionskinAct, SIGNAL(triggered()), editor, SLOT(toggleOnionskin()));
    connect(toggleOnionskinloopAct, SIGNAL(triggered()), editor, SLOT(toggleOnionskinloop()));
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

    titlebar->getMenubar()->getToolsMenu()->addAction(changeKnockbackAct);
    titlebar->getMenubar()->getToolsMenu()->addAction(changeFPSAct);
    titlebar->getMenubar()->getToolsMenu()->addAction(changeUndoAmountAct);
    titlebar->getMenubar()->getToolsMenu()->addSeparator();
    titlebar->getMenubar()->getToolsMenu()->addAction(toggleOnionskinAct);
    titlebar->getMenubar()->getToolsMenu()->addAction(toggleOnionskinloopAct);
    titlebar->getMenubar()->getToolsMenu()->addAction(toggleStayOnTopAct);
}

void MainWindow::openPenColorWindow()
{
    bool onTop = toggleStayOnTopAct->isChecked();
    if (onTop) toggleStayOnTopAct->setChecked(false); toggleStayOnTop();
    QColor newColor = QColorDialog::getColor(editor->getPenTool()->color(), this, QString("Pen color"), QColorDialog::ShowAlphaChannel);
    if (onTop) toggleStayOnTopAct->setChecked(true); toggleStayOnTop();
    if (!newColor.isValid()) return;
    editor->getPenTool()->setColor(newColor);
    toolbar->switchToolbarButton(1);
}

void MainWindow::openPenOpacityWindow()
{
    bool onTop = toggleStayOnTopAct->isChecked();
    if (onTop) toggleStayOnTopAct->setChecked(false); toggleStayOnTop();
    bool ok;
    int newOpacity = QInputDialog::getInt(this, tr("Pen opacity"), tr("Pen opacity"), editor->getPenTool()->color().alpha(), 0, 255, 1, &ok);
    if (onTop) toggleStayOnTopAct->setChecked(true); toggleStayOnTop();
    QPen* p = editor->getPenTool();
    if (ok) p->setColor(QColor(p->color().red(), p->color().green(), p->color().blue(), newOpacity));
}

void MainWindow::openPenWidthWindow()
{
    bool onTop = toggleStayOnTopAct->isChecked();
    if (onTop) toggleStayOnTopAct->setChecked(false); toggleStayOnTop();
    bool ok;
    int newWidth = QInputDialog::getInt(this, tr("Pen width"), tr("Pen width"), editor->getPenTool()->width(), 1, 50, 1, &ok);
    if (onTop) toggleStayOnTopAct->setChecked(true); toggleStayOnTop();
    if (ok) editor->getPenTool()->setWidth(newWidth);
}

void MainWindow::openPenStyleWindow()
{
}

void MainWindow::openLineColorWindow()
{
    bool onTop = toggleStayOnTopAct->isChecked();
    if (onTop) toggleStayOnTopAct->setChecked(false); toggleStayOnTop();
    QColor newColor = QColorDialog::getColor(editor->getLineTool()->color(), this, QString("Line color"), QColorDialog::ShowAlphaChannel);
    if (onTop) toggleStayOnTopAct->setChecked(true); toggleStayOnTop();
    if (!newColor.isValid()) return;
    editor->getLineTool()->setColor(newColor);
    toolbar->switchToolbarButton(2);
}

void MainWindow::openLineOpacityWindow()
{
    bool onTop = toggleStayOnTopAct->isChecked();
    if (onTop) toggleStayOnTopAct->setChecked(false); toggleStayOnTop();
    bool ok;
    int newOpacity = QInputDialog::getInt(this, tr("Line opacity"), tr("Line opacity"), editor->getLineTool()->color().alpha(), 0, 255, 1, &ok);
    if (onTop) toggleStayOnTopAct->setChecked(true); toggleStayOnTop();
    QPen* p = editor->getLineTool();
    if (ok) p->setColor(QColor(p->color().red(), p->color().green(), p->color().blue(), newOpacity));
}

void MainWindow::openLineWidthWindow()
{
    bool onTop = toggleStayOnTopAct->isChecked();
    if (onTop) toggleStayOnTopAct->setChecked(false); toggleStayOnTop();
    bool ok;
    int newWidth = QInputDialog::getInt(this, tr("Line width"), tr("Line width"), editor->getLineTool()->width(), 1, 50, 1, &ok);
    if (onTop) toggleStayOnTopAct->setChecked(true); toggleStayOnTop();
    if (ok) editor->getLineTool()->setWidth(newWidth);
}

void MainWindow::openLassofillColorWindow()
{
    bool onTop = toggleStayOnTopAct->isChecked();
    if (onTop) toggleStayOnTopAct->setChecked(false); toggleStayOnTop();
    QColor newColor = QColorDialog::getColor(editor->getLassoFillTool()->color(), this, QString("Lassofill color"), QColorDialog::ShowAlphaChannel);
    if (onTop) toggleStayOnTopAct->setChecked(true); toggleStayOnTop();
    if (!newColor.isValid()) return;
    editor->getLassoFillTool()->setColor(newColor);
    toolbar->switchToolbarButton(3);
}

void MainWindow::openLassofillOpacityWindow()
{
    bool onTop = toggleStayOnTopAct->isChecked();
    if (onTop) toggleStayOnTopAct->setChecked(false); toggleStayOnTop();
    bool ok;
    int newOpacity = QInputDialog::getInt(this, tr("Lassofill opacity"), tr("Lassofill opacity"), editor->getLassoFillTool()->color().alpha(), 0, 255, 1, &ok);
    if (onTop) toggleStayOnTopAct->setChecked(true); toggleStayOnTop();
    QBrush* p = editor->getLassoFillTool();
    if (ok) p->setColor(QColor(p->color().red(), p->color().green(), p->color().blue(), newOpacity));
}

void MainWindow::openLassofillStyleWindow()
{
    bool onTop = toggleStayOnTopAct->isChecked();
    if (onTop) toggleStayOnTopAct->setChecked(false); toggleStayOnTop();
    bool ok;
    int newFillStyle = QInputDialog::getInt(this, tr("Lassofill style"), tr("Lassofill style"), editor->getLassoFillTool()->style(), 1, 14, 1, &ok);
    if (onTop) toggleStayOnTopAct->setChecked(true); toggleStayOnTop();
    if (ok) editor->getLassoFillTool()->setStyle(static_cast<Qt::BrushStyle>(newFillStyle));
}

void MainWindow::openEraserWidthWindow()
{
    bool onTop = toggleStayOnTopAct->isChecked();
    if (onTop) toggleStayOnTopAct->setChecked(false); toggleStayOnTop();
    bool ok;
    int newWidth = QInputDialog::getInt(this, tr("Eraser width"), tr("Eraser width"), editor->getEraserTool()->width(), 1, 300, 1, &ok);
    if (onTop) toggleStayOnTopAct->setChecked(true); toggleStayOnTop();
    if (ok) editor->getEraserTool()->setWidth(newWidth);
}

void MainWindow::openBackgroundColorWindow()
{
    bool onTop = toggleStayOnTopAct->isChecked();
    if (onTop) toggleStayOnTopAct->setChecked(false); toggleStayOnTop();
    QColor newColor = QColorDialog::getColor(editor->getBackgroundColor(), this, QString("Background color"), QColorDialog::ShowAlphaChannel);
    if (onTop) toggleStayOnTopAct->setChecked(true); toggleStayOnTop();
    if (newColor.isValid()) editor->setBackgroundColor(newColor);
    editor->update();
    toolbar->switchToolbarButton(5);
}

void MainWindow::openBackgroundOpacityWindow()
{
    bool onTop = toggleStayOnTopAct->isChecked();
    if (onTop) toggleStayOnTopAct->setChecked(false); toggleStayOnTop();
    bool ok;
    int newOpacity = QInputDialog::getInt(this, tr("Background opacity"), tr("Background opacity"), editor->getBackgroundColor().alpha(), 0, 255, 1, &ok);
    if (onTop) toggleStayOnTopAct->setChecked(true); toggleStayOnTop();
    QColor c = QColor(editor->getBackgroundColor().red(), editor->getBackgroundColor().green(), editor->getBackgroundColor().blue(), newOpacity == 0 ? 1 : newOpacity);
    if (ok) editor->setBackgroundColor(c);
    editor->update();
}

void MainWindow::openKnockbackAmountWindow()
{
    bool onTop = toggleStayOnTopAct->isChecked();
    if (onTop) toggleStayOnTopAct->setChecked(false); toggleStayOnTop();
    bool ok;
    int newK = QInputDialog::getInt(this, tr("Knockback amount"), tr("Knockback amount:"), editor->getKnockbackAmount(), 1, 255, 1, &ok);
    if (onTop) toggleStayOnTopAct->setChecked(true); toggleStayOnTop();
    if (ok) editor->setKnockbackAmount(newK);
}

void MainWindow::openChangeFPSWindow()
{
    bool onTop = toggleStayOnTopAct->isChecked();
    if (onTop) toggleStayOnTopAct->setChecked(false); toggleStayOnTop();
    bool ok;
    int newFPS = QInputDialog::getInt(this, tr("Change FPS"), tr("Change FPS"), FPS, 1, 72, 1, &ok);
    if (onTop) toggleStayOnTopAct->setChecked(true); toggleStayOnTop();
    if (ok) FPS = newFPS;
}

void MainWindow::openUndoAmountWindow()
{
    bool onTop = toggleStayOnTopAct->isChecked();
    if (onTop) toggleStayOnTopAct->setChecked(false); toggleStayOnTop();
    bool ok;
    int undoAmount = QInputDialog::getInt(this, tr("Undo size "), tr("Undo size (This will delete the current undo stack)"), undostackAmount, 0, 100, 1, &ok);
    if (onTop) toggleStayOnTopAct->setChecked(true); toggleStayOnTop();
    if (ok) undostackAmount = undoAmount; undostack->clear(); undostack->setUndoLimit(undostackAmount);
}

void MainWindow::openPreviewWindow()
{
    // if (preview) preview->close();
    preview = new Preview(this);
    preview->setWindowTitle(tr("Preview"));
    preview->setAttribute(Qt::WA_QuitOnClose, false);
    preview->setGeometry(window()->x(), window()->y(), editor->width(), editor->height());
    preview->show();
}

void MainWindow::openUndoStackWindow()
{
    undoView = new QUndoView(undostack);
    undoView->setWindowTitle(tr("Undo Stack"));
    undoView->setAttribute(Qt::WA_QuitOnClose, false);
    undoView->show();
}

void MainWindow::toggleStayOnTop()
{
    #ifdef Q_OS_WIN
        if (toggleStayOnTopAct->isChecked()) SetWindowPos(reinterpret_cast<HWND>(winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        else SetWindowPos(reinterpret_cast<HWND>(winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    #else
        if (toggleStayOnTopAct->isChecked()) setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
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
        case Qt::Key_1: editor->setToolAsPen(); toolbar->switchToolbarButton(1); break;
        case Qt::Key_2: editor->setToolAsLine(); toolbar->switchToolbarButton(2); break;
        case Qt::Key_3: editor->setToolAsLassoFill(); toolbar->switchToolbarButton(3); break;
        case Qt::Key_4: editor->setToolAsEraser(); toolbar->switchToolbarButton(4); break;
        case Qt::Key_5: editor->setToolAsEmpty(); toolbar->switchToolbarButton(5); break;
    }
}
