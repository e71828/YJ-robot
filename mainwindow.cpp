#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qgc_udp_server.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(" 油浸式变压器检测机器人控制系统 ");


    //查找可用的串口
    foreach (const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))
        {
            ui->PortBox->addItem(serial.portName());
            serial.close();
        }
    }

    //点击串口[开启]/[关闭]按钮
    connect(ui->OpenSerialButton,&QPushButton::clicked,this,[this]()
    {

        if(ui->OpenSerialButton->text()=="打开串口")
        {
            port = ui->PortBox->currentText();
            open_serial();
            //关闭设置菜单使能
            ui->OpenSerialButton->setText(tr("关闭串口"));
        }
        else
        {
            close_serial();
            ui->OpenSerialButton->setText(tr("打开串口"));
        }
    });


    //qgc[开启]/[关闭]按钮
    ui->lineEdit_qgc_server_ip->setText("192.168.3.80"); //本地地址
    ui->lineEdit_qgc_server_port->setText("8888");
    qgc_server_ip = ui->lineEdit_qgc_server_ip->text();
    qgc_server_port = ui->lineEdit_qgc_server_port->text().toInt();

    connect(ui->OpenButton,&QPushButton::clicked,this,[this]()
    {

        if(ui->OpenButton->text()=="打开端口")
        {
            qgc_message = new qgc_udp_server;
            connect(qgc_message,&qgc_udp_server::qgc_server_recv_message,this,&MainWindow::qgc_server_recv_data);
            qgc_udp_server();

            //关闭设置菜单使能
            ui->OpenButton->setText(tr("关闭端口"));
        }
        else
        {
            ui->OpenButton->setText(tr("打开端口"));

        }
    });

    //显示时间
      QTimer *timer=new QTimer(this);			//新建一个定时器对象
      connect(timer,SIGNAL(timeout()),this,SLOT(show_time()));	//(d)
      timer->start(100);						//(e)
      show_time();								//初始时间显示
      showColon=true;                            //初始化

}

void sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void MainWindow::open_serial()
{
    serial = new QSerialPort;
    //设置串口名
    serial->setPortName(port);
    //serial->setPortName("COM2");
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

    //关闭设置菜单使能
    ui->OpenSerialButton->setText(tr("关闭串口"));

    //连接信号槽
    QObject::connect(serial,&QSerialPort::readyRead,this,&MainWindow::serial_read_data);

}

//读取接收到的信息
void MainWindow::serial_read_data()
{
    sleep(10);
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

    sscanf(joy_data, "%[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s\r\n",data_first,&joy_1,&joy_2,&rotate_key,
           &knob_1,&knob_2,&knob_3,&knob_4,&rotate_cam_1,&rotate_cam_2,&switch_key,&switch_1,&switch_2,&btn_1,&btn_2,data_last);

     remote_x = joy_1;remote_y = joy_2;remote_key=rotate_key;remote_knob_1 = knob_1;remote_knob_2 = knob_2;remote_knob_3 = knob_3;remote_knob_4 = knob_4;
     remote_cam_1 = rotate_cam_1;remote_cam_2 = rotate_cam_2;remote_switch_key = switch_key;remote_switch_1 = switch_1;remote_switch_2 = switch_2;remote_btn_1 = btn_1;remote_btn_2 = btn_2;

    serial_message(joy_1,joy_2,rotate_key,knob_1,knob_2,knob_3,knob_4,rotate_cam_1,rotate_cam_2,switch_key,switch_1,switch_2,btn_1,btn_2);

}

void MainWindow::close_serial()
{
    //关闭串口
    serial->clear();
    serial->close();
    serial->deleteLater();

}

void MainWindow::serial_message(int joy_1,int joy_2,int rotate_key,int knob_1,int knob_2,int knob_3,int knob_4,int rotate_cam_1,int rotate_cam_2,int switch_key,int switch_1,int switch_2,int btn_1,int btn_2)
{

    ui->lineEdit_joy_1->setText(str_joy_1.setNum(joy_1));
        ui->lineEdit_joy_1->setStyleSheet("font-size:24px");
    ui->lineEdit_joy_2->setText(str_joy_2.setNum(joy_2));
        ui->lineEdit_joy_2->setStyleSheet("font-size:24px");
    ui->lineEdit_rotate_key->setText(str_rotate_key.setNum(rotate_key));
        ui->lineEdit_rotate_key->setStyleSheet("font-size:24px");
    ui->lineEdit_knob_1->setText(str_knob_1.setNum(knob_1));
        ui->lineEdit_knob_1->setStyleSheet("font-size:24px");
    ui->lineEdit_knob_2->setText(str_knob_2.setNum(knob_2));
         ui->lineEdit_knob_2->setStyleSheet("font-size:24px");
    ui->lineEdit_knob_3->setText(str_knob_3.setNum(knob_3));
        ui->lineEdit_knob_3->setStyleSheet("font-size:24px");
    ui->lineEdit_knob_4->setText(str_knob_4.setNum(knob_4));
        ui->lineEdit_knob_4->setStyleSheet("font-size:24px");


    ui->lineEdit_rotate_cam_1->setText(str_rotate_cam_1.setNum(rotate_cam_1));
        ui->lineEdit_rotate_cam_1->setStyleSheet("font-size:24px");
    ui->lineEdit_rotate_cam_2->setText(str_rotate_cam_2.setNum(rotate_cam_2));
         ui->lineEdit_rotate_cam_2->setStyleSheet("font-size:24px");
    ui->lineEdit_switch_key->setText(str_switch_key.setNum(switch_key));
         ui->lineEdit_switch_key->setStyleSheet("font-size:24px");
    //ui->lineEdit_switch_1->setText(str_switch_1.setNum(switch_1));
        ui->lineEdit_switch_1->setStyleSheet("font-size:24px");
        if (switch_1 != 0 )
        {
            QPalette p=QPalette();
            p.setColor(QPalette::Base,Qt::green);
            p.setColor(QPalette::WindowText,Qt::green);
            p.setColor(QPalette::Background,QColor(0,255,0,255));
            ui->lineEdit_switch_1->setPalette(p);
        }
        else
        {
            QPalette p=QPalette();
            p.setColor(QPalette::Base,Qt::red);
            p.setColor(QPalette::WindowText,Qt::red);
            p.setColor(QPalette::Background,QColor(255,0,0,255));
            ui->lineEdit_joy_1->setPalette(p);
            ui->lineEdit_joy_2->setPalette(p);
            ui->lineEdit_rotate_key->setPalette(p);
            ui->lineEdit_knob_1->setPalette(p);
            ui->lineEdit_knob_2->setPalette(p);
            ui->lineEdit_knob_3->setPalette(p);
            ui->lineEdit_knob_4->setPalette(p);
            ui->lineEdit_rotate_cam_1->setPalette(p);
            ui->lineEdit_rotate_cam_2->setPalette(p);
            ui->lineEdit_switch_key->setPalette(p);
            ui->lineEdit_switch_1->setPalette(p);
            ui->lineEdit_switch_2->setPalette(p);
            ui->lineEdit_btn_1->setPalette(p);
            ui->lineEdit_btn_2->setPalette(p);
        }

    ui->lineEdit_switch_2->setText(str_switch_2.setNum(switch_2));
        ui->lineEdit_switch_2->setStyleSheet("font-size:24px");
        if (switch_2 != 0 )
        {
            if (switch_1 != 0 )
            {
                QPalette p=QPalette();
                p.setColor(QPalette::Base,Qt::green);
                p.setColor(QPalette::WindowText,Qt::green);
                p.setColor(QPalette::Background,QColor(0,255,0,255));
                ui->lineEdit_switch_2->setPalette(p);
                ui->lineEdit_knob_4->setPalette(p);
            }
            else
            {
                QPalette p=QPalette();
                p.setColor(QPalette::Base,Qt::red);
                p.setColor(QPalette::WindowText,Qt::red);
                p.setColor(QPalette::Background,QColor(255,0,0,255));
                ui->lineEdit_switch_2->setPalette(p);
                ui->lineEdit_knob_4->setPalette(p);
            }

        }
        else
        {
            if (switch_1 != 0 )
            {
                QPalette p=QPalette();
                p.setColor(QPalette::Base,Qt::yellow);
                p.setColor(QPalette::WindowText,Qt::yellow);
                p.setColor(QPalette::Background,QColor(255,255,0,255));
                ui->lineEdit_switch_2->setPalette(p);

                QPalette p1=QPalette();
                p1.setColor(QPalette::Base,Qt::yellow);
                p1.setColor(QPalette::WindowText,Qt::yellow);
                p1.setColor(QPalette::Background,QColor(255,255,0,255));
                ui->lineEdit_knob_3->setPalette(p1);
            }
            else
            {
                QPalette p=QPalette();
                p.setColor(QPalette::Base,Qt::red);
                p.setColor(QPalette::WindowText,Qt::red);
                p.setColor(QPalette::Background,QColor(255,0,0,255));
                ui->lineEdit_switch_2->setPalette(p);

                QPalette p1=QPalette();
                p1.setColor(QPalette::Base,Qt::red);
                p1.setColor(QPalette::WindowText,Qt::red);
                p1.setColor(QPalette::Background,QColor(255,0,0,255));
                ui->lineEdit_knob_3->setPalette(p1);
            }

        }
    ui->lineEdit_btn_1->setText(str_btn_1.setNum(btn_1));
        ui->lineEdit_btn_1->setStyleSheet("font-size:24px");
        if (btn_1 != 0 )
        {
            if (switch_1 != 0 )
            {
                QPalette p=QPalette();
                p.setColor(QPalette::Base,Qt::green);
                p.setColor(QPalette::WindowText,Qt::green);
                p.setColor(QPalette::Background,QColor(0,255,0,255));
                ui->lineEdit_btn_1->setPalette(p);
            }
            else
            {
                QPalette p=QPalette();
                p.setColor(QPalette::Base,Qt::red);
                p.setColor(QPalette::WindowText,Qt::red);
                p.setColor(QPalette::Background,QColor(255,0,0,255));
                ui->lineEdit_btn_1->setPalette(p);
            }

        }
        else
        {
            QPalette p=QPalette();
            p.setColor(QPalette::Base,Qt::red);
            p.setColor(QPalette::WindowText,Qt::red);
            p.setColor(QPalette::Background,QColor(255,0,0,255));
            ui->lineEdit_btn_1->setPalette(p);
        }
    ui->lineEdit_btn_2->setText(str_btn_2.setNum(btn_2));
        ui->lineEdit_btn_2->setStyleSheet("font-size:24px");
        if (btn_2 != 0 )
        {
            if (switch_1 != 0 )
            {
                QPalette p=QPalette();
                p.setColor(QPalette::Base,Qt::green);
                p.setColor(QPalette::WindowText,Qt::green);
                p.setColor(QPalette::Background,QColor(0,255,0,255));
                ui->lineEdit_btn_2->setPalette(p);
            }
            else
            {
                QPalette p=QPalette();
                p.setColor(QPalette::Base,Qt::red);
                p.setColor(QPalette::WindowText,Qt::red);
                p.setColor(QPalette::Background,QColor(255,0,0,255));
                ui->lineEdit_btn_2->setPalette(p);
            }
        }
        else
        {
            QPalette p=QPalette();
            p.setColor(QPalette::Base,Qt::red);
            p.setColor(QPalette::WindowText,Qt::red);
            p.setColor(QPalette::Background,QColor(255,0,0,255));
            ui->lineEdit_btn_2->setPalette(p);
        }

}
void MainWindow::qgc_server_recv_data(int uuv_state,float uuv_roll,float uuv_pitch,float uuv_yaw,float uuv_depth,float uuv_voltage)
{
    //ui->lineEdit_state->setText(str_state.setNum(uuv_state));
        ui->lineEdit_state->setStyleSheet("font-size:24px");
        if (uuv_state != 0 )
        {
            QPalette p=QPalette();
            p.setColor(QPalette::Base,Qt::green);
            p.setColor(QPalette::WindowText,Qt::green);
            p.setColor(QPalette::Background,QColor(0,255,0,255));
            ui->lineEdit_state->setPalette(p);
             //ui->lineEdit_state->setStyleSheet(QString(":focus{ background-color: green; }"));
        }
        else
        {
            QPalette p=QPalette();
            p.setColor(QPalette::Base,Qt::red);
            p.setColor(QPalette::WindowText,Qt::red);
            p.setColor(QPalette::Background,QColor(255,0,0,255));
            ui->lineEdit_state->setPalette(p);
            //ui->lineEdit_state->setStyleSheet(QString(":focus{ background-color: red; }"));
        }

    ui->lineEdit_roll->setText(str_roll.setNum(uuv_roll));
        ui->lineEdit_roll->setStyleSheet("font-size:24px");
    ui->lineEdit_pitch->setText(str_pitch.setNum(uuv_pitch));
        ui->lineEdit_pitch->setStyleSheet("font-size:24px");
    ui->lineEdit_yaw->setText(str_yaw.setNum(uuv_yaw));
        ui->lineEdit_yaw->setStyleSheet("font-size:24px");
    ui->lineEdit_depth->setText(str_depth.setNum(uuv_depth));
        ui->lineEdit_depth->setStyleSheet("font-size:24px");
    ui->lineEdit_voltage->setText(str_voltage.setNum(uuv_voltage));
        ui->lineEdit_voltage->setStyleSheet("font-size:24px");

}

void MainWindow::changeDuration(qint64 seconds)
{
    qDebug() << seconds;
}

void MainWindow::show_time()
{
    QDateTime time=QDateTime::currentDateTime();			    //(a)
   // QString Text=time.toString("yyyy年/MM月/dd日/ddd  hh:mm:ss");		//(b)
    QString Text=time.toString("yyyy/MM/dd/ hh:mm:ss");		//(b)

   // qDebug()<<Text;
    if(showColon)
    {
        //Text[21]=':';
        //Text[24]=':';
        showColon=false;
    }
    else
    {
       // Text[21]=' ';
        //Text[24]=' ';
        showColon=true;
    }
    ui->lineEdit_time->setText(Text);    
    ui->lineEdit_time->setStyleSheet("font-size:24px");

}
MainWindow::~MainWindow()
{
    delete ui;
    close();
}
