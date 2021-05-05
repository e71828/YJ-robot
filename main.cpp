#include "mainwindow.h"
#include <QApplication>
#include <QPixmap>
#include <QSplashScreen>
#include <QStyle>
#include <QScreen>
#include <QGuiApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFont font  = a.font();
        font.setPointSize(14);
        a.setFont(font);


    QPixmap pixmap(":/icons/splash.png");

    QSplashScreen splash(pixmap.scaled(QSize(300,200),  Qt::KeepAspectRatio));
    splash.show();
    a.processEvents(); //相应其他事件

    MainWindow w;
    w.setGeometry(
       QStyle::alignedRect(
           Qt::LeftToRight,
           Qt::AlignCenter,
           w.size(),
           QGuiApplication::primaryScreen()->availableGeometry()
       )
    );
    w.show();
    splash.finish(&w);
    return a.exec();
}
