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
    setGeometry(0, 50, 40, 320);

    tool1 = new ToolbarButton(mainwindow, this, 0,  0,  40, 40, TOOL_TEXT, "pen");
    tool2 = new ToolbarButton(mainwindow, this, 0,  40, 40, 40, TOOL_TEXT, "shape");
    tool3 = new ToolbarButton(mainwindow, this, 0,  80, 40, 40, TOOL_TEXT, "fill");
    tool4 = new ToolbarButton(mainwindow, this, 0, 120, 40, 40, TOOL_TEXT, "eraser");
    tool5 = new ToolbarButton(mainwindow, this, 0, 160, 40, 40, TOOL_TEXT, "select");
    tool6 = new ToolbarButton(mainwindow, this, 0, 200, 40, 40, TOOL_TEXT, "other");
    tool7 = new ToolbarButton(mainwindow, this, 0, 240, 40, 40, TOOL_WHITE, "knockback");
    tool8 = new ToolbarButton(mainwindow, this, 0, 280, 40, 40, TOOL_RED, "selection");

    connect(tool1, &QAbstractButton::pressed, this, [this]{ editor()->setToolAsPen(); });
    connect(tool2, &QAbstractButton::pressed, this, [this]{ editor()->setToolAsShape(); });
    connect(tool3, &QAbstractButton::pressed, this, [this]{ editor()->setToolAsLassoFill(); });
    connect(tool4, &QAbstractButton::pressed, this, [this]{ editor()->setToolAsEraser(); });
    connect(tool5, &QAbstractButton::pressed, this, [this]{ editor()->setToolAsSelect(); });
    connect(tool6, &QAbstractButton::pressed, this, [this]{ editor()->setToolAsEmpty(); });
    connect(tool7, &QAbstractButton::pressed, this, [this]{ editor()->knockback(); });
    connect(tool8, &QAbstractButton::pressed, this, [this]{ editor()->clearImage(); });
}

void Toolbar::setCurrentTool(ToolbarTool t)
{
    if (currentTool != t)
    {
        subtoolbar->hideProperties();
        currentTool = t;
        editor()->drawSelect();
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
    tool6->setIsCurrent(false);
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
    QImage* shapeIcon = new QImage(26, 26, QImage::Format_ARGB32);
        shapeIcon->fill(Qt::transparent);
        QPainter shapePainter(shapeIcon);
        shapePainter.setPen(QPen(QColor(50,50,50), 2));
    QImage* fillShapeIcon = new QImage(26, 26, QImage::Format_ARGB32);
        fillShapeIcon->fill(Qt::transparent);
        QPainter fillShapePainter(fillShapeIcon);
        fillShapePainter.setPen(Qt::transparent);
        fillShapePainter.setBrush(QBrush(QColor(50,50,50)));
    QImage* selectStyleIcon = new QImage(26, 26, QImage::Format_ARGB32);
        selectStyleIcon->fill(Qt::transparent);
        QPainter selectStylePainter(selectStyleIcon);
        selectStylePainter.setPen(QPen(QColor(50,50,50), 2, Qt::DashLine));

    switch (t) {
        case TOOL1: {
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
       } case TOOL2: {
            QString shapeOpacity = QString::fromUtf8((std::to_string((editor()->getShapeTool()->color().alpha()*100)/255) + "%").c_str());
            QString shapeWidth = QString::fromUtf8((std::to_string(editor()->getShapeTool()->width())).c_str());
            colorPainter.setBrush(editor()->getShapeTool()->color());
            colorPainter.drawEllipse(0,0, 25, 25);

            if (editor()->shapeSubtool == LINE){
                shapePainter.drawLine(5, 23, 21, 3);
            } else if (editor()->shapeSubtool == RECTANGLE){
                shapePainter.drawRect(2,3,22,22);
            } else if (editor()->shapeSubtool == ELLIPSE){
                shapePainter.drawEllipse(2,3,22,22);
            }

            tool2->setIsCurrent(true);
            sub1()->show(); sub1()->setImage(*shapeIcon); sub1()->setStyle(SUB_ICON);
            sub2()->show(); sub2()->setImage(*colorIcon); sub2()->setStyle(SUB_ICON);
            sub3()->show(); sub3()->setText(shapeOpacity); sub3()->setImage(*opacityIcon); sub3()->setStyle(SUB_TEXTICON);
            sub4()->show(); sub4()->setText(shapeWidth)  ; sub4()->setImage(*widthIcon); sub4()->setStyle(SUB_TEXTICON);
            subtoolbar->setGeometry(40, 70, 40, 160);
            break;
        } case TOOL3: {
            QString fillOpacity = QString::fromUtf8((std::to_string((editor()->getFillTool()->color().alpha()*100)/255) + "%").c_str());
            colorPainter.setBrush(editor()->getFillTool()->color());
            colorPainter.drawEllipse(0,0, 25, 25);
            stylePainter.setBrush(QBrush(QColor(50,50,50), editor()->getFillTool()->style()));
            stylePainter.drawRect(1, 1, 24, 24);

            if (editor()->fillSubtool == LASSO){
                fillShapePainter.drawImage(QPoint(0,0), QImage(":/icons/lasso.png"));
            } else if (editor()->fillSubtool == RECTANGLE){
                fillShapePainter.drawRect(2,3,22,22);
            } else if (editor()->fillSubtool == ELLIPSE){
                fillShapePainter.drawEllipse(2,3,22,22);
            }

            tool3->setIsCurrent(true);
            sub1()->show(); sub1()->setImage(*fillShapeIcon); sub1()->setStyle(SUB_ICON);
            sub2()->show(); sub2()->setImage(*colorIcon); sub2()->setStyle(SUB_ICON);
            sub3()->show(); sub3()->setText(fillOpacity); sub3()->setImage(*opacityIcon); sub3()->setStyle(SUB_TEXTICON);
            sub4()->show(); sub4()->setImage(*styleIcon); sub4()->setStyle(SUB_ICON);
            subtoolbar->setGeometry(40, 70, 40, 160);
            break;
        } case TOOL4: {
            QString eraserWidth = QString::fromUtf8((std::to_string(editor()->getEraserTool()->width())).c_str());
            tool4->setIsCurrent(true);
            sub1()->show(); sub1()->setText(eraserWidth); sub1()->setImage(*widthIcon); sub1()->setStyle(SUB_TEXTICON);
            subtoolbar->setGeometry(40, 70, 40, 40);
            break;
        } case TOOL6: {
            if (editor()->selectSubtool == LASSO){
                selectStylePainter.drawEllipse(2,3,22,22);
            } else if (editor()->selectSubtool == RECTANGLE){
                selectStylePainter.drawRect(2,3,22,22);
            }
            tool5->setIsCurrent(true);
            sub1()->show(); sub1()->setImage(*selectStyleIcon); sub1()->setStyle(SUB_ICON);
            subtoolbar->setGeometry(40, 70, 40, 40);
            break;
        } case TOOL5: {
            QString backgroundOpacity = QString::fromUtf8((std::to_string((editor()->getBackgroundColor().alpha()*100)/255) + "%").c_str());
            colorPainter.setBrush(editor()->getBackgroundColor());
            colorPainter.drawEllipse(0,0, 25, 25);
            tool6->setIsCurrent(true);
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

    subtool1 = new ToolbarButton(mainwindow, this, 0, 0, 40, 40, SUB_ICON);
    subtool2 = new ToolbarButton(mainwindow, this, 0, 40, 40, 40, SUB_ICON);
    subtool3 = new ToolbarButton(mainwindow, this, 0, 80, 40, 40, SUB_ICON);
    subtool4 = new ToolbarButton(mainwindow, this, 0, 120, 40, 40, SUB_ICON);
    subtool6 = new ToolbarButton(mainwindow, this, 0, 160, 40, 40, SUB_ICON);

    connect(subtool1, &QAbstractButton::pressed, this, [this]{ clickSubtool(SUB1); });
    connect(subtool2, &QAbstractButton::pressed, this, [this]{ clickSubtool(SUB2); });
    connect(subtool3, &QAbstractButton::pressed, this, [this]{ clickSubtool(SUB3); });
    connect(subtool4, &QAbstractButton::pressed, this, [this]{ clickSubtool(SUB4); });
    connect(subtool6, &QAbstractButton::pressed, this, [this]{ clickSubtool(SUB5); });

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
            toolbar->setCurrentTool(TOOL1);
        });
        connect(penColorwheel, &ColorWheel::colorSelected, this, [penColorwheel, this]{
            QPen* p = editor()->getPenTool();
            p->setColor(QColor(penColorwheel->color().red(), penColorwheel->color().green(), penColorwheel->color().blue(), p->color().alpha()));
            toolbar->setCurrentTool(TOOL1);
        });

    bgColorProperty = new ToolbarButton(mainwindow, mainwindow, 80, 70, 120, 120, SUB_EMPTY);
        ColorWheel* bgColorwheel = new ColorWheel(bgColorProperty);
        bgColorwheel->resize(120, 120);
        connect(bgColorwheel, &ColorWheel::colorChanged, this, [bgColorwheel, this]{
            QColor p = editor()->getBackgroundColor();
            QColor q = QColor(bgColorwheel->color().red(), bgColorwheel->color().green(), bgColorwheel->color().blue(), p.alpha());
            editor()->setBackgroundColor(q);
            editor()->update();
            toolbar->setCurrentTool(TOOL5);
        });
        connect(bgColorwheel, &ColorWheel::colorSelected, this, [bgColorwheel, this]{
            QColor p = editor()->getBackgroundColor();
            QColor q = QColor(bgColorwheel->color().red(), bgColorwheel->color().green(), bgColorwheel->color().blue(), p.alpha());
            editor()->setBackgroundColor(q);
            editor()->update();
            toolbar->setCurrentTool(TOOL5);
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
            toolbar->setCurrentTool(TOOL1);
        });

    penWidthProperty = new ToolbarButton(mainwindow, mainwindow, 80, 150, 120, 40, SUB_EMPTY);
        QSlider* penWidthSlider = new QSlider(Qt::Horizontal, penWidthProperty);
        penWidthSlider->setGeometry(5, 0, 110, 40);
        penWidthSlider->setRange(1, 30);
        penWidthSlider->setValue(editor()->getPenTool()->width());
        penWidthSlider->setStyleSheet(sliderStylesheet);
        connect(penWidthSlider, &QAbstractSlider::valueChanged, this, [penWidthSlider, this]{
            editor()->getPenTool()->setWidth(penWidthSlider->value());
            toolbar->setCurrentTool(TOOL1);
        });

    shapeStyleProperty = new ToolbarButton(mainwindow, mainwindow, 80, 70, 98, 40, SUB_EMPTY);
        QImage* lineIcon = new QImage(26, 26, QImage::Format_ARGB32);
            lineIcon->fill(Qt::transparent);
            QPainter linePainter(lineIcon);
            linePainter.setPen(QPen(QColor(50,50,50), 2));
            linePainter.drawLine(2, 20, 20, 2);
        QImage* rectIcon = new QImage(26, 26, QImage::Format_ARGB32);
            rectIcon->fill(Qt::transparent);
            QPainter rectPainter(rectIcon);
            rectPainter.setPen(QPen(QColor(50,50,50), 2));
            rectPainter.drawRect(2,2,20,20);
        QImage* ellipseIcon = new QImage(26, 26, QImage::Format_ARGB32);
            ellipseIcon->fill(Qt::transparent);
            QPainter ellipsePainter(ellipseIcon);
            ellipsePainter.setPen(QPen(QColor(50,50,50), 2));
            ellipsePainter.drawEllipse(2,2,20,20);
        ToolbarButton* lineProperty = new ToolbarButton(mainwindow, shapeStyleProperty, 0,0, 30,30, SUB_ICON, "", false, *lineIcon);
        ToolbarButton* rectProperty = new ToolbarButton(mainwindow, shapeStyleProperty, 30,0, 30,30, SUB_ICON, "", false, *rectIcon);
        ToolbarButton* ellipseProperty = new ToolbarButton(mainwindow, shapeStyleProperty, 60,0, 30,30, SUB_ICON, "", false, *ellipseIcon);
        connect(lineProperty, &QAbstractButton::pressed, this, [this]{ editor()->shapeSubtool = LINE; toolbar->setCurrentTool(TOOL2);});
        connect(rectProperty, &QAbstractButton::pressed, this, [this]{ editor()->shapeSubtool = RECTANGLE; toolbar->setCurrentTool(TOOL2);});
        connect(ellipseProperty, &QAbstractButton::pressed, this, [this]{ editor()->shapeSubtool = ELLIPSE; toolbar->setCurrentTool(TOOL2);});

    shapeColorProperty = new ToolbarButton(mainwindow, mainwindow, 80, 110, 120, 120, SUB_EMPTY);
        ColorWheel* shapeColorwheel = new ColorWheel(shapeColorProperty);
        shapeColorwheel->resize(120, 120);
        connect(shapeColorwheel, &ColorWheel::colorChanged, this, [shapeColorwheel, this]{
            QPen* p = editor()->getShapeTool();
            p->setColor(QColor(shapeColorwheel->color().red(), shapeColorwheel->color().green(), shapeColorwheel->color().blue(), p->color().alpha()));
            toolbar->setCurrentTool(TOOL2);
        });
        connect(shapeColorwheel, &ColorWheel::colorSelected, this, [shapeColorwheel, this]{
            QPen* p = editor()->getShapeTool();
            p->setColor(QColor(shapeColorwheel->color().red(), shapeColorwheel->color().green(), shapeColorwheel->color().blue(), p->color().alpha()));
            toolbar->setCurrentTool(TOOL2);
        });

    shapeOpacityProperty = new ToolbarButton(mainwindow, mainwindow, 80, 150, 120, 40, SUB_EMPTY);
        QSlider* shapeOpacitySlider = new QSlider(Qt::Horizontal, shapeOpacityProperty);
        shapeOpacitySlider->setGeometry(5, 0, 110, 40);
        shapeOpacitySlider->setRange(0, 255);
        shapeOpacitySlider->setValue(editor()->getShapeTool()->color().alpha());
        shapeOpacitySlider->setStyleSheet(sliderStylesheet);
        connect(shapeOpacitySlider, &QAbstractSlider::valueChanged, this, [shapeOpacitySlider, this]{
            QPen* p = editor()->getShapeTool();
            p->setColor(QColor(p->color().red(), p->color().green(), p->color().blue(), shapeOpacitySlider->value()));
            toolbar->setCurrentTool(TOOL2);
        });

    shapeWidthProperty = new ToolbarButton(mainwindow, mainwindow, 80, 190, 120, 40, SUB_EMPTY);
        QSlider* shapeWidthSlider = new QSlider(Qt::Horizontal, shapeWidthProperty);
        shapeWidthSlider->setGeometry(5, 0, 110, 40);
        shapeWidthSlider->setRange(1, 30);
        shapeWidthSlider->setValue(editor()->getShapeTool()->width());
        shapeWidthSlider->setStyleSheet(sliderStylesheet);
        connect(shapeWidthSlider, &QAbstractSlider::valueChanged, this, [shapeWidthSlider, this]{
            editor()->getShapeTool()->setWidth(shapeWidthSlider->value());
            toolbar->setCurrentTool(TOOL2);
        });

    fillShapeProperty = new ToolbarButton(mainwindow, mainwindow, 80, 70, 98, 40, SUB_EMPTY);
        QImage* lassofillIcon = new QImage(":/icons/lasso.png");
        QImage* rectfillIcon = new QImage(26, 26, QImage::Format_ARGB32);
            rectfillIcon->fill(Qt::transparent);
            QPainter rectfillPainter(rectfillIcon);
            rectfillPainter.setPen(Qt::transparent);
            rectfillPainter.setBrush(QBrush(QColor(50,50,50)));
            rectfillPainter.drawRect(2,2,20,20);
        QImage* ellipsefillIcon = new QImage(26, 26, QImage::Format_ARGB32);
            ellipsefillIcon->fill(Qt::transparent);
            QPainter ellipsefillPainter(ellipsefillIcon);
            ellipsefillPainter.setPen(Qt::transparent);
            ellipsefillPainter.setBrush(QBrush(QColor(50,50,50)));
            ellipsefillPainter.drawEllipse(2,2,20,20);
        ToolbarButton* lassofillProperty = new ToolbarButton(mainwindow, fillShapeProperty, 0,0, 30,30, SUB_ICON, "", false, *lassofillIcon);
        ToolbarButton* rectfillProperty = new ToolbarButton(mainwindow, fillShapeProperty, 30,0, 30,30, SUB_ICON, "", false, *rectfillIcon);
        ToolbarButton* ellipsefillProperty = new ToolbarButton(mainwindow, fillShapeProperty, 60,0, 30,30, SUB_ICON, "", false, *ellipsefillIcon);
        connect(lassofillProperty, &QAbstractButton::pressed, this, [this]{ editor()->fillSubtool = LASSO; toolbar->setCurrentTool(TOOL3);});
        connect(rectfillProperty, &QAbstractButton::pressed, this, [this]{ editor()->fillSubtool = RECTANGLE; toolbar->setCurrentTool(TOOL3);});
        connect(ellipsefillProperty, &QAbstractButton::pressed, this, [this]{ editor()->fillSubtool = ELLIPSE; toolbar->setCurrentTool(TOOL3);});

    fillColorProperty = new ToolbarButton(mainwindow, mainwindow, 80, 110, 120, 120, SUB_EMPTY);
        ColorWheel* lassoColorwheel = new ColorWheel(fillColorProperty);
        lassoColorwheel->resize(120, 120);
        connect(lassoColorwheel, &ColorWheel::colorChanged, this, [lassoColorwheel, this]{
            QBrush* p = editor()->getFillTool();
            p->setColor(QColor(lassoColorwheel->color().red(), lassoColorwheel->color().green(), lassoColorwheel->color().blue(), p->color().alpha()));
            toolbar->setCurrentTool(TOOL3);
        });
        connect(lassoColorwheel, &ColorWheel::colorSelected, this, [lassoColorwheel, this]{
            QBrush* p = editor()->getFillTool();
            p->setColor(QColor(lassoColorwheel->color().red(), lassoColorwheel->color().green(), lassoColorwheel->color().blue(), p->color().alpha()));
            toolbar->setCurrentTool(TOOL3);
        });

    fillOpacityProperty = new ToolbarButton(mainwindow, mainwindow, 80, 150, 120, 40, SUB_EMPTY);
        QSlider* lassoOpacitySlider = new QSlider(Qt::Horizontal, fillOpacityProperty);
        lassoOpacitySlider->setGeometry(5, 0, 110, 40);
        lassoOpacitySlider->setRange(0, 255);
        lassoOpacitySlider->setValue(editor()->getFillTool()->color().alpha());
        lassoOpacitySlider->setStyleSheet(sliderStylesheet);
        connect(lassoOpacitySlider, &QAbstractSlider::valueChanged, this, [lassoOpacitySlider, this]{
            QBrush* p = editor()->getFillTool();
            p->setColor(QColor(p->color().red(), p->color().green(), p->color().blue(), lassoOpacitySlider->value()));
            toolbar->setCurrentTool(TOOL3);
        });

    fillStyleProperty = new ToolbarButton(mainwindow, mainwindow, 80, 190, 98, 158, SUB_EMPTY);
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
                ToolbarButton* t = new ToolbarButton(mainwindow, fillStyleProperty, 30*j,  30*i,  30, 30, SUB_ICON, "", false, *styleIcon);
                connect(t, &QAbstractButton::pressed, this, [this, currentStyle]{ editor()->getFillTool()->setStyle(static_cast<Qt::BrushStyle>(currentStyle)); toolbar->setCurrentTool(TOOL3); });
                currentStyle += 1;
            }
        }

    selectStyleProperty = new ToolbarButton(mainwindow, mainwindow, 80, 70, 64, 40, SUB_EMPTY);
        QImage* selectrectIcon = new QImage(26, 26, QImage::Format_ARGB32);
            selectrectIcon->fill(Qt::transparent);
            QPainter selectrectPainter(selectrectIcon);
            selectrectPainter.setPen(QPen(QColor(50,50,50), 2, Qt::DashLine));
            selectrectPainter.drawRect(2,2,20,20);
        QImage* selectlassoIcon = new QImage(26, 26, QImage::Format_ARGB32);
            selectlassoIcon->fill(Qt::transparent);
            QPainter selectlassoPainter(selectlassoIcon);
            selectlassoPainter.setPen(QPen(QColor(50,50,50), 2,  Qt::DashLine));
            selectlassoPainter.drawEllipse(2,2,20,20);
        ToolbarButton* selectrectProperty = new ToolbarButton(mainwindow, selectStyleProperty, 0, 2, 30,30, SUB_ICON, "", false, *selectrectIcon);
        ToolbarButton* selectlassoProperty = new ToolbarButton(mainwindow, selectStyleProperty, 30, 2, 30,30, SUB_ICON, "", false, *selectlassoIcon);
        connect(selectrectProperty, &QAbstractButton::pressed, this, [this]{ editor()->drawSelect(); editor()->selectSubtool = RECTANGLE; toolbar->setCurrentTool(TOOL6);});
        connect(selectlassoProperty, &QAbstractButton::pressed, this, [this]{ editor()->drawSelect(); editor()->selectSubtool = LASSO; toolbar->setCurrentTool(TOOL6);});

    eraserWidthProperty = new ToolbarButton(mainwindow, mainwindow, 80, 70, 120, 40, SUB_EMPTY);
        QSlider* eraserWidthSlider = new QSlider(Qt::Horizontal, eraserWidthProperty);
        eraserWidthSlider->setGeometry(5, 0, 110, 40);
        eraserWidthSlider->setRange(1, 50);
        eraserWidthSlider->setValue(editor()->getEraserTool()->width());
        eraserWidthSlider->setStyleSheet(sliderStylesheet);
        connect(eraserWidthSlider, &QAbstractSlider::valueChanged, this, [eraserWidthSlider, this]{
            editor()->getEraserTool()->setWidth(eraserWidthSlider->value());
            toolbar->setCurrentTool(TOOL4);
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
            toolbar->setCurrentTool(TOOL5);
        });
}

void Subtoolbar::hideProperties()
{
    penColorProperty->hide();
    penOpacityProperty->hide();
    penWidthProperty->hide();
    shapeStyleProperty->hide();
    shapeColorProperty->hide();
    shapeOpacityProperty->hide();
    shapeWidthProperty->hide();
    fillShapeProperty->hide();
    fillColorProperty->hide();
    fillOpacityProperty->hide();
    fillStyleProperty->hide();
    eraserWidthProperty->hide();
    bgColorProperty->hide();
    bgOpacityProperty->hide();
    selectStyleProperty->hide();
}

void Subtoolbar::clickSubtool(ToolbarTool sub)
{
    bool isPenColorVisible = penColorProperty->isVisible();
    bool isPenOpacityVisible = penOpacityProperty->isVisible();
    bool isPenWidthVisible = penWidthProperty->isVisible();
    bool isShapeStyleVisible = shapeStyleProperty->isVisible();
    bool isShapeColorVisible = shapeColorProperty->isVisible();
    bool isShapeOpacityVisible = shapeOpacityProperty->isVisible();
    bool isShapeWidthVisible = shapeWidthProperty->isVisible();
    bool isFillShapeVisible = fillShapeProperty->isVisible();
    bool isFillColorVisible = fillColorProperty->isVisible();
    bool isFillOpacityVisible = fillOpacityProperty->isVisible();
    bool isFillStyleVisible = fillStyleProperty->isVisible();
    bool isEraserWidthVisible = eraserWidthProperty->isVisible();
    bool isBgColorVisible = bgColorProperty->isVisible();
    bool isBgOpacityVisible = bgOpacityProperty->isVisible();
    bool isSelectStyleisible = selectStyleProperty->isVisible();
    hideProperties();

    switch(toolbar->getCurrentTool())
    {
        case TOOL1:
            switch(sub){
                case SUB1: if(!isPenColorVisible) penColorProperty->show(); break;
                case SUB2: if(!isPenOpacityVisible) penOpacityProperty->show(); break;
                case SUB3: if(!isPenWidthVisible) penWidthProperty->show(); break;
                case SUB4: break;
                case SUB5: break;
                default: break;
            } break;
        case TOOL2:
            switch(sub){
                case SUB1: if(!isShapeStyleVisible) shapeStyleProperty->show(); break;
                case SUB2: if(!isShapeColorVisible) shapeColorProperty->show(); break;
                case SUB3: if(!isShapeOpacityVisible) shapeOpacityProperty->show(); break;
                case SUB4: if(!isShapeWidthVisible) shapeWidthProperty->show(); break;
                case SUB5: break;
                default: break;
            } break;
        case TOOL3:
            switch(sub){
                case SUB1: if(!isFillShapeVisible) fillShapeProperty->show(); break;
                case SUB2: if(!isFillColorVisible) fillColorProperty->show(); break;
                case SUB3: if(!isFillOpacityVisible) fillOpacityProperty->show(); break;
                case SUB4: if(!isFillStyleVisible) fillStyleProperty->show(); break;
                case SUB5: break;
                default: break;
            } break;
        case TOOL4:
            switch(sub){
                case SUB1: if(!isEraserWidthVisible) eraserWidthProperty->show(); break;
                case SUB2: break;
                case SUB3: break;
                case SUB4: break;
                case SUB5: break;
                default: break;
            } break;
        case TOOL6:
            switch(sub){
                case SUB1: if (!isSelectStyleisible) selectStyleProperty->show(); break;
                case SUB2: break;
                case SUB3: break;
                case SUB4: break;
                case SUB5: break;
                default: break;
        } break;
        case TOOL5:
            switch(sub){
                case SUB1: if(!isBgColorVisible) bgColorProperty->show(); break;
                case SUB2: if(!isBgOpacityVisible) bgOpacityProperty->show(); break;
                case SUB3: editor()->toggleOnionskin(); editor()->setToolAsEmpty();  break;
                case SUB4: editor()->toggleOnionskinloop(); editor()->setToolAsEmpty(); break;
                case SUB5: mainwindow->toggleStayOnTop(); editor()->setToolAsEmpty(); break;
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
