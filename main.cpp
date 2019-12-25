#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;

    window.setGeometry(100, 100, 850, 650);
    window.show();

    return app.exec();
}
