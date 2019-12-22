#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    //QDesktopWidget *desk = new QDesktopWidget();
//    window.setGeometry(
//        QStyle::alignedRect(
//            Qt::LeftToRight,
//            Qt::AlignCenter,
//            window.size(),
//            desk->screenGeometry(1)
//        )
//    );
    window.setGeometry(50, 50, 850, 650);
    window.show();

    return app.exec();
}
