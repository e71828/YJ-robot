#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QDebug>
#include <QPushButton>
#include <QUdpSocket>
#include <iostream>
#include <QListView>
#include <QFileDialog>
#include <QLabel>
#include <QTime>
#include <QTimer>
#include <QThread>
#include <QUrl>
#include <QFile>
#include <QString>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QGridLayout>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QHostInfo>
#include "qgc_udp_server.h"

using namespace std;

//extern QString qgc_server_ip;
extern int qgc_server_port;

extern QString dest_server_ip;
extern int dest_server_port;

extern int qgc_video1_port;
extern int qgc_video2_port;

extern int remote_x,remote_y,remote_key,remote_knob_1,remote_knob_2,remote_knob_3,remote_knob_4;
extern int remote_cam_1,remote_cam_2,remote_switch_key,remote_switch_1,remote_switch_2,remote_btn_1,remote_btn_2;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString port;  //serial
    QByteArray joy_data;
    int joy_1;
    int joy_2;
    int rotate_key;
    int knob_1;
    int knob_2;
    int knob_3;
    int knob_4;
    int rotate_cam_1;
    int rotate_cam_2;
    int switch_key;
    int switch_1;
    int switch_2;
    int btn_1;
    int btn_2;

    void find_serial();
    void open_serial();
    void close_serial();

    void serial_write_data();
    void show_video();

    QString find_IP();

    //recv sensor data
      int   uuv_state;
      float uuv_roll;
      float uuv_pitch;
      float uuv_yaw;
      float uuv_depth;
      float uuv_voltage;


signals:

private slots:

    void serial_read_data();

    void serial_message(int joy_1,int joy_2,int rotate_key,int knob_1,int knob_2,int knob_3,int knob_4,int rotate_cam_1,int rotate_cam_2,int switch_key,int switch_1,int switch_2,int btn_1,int btn_2);

    void show_time();            //显示当前的时间

    void update_data(int uuv_state,float uuv_roll,float uuv_pitch,float uuv_yaw,float uuv_depth,float uuv_voltage);

    void changeDuration(qint64 seconds);

    void on_videoButton_released();

    bool eventFilter(QObject *, QEvent *);

private:

    QSerialPort *serial;
    QString str_Height;
    QString str_joy_1;
    QString str_joy_2;
    QString str_rotate_key;
    QString str_knob_1;
    QString str_knob_2;
    QString str_knob_3;
    QString str_knob_4;
    QString str_rotate_cam_1;
    QString str_rotate_cam_2;
    QString str_switch_key;
    QString str_switch_1;
    QString str_switch_2;
    QString str_btn_1;
    QString str_btn_2;
    int temp_depth_key0;
    int temp_depth_key1;
    int temp_depth_key2;
    bool depth_init;


    qgc_udp_server * qgc_message;
    QString str_state;
    QString str_roll;
    QString str_pitch;
    QString str_yaw;
    QString str_depth;
    QString str_voltage;

    QString str_Text;

    bool showColon;             //用于显示时间时是否显示“:”
    bool updateData;

    QString host;

    Ui::MainWindow *ui;

    QString nbSource1;
    QString nbSource2;

};

#endif // MAINWINDOW_H
