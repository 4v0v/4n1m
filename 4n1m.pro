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
    toolbar.h \
    gif.h
