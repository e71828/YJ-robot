#include "remote_joy.h"
#include "mainwindow.h"

QString serial_port;
int open_serial_flag;

remote_joy::remote_joy(QObject *parent) :
    QObject(parent)
{

    if (open_serial_flag != 0)
    {
        open_serial();
    }
    else
    {
        close_serial();
    }
}

void remote_joy::open_serial()
{
    serial = new QSerialPort;
    //设置串口名
    serial->setPortName(serial_port);
    qDebug()<<"serial_port"<<serial_port;

    serial->setPortName("COM23");
    //打开串口
    serial->open(QIODevice::ReadWrite);
    //设置波特率
    serial->setBaudRate(QSerialPort::Baud115200);//设置波特率为115200
    //设置数据位数
     serial->setDataBits(QSerialPort::Data8);//设置数据位8

     serial->setParity(QSerialPort::NoParity);

    //设置停止位
    serial->setStopBits(QSerialPort::OneStop);//停止位设置为1
    //设置流控制
    serial->setFlowControl(QSerialPort::NoFlowControl);//设置为无流控制

    //连接信号槽
    //connect(serial, SIGNAL(readyRead()), this, SLOT(serial_read_data()));
    QObject::connect(serial,&QSerialPort::readyRead,this,&remote_joy::serial_read_data);

}
void remote_joy::close_serial()
{
    //关闭串口
    serial->clear();
    serial->close();
    serial->deleteLater();
}

//读取接收到的信息
void remote_joy::serial_read_data()
{
    char data_first[16];char data_last[16];
    QByteArray read_buf;
    QString str = "";
    read_buf = serial->readAll();
    if(!read_buf.isEmpty())
    {
        str+=tr(read_buf);
    }

    qDebug()<< "read data";
    qDebug()<<  str;

    QByteArray joy_data = str.toLatin1(); // must
    qDebug()<<  joy_data;

    sscanf(joy_data, "%[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s\r\n",data_first,&remote_x,&remote_y,&remote_z,&remote_a,&remote_b,&remote_c,&remote_knob_1,&remote_knob_2,&remote_knob_3,&remote_knob_4,
              &remote_joy_1,&remote_joy_2,&remote_joy_3,&remote_joy_4,&remote_joy_5,&remote_joy_6,&remote_joy_7,&remote_joy_8,&remote_joy_9,&remote_joy_10,data_last);

    remote_1 = remote_x;remote_2=remote_y;remote_3 = remote_a;remote_4=remote_b;

    remote_5 = remote_knob_1;remote_6 = remote_knob_3;remote_7 = remote_knob_2;remote_8 = remote_knob_4;

    remote_9 = remote_joy_1;remote_10 = remote_joy_2;remote_11 = remote_joy_3;remote_12 = remote_joy_4;remote_13 = remote_joy_5;


    remote_14 = remote_joy_6;remote_15 = remote_joy_7;remote_16 = remote_joy_8;remote_17 = remote_joy_9;remote_18 = remote_joy_10;


    com_joy_message(remote_1,remote_2,remote_3,remote_4,remote_5,remote_6,remote_7,remote_8,remote_9,remote_10,remote_11,remote_12,remote_13,remote_14,remote_15,remote_16,remote_17,remote_18);

    serial_write_data();
}

//发送按钮槽函数
void remote_joy::serial_write_data()
{
    QByteArray write_buf = "hello";

    serial->write(write_buf.data(),write_buf.size());

    qDebug()<< "write data";
    qDebug()<< write_buf.data();

}

remote_joy::~remote_joy()
{
    delete serial;
}

