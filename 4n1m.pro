QT += core gui
QT += widgets
QT += xml

TARGET = 4n1m
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    mw.cpp \
    preview.cpp \
    animation.cpp \
    colorwheel.cpp \
    commands.cpp \
    main.cpp \
    editor.cpp \
    timeline.cpp \
    tool_colorpicker.cpp \
    tool_eraser.cpp \
    tool_lassofill.cpp \
    tool_pen.cpp \
    toolbar.cpp

HEADERS += \
    miniz.h \
    mw.h \
    preview.h \
    qminiz.h \
    animation.h \
    colorwheel.h \
    commands.h \
    editor.h \
    timeline.h \
    tool_colorpicker.h \
    tool_eraser.h \
    tool_lassofill.h \
    tool_move.h \
    tool_pen.h \
    toolbar.h \
    gif.h
