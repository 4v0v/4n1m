#ifndef PREVIEW_H
#define PREVIEW_H

#include <QWidget>

class Object;
class Preview : public QWidget
{
    Q_OBJECT

public:
    Preview(Object *o = nullptr, QWidget *parent = nullptr);

    void setObject(Object* o) { object = o; }

public slots:
    void play();

protected:
    void paintEvent(QPaintEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;


private:
    Object* object;

    int currentPosition = 0;
};


#endif
