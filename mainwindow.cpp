#include "mainwindow.h"
#include "animation.h"
#include "preview.h"
#include "commands.h"
#include "mainWidgets/editor.h"
#include "mainWidgets/timeline.h"
#include "mainWidgets/titlebar.h"

MainWindow::MainWindow()
{
    // Init window
    setGeometry(windowDimensions);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowTitle(tr("4n1m"));
    setFocusPolicy(Qt::StrongFocus);

    // Init Widgets
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
    QWidget* window = new QWidget();
    window->setLayout(layout);
    setCentralWidget(window);

    QPushButton* pen = new QPushButton("P", window);
    pen->setGeometry(0, 40, 40, 40);
    pen->setFocusPolicy(Qt::NoFocus);
    connect(pen, &QAbstractButton::clicked, this, [this]{ editor->setToolAsPen(); checkTool(Tool::PEN); });

    QPushButton* line = new QPushButton("L", window);
    line->setGeometry(0, 80-4, 40, 40);
    line->setFocusPolicy(Qt::NoFocus);
    connect(line, &QAbstractButton::clicked, this, [this]{ editor->setToolAsLine(); checkTool(Tool::LINE); });

    QPushButton* lassofill = new QPushButton("F", window);
    lassofill->setGeometry(0, 120-8, 40, 40);
    lassofill->setFocusPolicy(Qt::NoFocus);
    connect(lassofill, &QAbstractButton::clicked, this, [this]{ editor->setToolAsLassoFill(); checkTool(Tool::LASSOFILL); });

    QPushButton* eraser = new QPushButton("E", window);
    eraser->setGeometry(0, 160-12, 40, 40);
    eraser->setFocusPolicy(Qt::NoFocus);
    connect(eraser, &QAbstractButton::clicked, this, [this]{ editor->setToolAsEraser(); checkTool(Tool::ERASER); });

    QPushButton* other = new QPushButton("O", window);
    other->setGeometry(0, 200-16, 40, 40);
    other->setFocusPolicy(Qt::NoFocus);
    QPushButton* undo = new QPushButton("<=", window);
    undo->setGeometry(0, 240-18, 20, 20);
    undo->setFocusPolicy(Qt::NoFocus);
    QPushButton* redo = new QPushButton("=>", window);
    redo->setGeometry(20, 240-18, 20, 20);
    redo->setFocusPolicy(Qt::NoFocus);

    QPushButton* p1 = new QPushButton("P1", window);
    p1->setGeometry(36, 60, 30, 30);
    p1->setFocusPolicy(Qt::NoFocus);
    QPushButton* p2 = new QPushButton("P2", window);
    p2->setGeometry(36, 90-4, 30, 30);
    p2->setFocusPolicy(Qt::NoFocus);
    QPushButton* p3 = new QPushButton("P3", window);
    p3->setGeometry(36, 120-8, 30, 30);
    p3->setFocusPolicy(Qt::NoFocus);
    QPushButton* p4 = new QPushButton("P4", window);
    p4->setGeometry(36, 150-12, 30, 30);
    p4->setFocusPolicy(Qt::NoFocus);

    // Create Actions
    QAction* saveAnimationAct = new QAction(tr("Save animation"), this);
    QAction* changePenColorAct = new QAction(tr("Color..."), this);
    QAction* changeBackgroundColorAct = new QAction(tr("Background Color..."), this);
    QAction* changePenWidthAct = new QAction(tr("Pen Width..."), this);
    QAction* changeEraserWidthAct = new QAction(tr("Eraser Width..."), this);
    QAction* changeLassoFillStyleAct = new QAction(tr("LassoFill Style..."), this);
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

    setToolAsPenAct = new QAction(tr("Pen"), this);
    setToolAsLineAct = new QAction(tr("Line"), this);
    setToolAsLassoFillAct = new QAction(tr("LassoFill"), this);
    setToolAsEraserAct = new QAction(tr("Eraser"), this);
    toggleOnionskinAct = new QAction(tr("Toggle onionskin"), this);
    toggleOnionskinloopAct = new QAction(tr("Toggle onionskin loop"), this);
    toggleStayOnTopAct = new QAction("Stay on top", this);

    setToolAsPenAct->setCheckable(true);
    setToolAsLineAct->setCheckable(true);
    setToolAsLassoFillAct->setCheckable(true);
    setToolAsEraserAct->setCheckable(true);
    toggleOnionskinAct->setCheckable(true);
    toggleOnionskinloopAct->setCheckable(true);
    toggleStayOnTopAct->setCheckable(true);
    setToolAsPenAct->setChecked(true);
    toggleOnionskinAct->setChecked(true);

    // Shortcuts
    setToolAsPenAct->setShortcut(Qt::Key_1);
    setToolAsLineAct->setShortcut(Qt::Key_2);
    setToolAsLassoFillAct->setShortcut(Qt::Key_3);
    setToolAsEraserAct->setShortcut(Qt::Key_4);
    changePenColorAct->setShortcut(Qt::Key_5);
    changePenWidthAct->setShortcut(Qt::Key_6);
    changeLassoFillStyleAct->setShortcut(Qt::Key_7);
    changeKnockbackAct->setShortcut(Qt::Key_0);
    changeBackgroundColorAct->setShortcut(Qt::Key_8);
    changeFPSAct->setShortcut(Qt::Key_8);
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
    connect(changePenColorAct, SIGNAL(triggered()), this, SLOT(openPenColorWindow()));
    connect(changePenWidthAct, SIGNAL(triggered()), this, SLOT(openPenWidthWindow()));
    connect(changeEraserWidthAct, SIGNAL(triggered()), this, SLOT(openEraserWidthWindow()));
    connect(changeBackgroundColorAct, SIGNAL(triggered()), this, SLOT(openBackgroundColorWindow()));
    connect(changeLassoFillStyleAct, SIGNAL(triggered()), this, SLOT(openFillStyleWindow()));
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
    connect(setToolAsPenAct, &QAction::triggered, this, [this]{ editor->setToolAsPen(); checkTool(Tool::PEN); });
    connect(setToolAsLineAct, &QAction::triggered, this, [this]{ editor->setToolAsLine(); checkTool(Tool::LINE); });
    connect(setToolAsLassoFillAct, &QAction::triggered, this, [this]{ editor->setToolAsLassoFill(); checkTool(Tool::LASSOFILL); });
    connect(setToolAsEraserAct, &QAction::triggered, this, [this]{ editor->setToolAsEraser(); checkTool(Tool::ERASER); });
    connect(addKeyAct, SIGNAL(triggered()), timeline, SLOT(addKey()));
    connect(removeKeyAct, SIGNAL(triggered()), timeline, SLOT(removeKey()));
    connect(insertFrameAct, SIGNAL(triggered()), timeline, SLOT(insertFrame()));
    connect(removeFrameAct, SIGNAL(triggered()), timeline, SLOT(removeFrame()));
    connect(copyFrameAct, SIGNAL(triggered()), timeline, SLOT(copyFrame()));
    connect(cutFrameAct, SIGNAL(triggered()), timeline, SLOT(cutFrame()));
    connect(pasteFrameAct, SIGNAL(triggered()), timeline, SLOT(pasteFrame()));

    // Create Menus
    QMenu *optionMenu = new QMenu(tr("="), this);
    optionMenu->addAction(saveAnimationAct);
    optionMenu->addSeparator();
    optionMenu->addAction(addKeyAct);
    optionMenu->addAction(removeKeyAct);
    optionMenu->addAction(insertFrameAct);
    optionMenu->addAction(removeFrameAct);
    optionMenu->addAction(clearScreenAct);
    optionMenu->addAction(knockbackAct);
    optionMenu->addSeparator();
    optionMenu->addAction(openUndoStackWindowAct);
    optionMenu->addAction(openPreviewWindowAct);
    optionMenu->addSeparator();
    optionMenu->addAction(copyFrameAct);
    optionMenu->addAction(cutFrameAct);
    optionMenu->addAction(pasteFrameAct);
    optionMenu->addAction(undoAct);
    optionMenu->addAction(redoAct);
    optionMenu->addSeparator();
    titlebar->getMenubar()->addMenu(optionMenu);

    QMenu *toolsMenu = new QMenu(tr("+"), this);
    toolsMenu->addAction(changePenColorAct);
    toolsMenu->addAction(changePenWidthAct);
    toolsMenu->addAction(changeEraserWidthAct);
    toolsMenu->addAction(changeLassoFillStyleAct);
    toolsMenu->addAction(changeKnockbackAct);
    toolsMenu->addAction(changeBackgroundColorAct);
    toolsMenu->addAction(changeFPSAct);
    toolsMenu->addAction(changeUndoAmountAct);
    toolsMenu->addSeparator();
    toolsMenu->addAction(setToolAsPenAct);
    toolsMenu->addAction(setToolAsLineAct);
    toolsMenu->addAction(setToolAsLassoFillAct);
    toolsMenu->addAction(setToolAsEraserAct);
    toolsMenu->addSeparator();
    toolsMenu->addAction(toggleOnionskinAct);
    toolsMenu->addAction(toggleOnionskinloopAct);
    toolsMenu->addAction(toggleStayOnTopAct);
    titlebar->getMenubar()->addMenu(toolsMenu);
}


void MainWindow::openPenColorWindow()
{
    bool onTop = toggleStayOnTopAct->isChecked();
    if (onTop) toggleStayOnTopAct->setChecked(false); toggleStayOnTop();
    QColor newColor = QColorDialog::getColor(editor->getLinePen()->color(), nullptr, QString("Color"), QColorDialog::ShowAlphaChannel);
    if (onTop) toggleStayOnTopAct->setChecked(true); toggleStayOnTop();
    if (!newColor.isValid()) return;
    editor->getLinePen()->setColor(newColor);
    editor->getLassoFillBrush()->setColor(newColor);
}

void MainWindow::openBackgroundColorWindow()
{
    bool onTop = toggleStayOnTopAct->isChecked();
    if (onTop) toggleStayOnTopAct->setChecked(false); toggleStayOnTop();
    QColor newColor = QColorDialog::getColor(editor->getBackgroundColor(), nullptr, QString("Color"), QColorDialog::ShowAlphaChannel);
    if (onTop) toggleStayOnTopAct->setChecked(true); toggleStayOnTop();
    if (newColor.isValid()) editor->setBackgroundColor(newColor);
    editor->update();
}

void MainWindow::openPenWidthWindow()
{
    bool onTop = toggleStayOnTopAct->isChecked();
    if (onTop) toggleStayOnTopAct->setChecked(false); toggleStayOnTop();
    bool ok;
    int newWidth = QInputDialog::getInt(this, tr("Scribble"), tr("Select pen width:"), editor->getLinePen()->width(), 1, 50, 1, &ok);
    if (onTop) toggleStayOnTopAct->setChecked(true); toggleStayOnTop();
    if (ok) editor->getLinePen()->setWidth(newWidth);
}

void MainWindow::openEraserWidthWindow()
{
    bool onTop = toggleStayOnTopAct->isChecked();
    if (onTop) toggleStayOnTopAct->setChecked(false); toggleStayOnTop();
    bool ok;
    int newWidth = QInputDialog::getInt(this, tr("Scribble"), tr("Select eraser width:"), editor->getEraserPen()->width(), 1, 300, 1, &ok);
    if (onTop) toggleStayOnTopAct->setChecked(true); toggleStayOnTop();
    if (ok) editor->getEraserPen()->setWidth(newWidth); editor->changeTool();
}

void MainWindow::openKnockbackAmountWindow()
{
    bool onTop = toggleStayOnTopAct->isChecked();
    if (onTop) toggleStayOnTopAct->setChecked(false); toggleStayOnTop();
    bool ok;
    int newK = QInputDialog::getInt(this, tr("Scribble"), tr("Select knockback amount:"), editor->getKnockbackAmount(), 1, 255, 1, &ok);
    if (onTop) toggleStayOnTopAct->setChecked(true); toggleStayOnTop();
    if (ok) editor->setKnockbackAmount(newK);
}

void MainWindow::openChangeFPSWindow()
{
    bool onTop = toggleStayOnTopAct->isChecked();
    if (onTop) toggleStayOnTopAct->setChecked(false); toggleStayOnTop();
    bool ok;
    int newFPS = QInputDialog::getInt(this, tr("Scribble"), tr("Select FPS:"), FPS, 1, 72, 1, &ok);
    if (onTop) toggleStayOnTopAct->setChecked(true); toggleStayOnTop();
    if (ok) FPS = newFPS;
}

void MainWindow::openFillStyleWindow()
{
    bool onTop = toggleStayOnTopAct->isChecked();
    if (onTop) toggleStayOnTopAct->setChecked(false); toggleStayOnTop();
    bool ok;
    int newFillStyle = QInputDialog::getInt(this, tr("Fill Style"), tr("Select fill style"), editor->getLassoFillBrush()->style(), 1, 14, 1, &ok);
    if (onTop) toggleStayOnTopAct->setChecked(true); toggleStayOnTop();
    if (ok) editor->getLassoFillBrush()->setStyle(static_cast<Qt::BrushStyle>(newFillStyle));
}

void MainWindow::openUndoAmountWindow()
{
    bool onTop = toggleStayOnTopAct->isChecked();
    if (onTop) toggleStayOnTopAct->setChecked(false); toggleStayOnTop();
    bool ok;
    int undoAmount = QInputDialog::getInt(this, tr("Fill Style"), tr("Select undo amount (This will delete the current undo stack)"), undostackAmount, 0, 100, 1, &ok);
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

void MainWindow::checkTool(Tool t)
{
    switch (t) {
        case Tool::PEN:
            setToolAsPenAct->setChecked(true);
            setToolAsLineAct->setChecked(false);
            setToolAsEraserAct->setChecked(false);
            setToolAsLassoFillAct->setChecked(false);
            break;
        case Tool::LINE:
            setToolAsPenAct->setChecked(false);
            setToolAsLineAct->setChecked(true);
            setToolAsEraserAct->setChecked(false);
            setToolAsLassoFillAct->setChecked(false);
            break;
        case Tool::LASSOFILL:
            setToolAsPenAct->setChecked(false);
            setToolAsLineAct->setChecked(false);
            setToolAsEraserAct->setChecked(false);
            setToolAsLassoFillAct->setChecked(true);
            break;
        case Tool::ERASER:
            setToolAsPenAct->setChecked(false);
            setToolAsLineAct->setChecked(false);
            setToolAsEraserAct->setChecked(true);
            setToolAsLassoFillAct->setChecked(false);
            break;
        case Tool::EMPTY:
            break;
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    switch  (event->key())
    {
        case Qt::Key_Up: timeline->gotoPrevLayer(); break;
        case Qt::Key_Down: timeline->gotoNextLayer(); break;
        case Qt::Key_Left: timeline->gotoPrevFrame(); break;
        case Qt::Key_Right: timeline->gotoNextFrame(); break;
    }
}
