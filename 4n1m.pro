QT += core gui
QT += widgets
QT += xml

TARGET = 4n1m
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    miniz.cpp \
    preview.cpp \
    qminiz.cpp \
    animation.cpp \
    colorwheel.cpp \
    commands.cpp \
    main.cpp \
    mainwindow.cpp \
    editor.cpp \
    timeline.cpp \
    toolbar.cpp

HEADERS += \
    miniz.h \
    preview.h \
    qminiz.h \
    animation.h \
    colorwheel.h \
    commands.h \
    mainwindow.h \
    editor.h \
    timeline.h \
    toolbar.h
