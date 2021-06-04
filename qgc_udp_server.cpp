#include "qgc_udp_server.h"
#include "mainwindow.h"

int remote_x,remote_y,remote_key,remote_knob_1,remote_knob_2,remote_knob_3,remote_knob_4;
int remote_cam_1,remote_cam_2,remote_switch_key,remote_switch_1,remote_switch_2,remote_btn_1,remote_btn_2;

extern QString dest_server_ip;
extern int dest_server_port;

extern QString qgc_server_ip;
extern int qgc_server_port;

qgc_udp_server::qgc_udp_server(QObject *parent) :
    QObject(parent)
{

    socket = new QUdpSocket(this);

    socket->bind(QHostAddress(qgc_server_ip), qgc_server_port);

//    qgc_server_send_data();

    connect(socket, SIGNAL(readyRead()), this, SLOT(qgc_server_recv_data()));

}

void qgc_udp_server::qgc_server_recv_data()
{
     QByteArray qgc_recv_msg;
     char data_first[16];char data_last[16];

    while(socket->hasPendingDatagrams())
    {
        qgc_recv_msg.resize(socket->pendingDatagramSize());
        socket->readDatagram(qgc_recv_msg.data(), qgc_recv_msg.size());
        qDebug()<<"server recv:";
        std::cout << qgc_recv_msg.data() << std::endl;
    }
    sscanf(qgc_recv_msg.data(), "%[^,],%d,%f,%f,%f,%f,%f,%s\n",data_first,&uuv_0,&uuv_1,&uuv_2,&uuv_3,&uuv_4,&uuv_5,data_last);

    emit qgc_server_recv_message(uuv_0, uuv_1, uuv_2, uuv_3, uuv_4, uuv_5);

//    qgc_server_send_data();

}

void qgc_udp_server::qgc_server_send_data()
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

             joy_x = remote_switch_key/2 * remote_x /100;
             joy_y = remote_switch_key/2 * remote_y /100;
             cam_1 = remote_cam_1;
             cam_2 = remote_cam_2;
             rotate_key = remote_key*100+remote_cam_2;

              knob_1 = remote_knob_1;
              knob_2 = remote_knob_2;

            if ( remote_switch_2 == 1 )
            {
                knob_3 = 0 ;
                knob_4 = remote_knob_4;
            }
            else
            {
                knob_3 =  remote_knob_3/15;
                knob_4 = 0 ;
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

         height_p=20.25; height_i=0.3; heigh_d=15.25; height_i_limit=100; height_out_limit=200;//高度
         motor_p=5; motor_i=0.5; motor_d=5; motor_i_limit=100.25; motor_out_limit=100.25;//前后
         locate_p=20; locate_i=3; locate_d=15; locate_i_limit=100.25; locate_out_limit=250.25;//左右

    char char_data[512]="";

    sprintf(char_data, "%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s\n",
            "$",height_p,height_i,heigh_d,height_i_limit,height_out_limit,
            motor_p,motor_i,motor_d,motor_i_limit,motor_out_limit,
            locate_p,locate_i,locate_d,locate_i_limit,locate_out_limit,
            joy_x,joy_y,rotate_key,knob_1,knob_2,knob_3,knob_4,cam_1,cam_2,btn_1,btn_2,"#");

    socket->writeDatagram(char_data,sizeof(char_data),QHostAddress(dest_server_ip),dest_server_port);

    qDebug()<<"server send:";
    qDebug()<<char_data;

}

qgc_udp_server::~qgc_udp_server()
{
    socket->close();
    socket->deleteLater();
}
