#include "pc_udp_client.h"
#include "mainwindow.h"

pc_udp_client::pc_udp_client(QObject *parent) :
    QObject(parent)
{

    socket = new QUdpSocket(this);
    socket->bind(QHostAddress("192.168.3.80"), 8881); //本地地址
    pc_client_send_data();
}

void pc_udp_client::pc_client_send_data()
{

    //QByteArray send_msg = "hello qgc ";

    if(remote_key!=0)  //rotate_key_limit------10m
    {
        if (remote_key <= 0)
        {
            rotate_key = 0;
        }
        else if (remote_key >= 1000)
        {
            rotate_key = 1000;
        }
    }

    if (remote_switch_1 != 0)
     {
            btn_1 = 0; btn_2 = 0;

             joy_x = remote_switch_key * remote_x;
             joy_y = 0 * remote_switch_key * (remote_y);
             rotate_key = remote_key;

              knob_1 = remote_knob_1;
              knob_2 = remote_knob_2;

            if ( remote_switch_2 == 1 )
            {
                knob_3 = 0 * remote_knob_3;
                knob_4 = remote_switch_2 * remote_knob_4;
            }
            else
            {
                knob_3 = remote_switch_2 * remote_knob_3;
                knob_4 = 0 * remote_knob_4;
            }
                cam_1 = remote_cam_1;
                cam_2 = remote_cam_2;
     }
     else
     {
            joy_x = 0, joy_y = 0, rotate_key = 0;
            knob_3 = 0, knob_4 = 0;
            btn_1 = 0; btn_2 = 0;

            knob_1 = remote_knob_1;
            knob_2 = remote_knob_2;
            cam_1 = remote_cam_1;
            cam_2 = remote_cam_2;
      }
     height_p=250.25; height_i=250.25; heigh_d=250.25; height_i_limit=100.25; height_out_limit=100.25;   //PID
     motor_p=250.25; motor_i=250.25; motor_d=250.25; motor_i_limit=100.25; motor_out_limit=100.25;
     locate_p=250.25; locate_i=250.25; locate_d=250.25; locate_i_limit=250.25; locate_out_limit=250.25;

    char char_data[512]="";

    sprintf(char_data, "%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s\n",
            "$",height_p,height_i,heigh_d,height_i_limit,height_out_limit,
            motor_p,motor_i,motor_d,motor_i_limit,motor_out_limit,
            locate_p,locate_i,locate_d,locate_i_limit,locate_out_limit,
            joy_x,joy_y,rotate_key,knob_1,knob_2,knob_3,knob_4,cam_1,cam_2,btn_1,btn_2,"#");


    QHostAddress serverAddress = QHostAddress("192.168.3.88");
    socket->writeDatagram(char_data,sizeof(char_data),QHostAddress(serverAddress),8882);

    qDebug()<<"server send:";
    qDebug()<<char_data;

}

pc_udp_client::~pc_udp_client()
{
    delete socket;

}
