#include "mainwindow.h"
#include "animation.h"
#include "preview.h"
#include "commands.h"
#include "mainWidgets/editor.h"
#include "mainWidgets/timeline.h"
#include "mainWidgets/titlebar.h"
#include "mainWidgets/toolbar.h"
#include "mainWidgets/colorwheel.h"

Toolbar::Toolbar(MainWindow* mw, QWidget* p): QWidget(p)
{
    mainwindow = mw;
    p = parent;
    setFocusPolicy(Qt::NoFocus);
    setGeometry(0, 50, 40, 280);

    tool1 = new ToolbarButton(mainwindow, this, 0,  0,  40, 40, ToolbarButtonStyle::TOOL_TEXT, "pen");
    tool2 = new ToolbarButton(mainwindow, this, 0,  40, 40, 40, ToolbarButtonStyle::TOOL_TEXT, "line");
    tool3 = new ToolbarButton(mainwindow, this, 0,  80, 40, 40, ToolbarButtonStyle::TOOL_TEXT, "lasso");
    tool4 = new ToolbarButton(mainwindow, this, 0, 120, 40, 40, ToolbarButtonStyle::TOOL_TEXT, "eraser");
    tool5 = new ToolbarButton(mainwindow, this, 0, 160, 40, 40, ToolbarButtonStyle::TOOL_TEXT, "other");
    tool6 = new ToolbarButton(mainwindow, this, 0, 200, 40, 40, ToolbarButtonStyle::TOOL_WHITE, "knockback");
    tool7 = new ToolbarButton(mainwindow, this, 0, 240, 40, 40, ToolbarButtonStyle::TOOL_RED, "clear");

    connect(tool1, &QAbstractButton::clicked, this, [this]{ editor()->setToolAsPen(); setCurrentTool(ToolbarTool::TOOL1); });
    connect(tool2, &QAbstractButton::clicked, this, [this]{ editor()->setToolAsLine(); setCurrentTool(ToolbarTool::TOOL2); });
    connect(tool3, &QAbstractButton::clicked, this, [this]{ editor()->setToolAsLassoFill(); setCurrentTool(ToolbarTool::TOOL3); });
    connect(tool4, &QAbstractButton::clicked, this, [this]{ editor()->setToolAsEraser(); setCurrentTool(ToolbarTool::TOOL4); });
    connect(tool5, &QAbstractButton::clicked, this, [this]{ editor()->setToolAsEmpty(); setCurrentTool(ToolbarTool::TOOL5); });
    connect(tool6, &QAbstractButton::clicked, this, [this]{ editor()->knockback(); });
    connect(tool7, &QAbstractButton::clicked, this, [this]{ editor()->clearImage(); });
}

void Toolbar::setCurrentTool(ToolbarTool t)
{
    if (currentTool != t)
    {
        subtoolbar->hideProperties();
        currentTool = t;
    }

    sub1()->hide();
    sub2()->hide();
    sub3()->hide();
    sub4()->hide();
    sub5()->hide();
    tool1->setIsCurrent(false);
    tool2->setIsCurrent(false);
    tool3->setIsCurrent(false);
    tool4->setIsCurrent(false);
    tool5->setIsCurrent(false);

    QImage* colorIcon = new QImage(26, 26, QImage::Format_ARGB32);
        colorIcon->fill(QColor(Qt::transparent));
        QPainter colorPainter(colorIcon);
        colorPainter.setPen(Qt::transparent);
    QImage* styleIcon = new QImage(26, 26, QImage::Format_ARGB32);
        QPainter stylePainter(styleIcon);
        styleIcon->fill(QColor(Qt::transparent));
        stylePainter.setPen(QPen(Qt::transparent, 1));
    QImage* widthIcon = new QImage(20, 20, QImage::Format_ARGB32);
        QPainter widthPainter(widthIcon);
        widthIcon->fill(QColor(Qt::transparent));
        widthPainter.setPen(QPen(QColor(50,50,50), 3));
        widthPainter.drawLine(0, 10, 20, 10);
    QImage* opacityIcon = new QImage(20, 20, QImage::Format_ARGB32);
        QPainter opacityPainter(opacityIcon);
        opacityIcon->fill(QColor(Qt::transparent));
        opacityPainter.setPen(QPen(QColor(50,50,50)));
        opacityPainter.drawEllipse(0,0, 19, 19);
        opacityPainter.setBrush(QColor(50,50,50));
        opacityPainter.drawPie(0, 0, 19, 19, 270*16, 180*16);

    switch (t) {
        case ToolbarTool::TOOL1: {
            QString penOpacity = QString::fromUtf8((std::to_string((editor()->getPenTool()->color().alpha()*100)/255) + "%").c_str());
            QString penWidth = QString::fromUtf8((std::to_string(editor()->getPenTool()->width())).c_str());
            colorPainter.setBrush(editor()->getPenTool()->color());
            colorPainter.drawEllipse(0, 0, 25, 25);
            tool1->setIsCurrent(true);
            sub1()->show(); sub1()->setImage(*colorIcon); sub1()->setStyle(SUB_ICON);
            sub2()->show(); sub2()->setText(penOpacity); sub2()->setImage(*opacityIcon); sub2()->setStyle(SUB_TEXTICON);
            sub3()->show(); sub3()->setText(penWidth); sub3()->setImage(*widthIcon); sub3()->setStyle(SUB_TEXTICON);
            subtoolbar->setGeometry(40, 70, 40, 120);
            break;
       } case ToolbarTool::TOOL2: {
            QString lineOpacity = QString::fromUtf8((std::to_string((editor()->getLineTool()->color().alpha()*100)/255) + "%").c_str());
            QString lineWidth = QString::fromUtf8((std::to_string(editor()->getLineTool()->width())).c_str());
            colorPainter.setBrush(editor()->getLineTool()->color());
            colorPainter.drawEllipse(0,0, 25, 25);
            tool2->setIsCurrent(true);
            sub1()->show(); sub1()->setImage(*colorIcon); sub1()->setStyle(SUB_ICON);
            sub2()->show(); sub2()->setText(lineOpacity); sub2()->setImage(*opacityIcon); sub2()->setStyle(SUB_TEXTICON);
            sub3()->show(); sub3()->setText(lineWidth); sub3()->setImage(*widthIcon); sub3()->setStyle(SUB_TEXTICON);
            subtoolbar->setGeometry(40, 70, 40, 120);
            break;
        } case ToolbarTool::TOOL3: {
            QString fillOpacity = QString::fromUtf8((std::to_string((editor()->getLassoFillTool()->color().alpha()*100)/255) + "%").c_str());
            colorPainter.setBrush(editor()->getLassoFillTool()->color());
            colorPainter.drawEllipse(0,0, 25, 25);
            stylePainter.setBrush(QBrush(QColor(50,50,50), editor()->getLassoFillTool()->style()));
            stylePainter.drawRect(1, 1, 24, 24);
            tool3->setIsCurrent(true);
            sub1()->show(); sub1()->setImage(*colorIcon); sub1()->setStyle(SUB_ICON);
            sub2()->show(); sub2()->setText(fillOpacity); sub2()->setImage(*opacityIcon); sub2()->setStyle(SUB_TEXTICON);
            sub3()->show(); sub3()->setImage(*styleIcon); sub3()->setStyle(SUB_ICON);
            subtoolbar->setGeometry(40, 70, 40, 120);
            break;
        } case ToolbarTool::TOOL4: {
            QString eraserWidth = QString::fromUtf8((std::to_string(editor()->getEraserTool()->width())).c_str());
            tool4->setIsCurrent(true);
            sub1()->show(); sub1()->setText(eraserWidth); sub1()->setImage(*widthIcon); sub1()->setStyle(SUB_TEXTICON);
            subtoolbar->setGeometry(40, 70, 40, 40);
            break;
        } case ToolbarTool::TOOL5: {
            QString backgroundOpacity = QString::fromUtf8((std::to_string((editor()->getBackgroundColor().alpha()*100)/255) + "%").c_str());
            colorPainter.setBrush(editor()->getBackgroundColor());
            colorPainter.drawEllipse(0,0, 25, 25);
            tool5->setIsCurrent(true);
            sub1()->show(); sub1()->setImage(*colorIcon); sub1()->setStyle(SUB_ICON);
            sub2()->show(); sub2()->setText(backgroundOpacity); sub2()->setImage(*opacityIcon); sub2()->setStyle(SUB_TEXTICON);
            sub3()->show(); sub3()->setText("onion"); sub3()->setIsCurrent(editor()->onionskinVisible); sub3()->setStyle(SUB_TOGGLE);
            sub4()->show(); sub4()->setText("oloop"); sub4()->setIsCurrent(editor()->onionskinloopVisible); sub4()->setStyle(SUB_TOGGLE);
            sub5()->show(); sub5()->setText("ontop"); sub5()->setIsCurrent(mainwindow->isOnTop); sub5()->setStyle(SUB_TOGGLE);
            subtoolbar->setGeometry(40, 70, 40, 200);
            break;
        } default: break;
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

Subtoolbar::Subtoolbar(MainWindow* mw, QWidget* p): QWidget(p)
{
    mainwindow = mw;
    p = parent;
    setFocusPolicy(Qt::NoFocus);
    setGeometry(40, 80, 40, 200);

    subtool1 = new ToolbarButton(mainwindow, this, 0, 0, 40, 40, ToolbarButtonStyle::SUB_ICON);
    subtool2 = new ToolbarButton(mainwindow, this, 0, 40, 40, 40, ToolbarButtonStyle::SUB_ICON);
    subtool3 = new ToolbarButton(mainwindow, this, 0, 80, 40, 40, ToolbarButtonStyle::SUB_ICON);
    subtool4 = new ToolbarButton(mainwindow, this, 0, 120, 40, 40, ToolbarButtonStyle::SUB_ICON);
    subtool5 = new ToolbarButton(mainwindow, this, 0, 160, 40, 40, ToolbarButtonStyle::SUB_ICON);

    connect(subtool1, &QAbstractButton::clicked, this, [this]{ clickSubtool(ToolbarTool::SUB1); });
    connect(subtool2, &QAbstractButton::clicked, this, [this]{ clickSubtool(ToolbarTool::SUB2); });
    connect(subtool3, &QAbstractButton::clicked, this, [this]{ clickSubtool(ToolbarTool::SUB3); });
    connect(subtool4, &QAbstractButton::clicked, this, [this]{ clickSubtool(ToolbarTool::SUB4); });
    connect(subtool5, &QAbstractButton::clicked, this, [this]{ clickSubtool(ToolbarTool::SUB5); });

    initProperties();
    hideProperties();
}

void Subtoolbar::initProperties()
{
    QString sliderStylesheet = "QSlider::groove:horizontal{border:0px;height:4px;background:white;margin:2px 0;}\
                                QSlider::handle:horizontal{background:rgb(50,50,50);border:1px solid #5c5c5c;width:9px;margin:-20px 0;}";

    penColorProperty = new ToolbarButton(mainwindow, mainwindow, 80, 70, 120, 120, SUB_EMPTY);
    ColorWheel* penColorwheel = new ColorWheel(penColorProperty);
    penColorwheel->resize(120, 120);
    connect(penColorwheel, &ColorWheel::colorChanged, this, [penColorwheel, this]{
        QPen* p = editor()->getPenTool();
        p->setColor(QColor(penColorwheel->color().red(), penColorwheel->color().green(), penColorwheel->color().blue(), p->color().alpha()));
        toolbar->setCurrentTool(ToolbarTool::TOOL1);
    });
    connect(penColorwheel, &ColorWheel::colorSelected, this, [penColorwheel, this]{
        QPen* p = editor()->getPenTool();
        p->setColor(QColor(penColorwheel->color().red(), penColorwheel->color().green(), penColorwheel->color().blue(), p->color().alpha()));
        toolbar->setCurrentTool(ToolbarTool::TOOL1);
    });

    lineColorProperty = new ToolbarButton(mainwindow, mainwindow, 80, 70, 120, 120, SUB_EMPTY);
    ColorWheel* lineColorwheel = new ColorWheel(lineColorProperty);
    lineColorwheel->resize(120, 120);
    connect(lineColorwheel, &ColorWheel::colorChanged, this, [lineColorwheel, this]{
        QPen* p = editor()->getLineTool();
        p->setColor(QColor(lineColorwheel->color().red(), lineColorwheel->color().green(), lineColorwheel->color().blue(), p->color().alpha()));
        toolbar->setCurrentTool(ToolbarTool::TOOL2);
    });
    connect(lineColorwheel, &ColorWheel::colorSelected, this, [lineColorwheel, this]{
        QPen* p = editor()->getLineTool();
        p->setColor(QColor(lineColorwheel->color().red(), lineColorwheel->color().green(), lineColorwheel->color().blue(), p->color().alpha()));
        toolbar->setCurrentTool(ToolbarTool::TOOL2);
    });

    lassoColorProperty = new ToolbarButton(mainwindow, mainwindow, 80, 70, 120, 120, SUB_EMPTY);
    ColorWheel* lassoColorwheel = new ColorWheel(lassoColorProperty);
    lassoColorwheel->resize(120, 120);
    connect(lassoColorwheel, &ColorWheel::colorChanged, this, [lassoColorwheel, this]{
        QBrush* p = editor()->getLassoFillTool();
        p->setColor(QColor(lassoColorwheel->color().red(), lassoColorwheel->color().green(), lassoColorwheel->color().blue(), p->color().alpha()));
        toolbar->setCurrentTool(ToolbarTool::TOOL3);
    });
    connect(lassoColorwheel, &ColorWheel::colorSelected, this, [lassoColorwheel, this]{
        QBrush* p = editor()->getLassoFillTool();
        p->setColor(QColor(lassoColorwheel->color().red(), lassoColorwheel->color().green(), lassoColorwheel->color().blue(), p->color().alpha()));
        toolbar->setCurrentTool(ToolbarTool::TOOL3);
    });

    bgColorProperty = new ToolbarButton(mainwindow, mainwindow, 80, 70, 120, 120, SUB_EMPTY);
    ColorWheel* bgColorwheel = new ColorWheel(bgColorProperty);
    bgColorwheel->resize(120, 120);
    connect(bgColorwheel, &ColorWheel::colorChanged, this, [bgColorwheel, this]{
        QColor p = editor()->getBackgroundColor();
        QColor q = QColor(bgColorwheel->color().red(), bgColorwheel->color().green(), bgColorwheel->color().blue(), p.alpha());
        editor()->setBackgroundColor(q);
        editor()->update();
        toolbar->setCurrentTool(ToolbarTool::TOOL5);
    });
    connect(bgColorwheel, &ColorWheel::colorSelected, this, [bgColorwheel, this]{
        QColor p = editor()->getBackgroundColor();
        QColor q = QColor(bgColorwheel->color().red(), bgColorwheel->color().green(), bgColorwheel->color().blue(), p.alpha());
        editor()->setBackgroundColor(q);
        editor()->update();
        toolbar->setCurrentTool(ToolbarTool::TOOL5);
    });

    penOpacityProperty = new ToolbarButton(mainwindow, mainwindow, 80, 110, 120, 40, SUB_EMPTY);
    QSlider* penOpacitySlider = new QSlider(Qt::Horizontal, penOpacityProperty);
    penOpacitySlider->setGeometry(5, 0, 110, 40);
    penOpacitySlider->setRange(0, 255);
    penOpacitySlider->setValue(editor()->getPenTool()->color().alpha());
    penOpacitySlider->setStyleSheet(sliderStylesheet);
    connect(penOpacitySlider, &QAbstractSlider::valueChanged, this, [penOpacitySlider, this]{
        QPen* p = editor()->getPenTool();
        p->setColor(QColor(p->color().red(), p->color().green(), p->color().blue(), penOpacitySlider->value()));
        toolbar->setCurrentTool(ToolbarTool::TOOL1);
    });

    penWidthProperty = new ToolbarButton(mainwindow, mainwindow, 80, 150, 120, 40, SUB_EMPTY);
    QSlider* penWidthSlider = new QSlider(Qt::Horizontal, penWidthProperty);
    penWidthSlider->setGeometry(5, 0, 110, 40);
    penWidthSlider->setRange(1, 30);
    penWidthSlider->setValue(editor()->getPenTool()->width());
    penWidthSlider->setStyleSheet(sliderStylesheet);
    connect(penWidthSlider, &QAbstractSlider::valueChanged, this, [penWidthSlider, this]{
        editor()->getPenTool()->setWidth(penWidthSlider->value());
        toolbar->setCurrentTool(ToolbarTool::TOOL1);
    });

    lineOpacityProperty = new ToolbarButton(mainwindow, mainwindow, 80, 110, 120, 40, SUB_EMPTY);
    QSlider* lineOpacitySlider = new QSlider(Qt::Horizontal, lineOpacityProperty);
    lineOpacitySlider->setGeometry(5, 0, 110, 40);
    lineOpacitySlider->setRange(0, 255);
    lineOpacitySlider->setValue(editor()->getLineTool()->color().alpha());
    lineOpacitySlider->setStyleSheet(sliderStylesheet);
    connect(lineOpacitySlider, &QAbstractSlider::valueChanged, this, [lineOpacitySlider, this]{
        QPen* p = editor()->getLineTool();
        p->setColor(QColor(p->color().red(), p->color().green(), p->color().blue(), lineOpacitySlider->value()));
        toolbar->setCurrentTool(ToolbarTool::TOOL2);
    });

    lineWidthProperty = new ToolbarButton(mainwindow, mainwindow, 80, 150, 120, 40, SUB_EMPTY);
    QSlider* lineWidthSlider = new QSlider(Qt::Horizontal, lineWidthProperty);
    lineWidthSlider->setGeometry(5, 0, 110, 40);
    lineWidthSlider->setRange(1, 30);
    lineWidthSlider->setValue(editor()->getLineTool()->width());
    lineWidthSlider->setStyleSheet(sliderStylesheet);
    connect(lineWidthSlider, &QAbstractSlider::valueChanged, this, [lineWidthSlider, this]{
        editor()->getLineTool()->setWidth(lineWidthSlider->value());
        toolbar->setCurrentTool(ToolbarTool::TOOL2);
    });

    lassoOpacityProperty = new ToolbarButton(mainwindow, mainwindow, 80, 110, 120, 40, SUB_EMPTY);
    QSlider* lassoOpacitySlider = new QSlider(Qt::Horizontal, lassoOpacityProperty);
    lassoOpacitySlider->setGeometry(5, 0, 110, 40);
    lassoOpacitySlider->setRange(0, 255);
    lassoOpacitySlider->setValue(editor()->getLassoFillTool()->color().alpha());
    lassoOpacitySlider->setStyleSheet(sliderStylesheet);
    connect(lassoOpacitySlider, &QAbstractSlider::valueChanged, this, [lassoOpacitySlider, this]{
        QBrush* p = editor()->getLassoFillTool();
        p->setColor(QColor(p->color().red(), p->color().green(), p->color().blue(), lassoOpacitySlider->value()));
        toolbar->setCurrentTool(ToolbarTool::TOOL3);
    });

    lassoStyleProperty = new ToolbarButton(mainwindow, mainwindow, 80, 150, 98, 158, SUB_EMPTY);
    int currentStyle =  1;
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (currentStyle > 14 ) break;
            QImage* styleIcon = new QImage(26, 26, QImage::Format_ARGB32);
                QPainter stylePainter(styleIcon);
                styleIcon->fill(QColor(Qt::transparent));
                stylePainter.setPen(QPen(Qt::transparent, 1));
                stylePainter.setBrush(QBrush(QColor(50,50,50), static_cast<Qt::BrushStyle>(currentStyle)));
                stylePainter.drawRect(1, 1, 24, 24);
            ToolbarButton* t = new ToolbarButton(mainwindow, lassoStyleProperty, 30*j,  30*i,  30, 30, ToolbarButtonStyle::SUB_ICON, "", false, *styleIcon);
            connect(t, &QAbstractButton::clicked, this, [this, currentStyle]{ editor()->getLassoFillTool()->setStyle(static_cast<Qt::BrushStyle>(currentStyle)); toolbar->setCurrentTool(ToolbarTool::TOOL3); });
            currentStyle += 1;
        }
    }

    eraserWidthProperty = new ToolbarButton(mainwindow, mainwindow, 80, 70, 120, 40, SUB_EMPTY);
    QSlider* eraserWidthSlider = new QSlider(Qt::Horizontal, eraserWidthProperty);
    eraserWidthSlider->setGeometry(5, 0, 110, 40);
    eraserWidthSlider->setRange(1, 50);
    eraserWidthSlider->setValue(editor()->getEraserTool()->width());
    eraserWidthSlider->setStyleSheet(sliderStylesheet);
    connect(eraserWidthSlider, &QAbstractSlider::valueChanged, this, [eraserWidthSlider, this]{
        editor()->getEraserTool()->setWidth(eraserWidthSlider->value());
        toolbar->setCurrentTool(ToolbarTool::TOOL4);
    });

    bgOpacityProperty = new ToolbarButton(mainwindow, mainwindow, 80, 110, 120, 40, SUB_EMPTY);
    QSlider* bgOpacitySlider = new QSlider(Qt::Horizontal, bgOpacityProperty);
    bgOpacitySlider->setGeometry(5, 0, 110, 40);
    bgOpacitySlider->setRange(0, 255);
    bgOpacitySlider->setValue(editor()->getBackgroundColor().alpha());
    bgOpacitySlider->setStyleSheet(sliderStylesheet);
    connect(bgOpacitySlider, &QAbstractSlider::valueChanged, this, [bgOpacitySlider, this]{
        QColor p = editor()->getBackgroundColor();
        p.setAlpha(bgOpacitySlider->value());
        editor()->setBackgroundColor(p);
        editor()->update();
        toolbar->setCurrentTool(ToolbarTool::TOOL5);
    });
}

void Subtoolbar::hideProperties()
{
    penColorProperty->hide();
    penOpacityProperty->hide();
    penWidthProperty->hide();
    lineColorProperty->hide();
    lineOpacityProperty->hide();
    lineWidthProperty->hide();
    lassoColorProperty->hide();
    lassoOpacityProperty->hide();
    eraserWidthProperty->hide();
    bgColorProperty->hide();
    bgOpacityProperty->hide();
    lassoStyleProperty->hide();
}

void Subtoolbar::clickSubtool(ToolbarTool sub)
{
    bool isPenColorVisible = penColorProperty->isVisible();
    bool isPenOpacityVisible = penOpacityProperty->isVisible();
    bool isPenWidthVisible = penWidthProperty->isVisible();
    bool isLineColorVisible = lineColorProperty->isVisible();
    bool isLineOpacityVisible = lineOpacityProperty->isVisible();
    bool isLineWidthVisible = lineWidthProperty->isVisible();
    bool isLassoColorVisible = lassoColorProperty->isVisible();
    bool isLassoOpacityVisible = lassoOpacityProperty->isVisible();
    bool isLassoStyleVisible = lassoStyleProperty->isVisible();
    bool isEraserWidthVisible = eraserWidthProperty->isVisible();
    bool isBgColorVisible = bgColorProperty->isVisible();
    bool isBgOpacityVisible = bgOpacityProperty->isVisible();
    hideProperties();

    switch(toolbar->getCurrentTool())
    {
        case ToolbarTool::TOOL1:
            switch(sub){
                case ToolbarTool::SUB1: if(!isPenColorVisible) penColorProperty->show(); break;
                case ToolbarTool::SUB2: if(!isPenOpacityVisible) penOpacityProperty->show(); break;
                case ToolbarTool::SUB3: if(!isPenWidthVisible) penWidthProperty->show(); break;
                case ToolbarTool::SUB4: break;
                case ToolbarTool::SUB5: break;
                default: break;
            } break;
        case ToolbarTool::TOOL2:
            switch(sub){
                case ToolbarTool::SUB1: if(!isLineColorVisible) lineColorProperty->show(); break;
                case ToolbarTool::SUB2: if(!isLineOpacityVisible) lineOpacityProperty->show(); break;
                case ToolbarTool::SUB3: if(!isLineWidthVisible) lineWidthProperty->show(); break;
                case ToolbarTool::SUB4: break;
                case ToolbarTool::SUB5: break;
                default: break;
            } break;
        case ToolbarTool::TOOL3:
            switch(sub){
                case ToolbarTool::SUB1: if(!isLassoColorVisible) lassoColorProperty->show(); break;
                case ToolbarTool::SUB2: if(!isLassoOpacityVisible) lassoOpacityProperty->show(); break;
                case ToolbarTool::SUB3: if(!isLassoStyleVisible) lassoStyleProperty->show(); break;
                case ToolbarTool::SUB4: break;
                case ToolbarTool::SUB5: break;
                default: break;
            } break;
        case ToolbarTool::TOOL4:
            switch(sub){
                case ToolbarTool::SUB1: if(!isEraserWidthVisible) eraserWidthProperty->show(); break;
                case ToolbarTool::SUB2: break;
                case ToolbarTool::SUB3: break;
                case ToolbarTool::SUB4: break;
                case ToolbarTool::SUB5: break;
                default: break;
            } break;
        case ToolbarTool::TOOL5:
            switch(sub){
                case ToolbarTool::SUB1: if(!isBgColorVisible) bgColorProperty->show(); break;
                case ToolbarTool::SUB2: if(!isBgOpacityVisible) bgOpacityProperty->show(); break;
                case ToolbarTool::SUB3: editor()->toggleOnionskin(); toolbar->setCurrentTool(ToolbarTool::TOOL5);  break;
                case ToolbarTool::SUB4: editor()->toggleOnionskinloop(); toolbar->setCurrentTool(ToolbarTool::TOOL5); break;
                case ToolbarTool::SUB5: mainwindow->toggleStayOnTop(); toolbar->setCurrentTool(ToolbarTool::TOOL5); break;
                default: break;
            } break;
        default:
            break;
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

ToolbarButton::ToolbarButton(MainWindow* mw, QWidget* p, int x, int y, int w, int h, ToolbarButtonStyle style, QString t, bool toggled, QImage img): QPushButton(p)
{
    mainwindow = mw;
    setFocusPolicy(Qt::NoFocus);
    setGeometry(x, y, w, h);
    isCurrent = toggled;
    if ((style == TOOL_TEXT || SUB_TOGGLE ) && toggled ) { bgColor = QColor(175, 175, 175); letterColor = QColor(50, 50, 50); };
    if ((style == TOOL_TEXT || SUB_TOGGLE ) && !toggled) { bgColor = QColor(50, 50, 50); letterColor = QColor(175, 175, 175); };
    setText(t);
    setImage(img);
    setStyle(style);
}

void ToolbarButton::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QPainterPath path;
    QFont font = painter.font();
    font.setKerning(true);

    switch(style)
    {
        case TOOL_TEXT:
            path.addRect(0, 0, width(), height());
            painter.fillPath(path, bgColor);
            font.setPixelSize(12);
            painter.setFont(font);
            painter.setPen(letterColor);
            painter.drawText(QRect(0, 0, width(), height()), Qt::AlignCenter, text);
            break;
        case TOOL_RED:
            setStyleSheet(
                "QPushButton{background-color:rgb(255,0,0);border:0px;color:white;font-size:12px;}\
                QPushButton:pressed{background-color:white;border:0px;color:rgb(50,50,50);}"
            );
            QPushButton::paintEvent(event);
            break;
        case TOOL_WHITE:
            setStyleSheet(
                "QPushButton{background-color:white;border:0px;color:black;font-size:12px;}\
                QPushButton:pressed{background-color:rgb(50,50,50);border:0px;color:white;}"
            );
            QPushButton::paintEvent(event);
            break;
        case SUB_ICON:
            path.addRect(0, 0, width(), height());
            painter.fillPath(path, QColor(175, 175, 175));
            painter.drawImage(QPoint(7,7), image);
            break;
        case SUB_TEXTICON:
            path.addRect(0, 0, width(), height());
            painter.fillPath(path, QColor(175, 175, 175));
            painter.drawImage(QPoint(10,4), image);
            font.setPixelSize(11);
            painter.setFont(font);
            painter.setPen(QColor(50, 50, 50));
            painter.drawText(QRect(0, 25, width(), 11), Qt::AlignCenter, text);
            break;
        case SUB_EMPTY:
            path.addRect(0, 0, width(), height());
            painter.fillPath(path, QColor(175, 175, 175));
            break;
        case SUB_TOGGLE:
            path.addRect(0, 0, width(), height());
            painter.fillPath(path, letterColor);
            font.setPixelSize(12);
            painter.setFont(font);
            painter.setPen(bgColor);
            painter.drawText(QRect(0, 0, width(), height()), Qt::AlignCenter, text);
            break;
        default:
            break;
    }
}

void ToolbarButton::setIsCurrent(bool b){
    isCurrent = b;

    if (isCurrent)
    {
        bgAnim->stop();
        bgAnim->setDuration(200);
        bgAnim->setKeyValueAt(0, 175);
        bgAnim->setKeyValueAt(1, 175);
        bgAnim->start();

        letterAnim->stop();
        letterAnim->setDuration(200);
        letterAnim->setKeyValueAt(0, 50);
        letterAnim->setKeyValueAt(1, 50);
        letterAnim->start();
    } else {
        bgAnim->stop();
        bgAnim->setDuration(300);
        bgAnim->setKeyValueAt(0, bgColor.red());
        bgAnim->setKeyValueAt(1, 50);
        bgAnim->start();

        letterAnim->stop();
        letterAnim->setDuration(300);
        letterAnim->setKeyValueAt(0, letterColor.red());
        letterAnim->setKeyValueAt(1, 175);
        letterAnim->start();
    }
}
