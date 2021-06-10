#ifndef QGC_UDP_SERVER_H
#define QGC_UDP_SERVER_H

#include <QObject>
#include <QUdpSocket>
#include <iostream>
#include <QDebug>


class qgc_udp_server : public QObject
{
    Q_OBJECT

public:
    explicit qgc_udp_server(QString host, QObject *parent = nullptr);
    ~qgc_udp_server();

//    void qgc_server_send_data();

    int uuv_0;
    float uuv_1,uuv_2,uuv_3,uuv_4,uuv_5;

    float height_p,height_i,heigh_d,height_i_limit,height_out_limit;
    float motor_p,motor_i,motor_d,motor_i_limit,motor_out_limit;
    float locate_p,locate_i,locate_d,locate_i_limit,locate_out_limit;

    int joy_x,joy_y,rotate_key,knob_1,knob_2,knob_3,knob_4;
    int cam_1,cam_2,btn_1,btn_2;

signals:

    void qgc_server_recv_message(int uuv_state,float uuv_roll,float uuv_pitch,float uuv_yaw,float uuv_depth,float uuv_voltage);

public slots:

    void qgc_server_recv_data();

    void qgc_server_send_data();

private:


    QUdpSocket *socket;
    QString qgc_server_ip;

};


#endif // QGC_UDP_SERVER_H
