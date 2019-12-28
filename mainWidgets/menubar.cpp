#include "animation.h"
#include "mainWidgets/editor.h"
#include "mainWidgets/timeline.h"
#include "preview.h"
#include "commands.h"
#include "mainWidgets/titlebar.h"
#include "menubar.h"

Menubar::Menubar(MainWindow* mw): QMenuBar(mw)
{
    mainwindow = mw;
    setGeometry(0, 0, mainwindow->getWindowDimensions().width(), 20);
    setMaximumHeight(20);
    setMinimumHeight(20);
    update();
}

