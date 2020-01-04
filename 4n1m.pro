QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainWidgets/colorwheel.cpp \
    mainWidgets/frame.cpp \
    mainWidgets/layer.cpp \
    mainWidgets/toolbar.cpp \
    mainwindow.cpp \
    animation.cpp \
    commands.cpp \
    preview.cpp \
    mainWidgets/editor.cpp \
    mainWidgets/timeline.cpp \
    mainWidgets/titlebar.cpp

HEADERS += \
    mainWidgets/colorwheel.h \
    mainWidgets/frame.h \
    mainWidgets/layer.h \
    mainWidgets/toolbar.h \
    mainwindow.h \
    animation.h \
    commands.h \
    preview.h \
    mainWidgets/editor.h \
    mainWidgets/timeline.h \
    mainWidgets/titlebar.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
