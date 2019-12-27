#include "animation.h"
#include "mainWidgets/editor.h"
#include "mainWidgets/timeline.h"
#include "preview.h"
#include "commands.h"
#include "mainWidgets/titlebar.h"
#include "menubar.h"

Menubar::Menubar(QWidget* parent): QMenuBar(parent)
{
    update();
}

