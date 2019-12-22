#include "object.h"
#include "editor.h"
#include "timeline.h"
#include "preview.h"
#include "commands.h"
#include "titlebar.h"
#include "menubar.h"

Menubar::Menubar(QWidget* parent): QMenuBar(parent)
{
    update();
}

