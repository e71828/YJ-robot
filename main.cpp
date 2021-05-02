#include "mainwindow.h"
#include <QApplication>
#include <QPixmap>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFont font  = a.font();
        font.setPointSize(14);
        a.setFont(font);


    QPixmap pixmap("C:\\Users\\dell\\Desktop\\eimo_ws\\robot\\04.png");

    QSplashScreen splash(pixmap);
    splash.show();
    a.processEvents(); //相应其他事件

    MainWindow w;
    w.show();
    splash.finish(&w);
    return a.exec();
}
