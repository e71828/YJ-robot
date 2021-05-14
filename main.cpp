#include "mainwindow.h"
#include <QApplication>
#include <QPixmap>
#include <QSplashScreen>
#include <QStyle>
#include <QScreen>
#include <QGuiApplication>

QString qgc_server_ip="192.168.31.99"; // 本地端口，接受姿态数据
int qgc_server_port = 8888;

QString dest_server_ip="192.168.31.16"; //远程端口，发送控制数据
int dest_server_port = 8888;

int qgc_video1_port = 8555;
int qgc_video2_port = 8556;


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
