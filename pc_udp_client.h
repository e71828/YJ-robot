#ifndef PC_UDP_CLIENT_H
#define PC_UDP_CLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <iostream>
#include <QDebug>
#include <string.h>
#include <cstring>
#include "mainwindow.h"

namespace Ui {
class MainWindow;
}

class pc_udp_client : public QObject
{
    Q_OBJECT

public:
    explicit pc_udp_client(QObject *parent = nullptr);
    ~pc_udp_client();

    float height_p,height_i,heigh_d,height_i_limit,height_out_limit;
    float motor_p,motor_i,motor_d,motor_i_limit,motor_out_limit;
    float locate_p,locate_i,locate_d,locate_i_limit,locate_out_limit;

    int joy_x,joy_y,rotate_key,knob_1,knob_2,knob_3,knob_4;
    int cam_1,cam_2,btn_1,btn_2;

    void pc_client_send_data();

public slots:



private:

    QUdpSocket *socket;

};

#endif // PC_UDP_CLIENT_H
