#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qgc_udp_server.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(" 油浸式变压器检测机器人控制系统 ");

    ui->PortBox->installEventFilter(this);
    find_serial();

    //点击串口[开启]/[关闭]按钮
    connect(ui->OpenSerialButton,&QPushButton::clicked,this,[this]()
    {

        if(ui->OpenSerialButton->text()=="打开串口")
        {
            port = ui->PortBox->currentText();
            open_serial();
            //关闭设置菜单使能
            ui->OpenSerialButton->setText(tr("关闭串口"));
            ui->spinBox->setValue(0);
        }
        else
        {
            close_serial();
            ui->OpenSerialButton->setText(tr("打开串口"));
        }
    });


    //qgc[开启]/[关闭]按钮
    host = find_IP();
    ui->lineEdit_qgc_server_port->setText(QString::number(qgc_server_port));
    ui->lineEdit_qgc_server_port->setAlignment(Qt::AlignCenter);
    connect(ui->OpenButton,&QPushButton::clicked,this,[this]()
    {

        if(ui->OpenButton->text()=="打开端口")
        {
            qgc_message = new qgc_udp_server(host);
            connect(qgc_message,&qgc_udp_server::qgc_server_recv_message,this,&MainWindow::update_data);
            updateData =1;

            QTimer *timer0 =new QTimer(this);			//新建一个定时器对象
            connect(timer0,&QTimer::timeout,qgc_message,&qgc_udp_server::qgc_server_send_data);
            timer0->start(20);

            //关闭设置菜单使能
            ui->OpenButton->setText(tr("关闭端口"));
        }
        else
        {
            ui->OpenButton->setText(tr("打开端口"));
            qgc_message->disconnect();
            qgc_message->deleteLater();

            updateData=0;

        }
    });

    //显示时间
      QTimer *timer=new QTimer(this);			//新建一个定时器对象
      connect(timer,SIGNAL(timeout()),this,SLOT(show_time()));	//(d)
      timer->start(100);						//(e)
      show_time();								//初始时间显示
      showColon=true;                            //初始化

    //显示视频
      show_video();

    //置零
      //update_data(0, 0.0, 0.0, 0.0, 0.0, 0.0);

    //串口数据矫正
    //depth_init=true;

      connect(ui->verticalSlider,SIGNAL(valueChanged(int)),ui->spinBox,SLOT(setValue(int)));
      connect(ui->spinBox,SIGNAL(valueChanged(int)),ui->verticalSlider,SLOT(setValue(int)));
}

void sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}


bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        if(obj == ui->PortBox)
        {
            //处理鼠标点击事件
            QComboBox * combobox = qobject_cast<QComboBox *>(obj);

            combobox->clear();
            find_serial();
        }
    }

    return QWidget::eventFilter(obj, event);
}

QString MainWindow::find_IP()
{
    QString localhostname =  QHostInfo::localHostName();
    QString localhostIP;
    quint32 rangeMin = QHostAddress("192.168.31.0").toIPv4Address();
    quint32 rangeMax = QHostAddress("192.168.31.255").toIPv4Address();
    QList<QHostAddress> hostList = QHostInfo::fromName(localhostname).addresses();
    foreach (const QHostAddress& address, hostList) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address.isLoopback() == false) {
             if(address.toIPv4Address() >= rangeMin && address.toIPv4Address()<= rangeMax){
                 localhostIP = address.toString();
    //                 qDebug()<< "localhostIP" << localhostIP;
             }
        }
    }
    return localhostIP;
}

void MainWindow::find_serial()
{
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
    //ui->OpenSerialButton->setText(tr("关闭串口"));

    //连接信号槽
    QObject::connect(serial,&QSerialPort::readyRead,this,&MainWindow::serial_read_data);

}

//读取接收到的信息
void MainWindow::serial_read_data()
{
    char data_first[16];char data_last[16];
    QByteArray read_buf;

    read_buf = serial->readAll();

    qDebug()<<  read_buf;

    if(read_buf.back() == '\n')
    {
        joy_data.append(read_buf);
        qDebug()<< "read data:=============================================";
        qDebug()<<  joy_data;

        sscanf(joy_data, "%[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s\r\n",data_first,&joy_1,&joy_2,&rotate_key,
               &knob_1,&knob_2,&knob_3,&knob_4,&rotate_cam_1,&rotate_cam_2,&switch_key,&switch_1,&switch_2,&btn_1,&btn_2,data_last);
        remote_x = joy_1;remote_y = joy_2;remote_key=rotate_key/10;remote_knob_1 = knob_1;remote_knob_2 = knob_2;remote_knob_3 = knob_3;remote_knob_4 = knob_4;
        remote_cam_1 = rotate_cam_1;remote_cam_2 = rotate_cam_2;remote_switch_key = switch_key;remote_switch_1 = switch_1;remote_switch_2 = switch_2;remote_btn_1 = btn_1;remote_btn_2 = btn_2;

//        if(depth_init){
//            temp_depth_key0 = rotate_key;
//            temp_depth_key1 = rotate_key;
//            temp_depth_key2 = rotate_key;
//            remote_key = 0;
//            depth_init = false;
//        }

//        if(!rotate_key){
//            temp_depth_key0 = 0;
//            temp_depth_key1 = 0;
//            temp_depth_key2 = 0;
//            remote_key = 0;
//        }

//        if(temp_depth_key2 == temp_depth_key1 && temp_depth_key1 == temp_depth_key0){
//            if(rotate_key > temp_depth_key0) remote_key++;
//            if(rotate_key < temp_depth_key0) remote_key--;
//        }

        int m =10;
        if(knob_3 < m && knob_3 > -m){
            remote_knob_3 = 0;
        }else{
            knob_3 < 0 ? remote_knob_3 = knob_3 + m : remote_knob_3 = knob_3 - m;
        }
        if(knob_4 < m && knob_4 > -m){
            remote_knob_4 = 0;
        }else{
            knob_4 < 0 ? remote_knob_4 = -36 : remote_knob_4 = 36;
        }

//        temp_depth_key2 = temp_depth_key1;
//        temp_depth_key1 = temp_depth_key0;
//        temp_depth_key0 = rotate_key;


        remote_key =ui->spinBox->value();
        qDebug()<<  data_last;
        serial_message(joy_1,joy_2,remote_key,knob_1,knob_2,knob_3,knob_4,rotate_cam_1,rotate_cam_2,switch_key,switch_1,switch_2,btn_1,btn_2);

        joy_data.clear();
    } else{

        joy_data.append(read_buf);

    }
}

void MainWindow::close_serial()
{
    //关闭串口
    serial->clear();
    serial->close();
    serial->deleteLater();
    remote_knob_1=remote_knob_2=remote_x=remote_y=remote_knob_3=remote_knob_4=0;
    ui->lineEdit_joy_1->clear();
    ui->lineEdit_joy_2->clear();
//    ui->lineEdit_rotate_key->clear();
    ui->expected_depth->clear();
    ui->spinBox->setValue(0);
    ui->lineEdit_knob_1->clear();
    ui->lineEdit_knob_2->clear();
    ui->lineEdit_knob_3->clear();
    ui->lineEdit_knob_4->clear();
    ui->lineEdit_rotate_cam_1->clear();
    ui->lineEdit_rotate_cam_2->clear();
    ui->lineEdit_switch_key->clear();
    ui->lineEdit_switch_1->clear();
    ui->lineEdit_switch_2->clear();
    ui->lineEdit_btn_1->clear();
    ui->lineEdit_btn_2->clear();

}

void MainWindow::serial_message(int joy_1,int joy_2,int rotate_key,int knob_1,int knob_2,int knob_3,int knob_4,int rotate_cam_1,int rotate_cam_2,int switch_key,int switch_1,int switch_2,int btn_1,int btn_2)
{
    QFont FontSerialData("NanumGpthic-Bold", 20, QFont::Normal);


    ui->lineEdit_joy_1->setText(str_joy_1.setNum(joy_1));
        ui->lineEdit_joy_1->setFont(FontSerialData);
        ui->lineEdit_joy_1->setAlignment(Qt::AlignCenter);
    ui->lineEdit_joy_2->setText(str_joy_2.setNum(joy_2));
        ui->lineEdit_joy_2->setFont(FontSerialData);
        ui->lineEdit_joy_2->setAlignment(Qt::AlignCenter);
//    ui->lineEdit_rotate_key->setText(str_rotate_key.setNum(rotate_key));
//        ui->lineEdit_rotate_key->setFont(FontSerialData);
//        ui->lineEdit_rotate_key->setAlignment(Qt::AlignCenter);
    ui->lineEdit_knob_1->setText(str_knob_1.setNum(knob_1));
        ui->lineEdit_knob_1->setFont(FontSerialData);
        ui->lineEdit_knob_1->setAlignment(Qt::AlignCenter);
    ui->lineEdit_knob_2->setText(str_knob_2.setNum(knob_2));
         ui->lineEdit_knob_2->setFont(FontSerialData);
         ui->lineEdit_knob_2->setAlignment(Qt::AlignCenter);
    ui->lineEdit_knob_3->setText(str_knob_3.setNum(knob_3));
        ui->lineEdit_knob_3->setFont(FontSerialData);
        ui->lineEdit_knob_3->setAlignment(Qt::AlignCenter);
    ui->lineEdit_knob_4->setText(str_knob_4.setNum(knob_4));
        ui->lineEdit_knob_4->setFont(FontSerialData);
        ui->lineEdit_knob_4->setAlignment(Qt::AlignCenter);


    ui->lineEdit_rotate_cam_1->setText(str_rotate_cam_1.setNum(rotate_cam_1));
        ui->lineEdit_rotate_cam_1->setFont(FontSerialData);
        ui->lineEdit_rotate_cam_1->setAlignment(Qt::AlignCenter);
    ui->lineEdit_rotate_cam_2->setText(str_rotate_cam_2.setNum(rotate_cam_2));
         ui->lineEdit_rotate_cam_2->setFont(FontSerialData);
         ui->lineEdit_rotate_cam_2->setAlignment(Qt::AlignCenter);
    ui->lineEdit_switch_key->setText(str_switch_key.setNum(switch_key));
         ui->lineEdit_switch_key->setFont(FontSerialData);
         ui->lineEdit_switch_key->setAlignment(Qt::AlignCenter);
    //ui->lineEdit_switch_1->setText(str_switch_1.setNum(switch_1));
        ui->lineEdit_switch_1->setFont(FontSerialData);
        ui->lineEdit_switch_1->setAlignment(Qt::AlignCenter);
    ui->expected_depth->setText(str_Height.setNum(rotate_cam_2 + rotate_key*100) + " mm");
        QFont serifFont("Times", 16, QFont::Normal);
        ui->expected_depth->setFont(serifFont);
        //ui->expected_depth->setAlignment(Qt::AlignCenter);

        QPalette org=QPalette();
        org.setColor(QPalette::Base,Qt::white);
        org.setColor(QPalette::WindowText,Qt::white);
        org.setColor(QPalette::Background,QColor(255,255,255,255));

        QPalette pass=QPalette();
        pass.setColor(QPalette::Base,Qt::green);
        pass.setColor(QPalette::WindowText,Qt::green);
        pass.setColor(QPalette::Background,QColor(0,255,0,255));

        QPalette error=QPalette();
        error.setColor(QPalette::Base,Qt::red);
        error.setColor(QPalette::WindowText,Qt::red);
        error.setColor(QPalette::Background,QColor(255,0,0,255));

        QPalette war=QPalette();
        war.setColor(QPalette::Base,Qt::yellow);
        war.setColor(QPalette::WindowText,Qt::yellow);
        war.setColor(QPalette::Background,QColor(255,255,0,255));

        if (switch_1 != 0 )
        {
            ui->lineEdit_switch_1->setPalette(pass);
            ui->lineEdit_joy_1->setPalette(org);
            ui->lineEdit_joy_2->setPalette(org);
//            ui->lineEdit_rotate_key->setPalette(org);
            ui->lineEdit_knob_1->setPalette(org);
            ui->lineEdit_knob_2->setPalette(org);
            ui->lineEdit_rotate_cam_1->setPalette(org);
            ui->lineEdit_rotate_cam_2->setPalette(org);
            ui->lineEdit_switch_key->setPalette(org);
        }
        else
        {
            ui->lineEdit_joy_1->setPalette(error);
            ui->lineEdit_joy_2->setPalette(error);
//            ui->lineEdit_rotate_key->setPalette(error);
            ui->lineEdit_knob_1->setPalette(error);
            ui->lineEdit_knob_2->setPalette(error);
            ui->lineEdit_knob_3->setPalette(error);
            ui->lineEdit_knob_4->setPalette(error);
            ui->lineEdit_rotate_cam_1->setPalette(error);
            ui->lineEdit_rotate_cam_2->setPalette(error);
            ui->lineEdit_switch_key->setPalette(error);
            ui->lineEdit_switch_1->setPalette(error);
            ui->lineEdit_switch_2->setPalette(error);
            ui->lineEdit_btn_1->setPalette(error);
            ui->lineEdit_btn_2->setPalette(error);
        }

        ui->lineEdit_switch_2->setText(str_switch_2.setNum(switch_2));
        ui->lineEdit_switch_2->setFont(FontSerialData);
        ui->lineEdit_switch_2->setAlignment(Qt::AlignCenter);
        if (switch_2 != 0 )
        {
            if (switch_1 != 0 )
            {
                ui->lineEdit_switch_2->setPalette(pass);
                ui->lineEdit_knob_3->setPalette(error);
                ui->lineEdit_knob_4->setPalette(pass);
            }
            else
            {
                ui->lineEdit_switch_2->setPalette(error);
                ui->lineEdit_knob_3->setPalette(error);
                ui->lineEdit_knob_4->setPalette(error);
            }

        }
        else
        {
            if (switch_1 != 0 )
            {
                ui->lineEdit_switch_2->setPalette(war);
                ui->lineEdit_knob_3->setPalette(war);
                ui->lineEdit_knob_4->setPalette(error);
            }
            else
            {
                ui->lineEdit_switch_2->setPalette(error);
                ui->lineEdit_knob_3->setPalette(error);
                ui->lineEdit_knob_4->setPalette(error);
            }

        }
    ui->lineEdit_btn_1->setText(str_btn_1.setNum(btn_1));
        ui->lineEdit_btn_1->setFont(FontSerialData);
        ui->lineEdit_btn_1->setAlignment(Qt::AlignCenter);
        if (btn_1 != 0 )
        {
            if (switch_1 != 0 )
            {
                ui->lineEdit_btn_1->setPalette(pass);
            }
            else
            {
                ui->lineEdit_btn_1->setPalette(error);
            }

        }
        else
        {
            ui->lineEdit_btn_1->setPalette(error);
        }
    ui->lineEdit_btn_2->setText(str_btn_2.setNum(btn_2));
        ui->lineEdit_btn_2->setFont(FontSerialData);
        ui->lineEdit_btn_2->setAlignment(Qt::AlignCenter);
        if (btn_2 != 0 )
        {
            if (switch_1 != 0 )
            {
                ui->lineEdit_btn_2->setPalette(pass);
            }
            else
            {
                ui->lineEdit_btn_2->setPalette(error);

            }
        }
        else
        {
            ui->lineEdit_btn_2->setPalette(error);
        }

}
void MainWindow::update_data(int uuv_state,float uuv_roll,float uuv_pitch,float uuv_yaw,float uuv_depth,float uuv_voltage)
{
    //ui->lineEdit_state->setText(str_state.setNum(uuv_state));

        QFont FontUdpData("opensans", 16, QFont::Normal);

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

    ui->lineEdit_roll->setText(str_roll.setNum(uuv_roll,'f',3));
        ui->lineEdit_roll->setFont(FontUdpData);
        ui->lineEdit_roll->setAlignment(Qt::AlignCenter);
    ui->lineEdit_pitch->setText(str_pitch.setNum(uuv_pitch,'f',3));
        ui->lineEdit_pitch->setFont(FontUdpData);
        ui->lineEdit_pitch->setAlignment(Qt::AlignCenter);
    ui->lineEdit_yaw->setText(str_yaw.setNum(uuv_yaw,'f',3));
        ui->lineEdit_yaw->setFont(FontUdpData);
        ui->lineEdit_yaw->setAlignment(Qt::AlignCenter);
//        qDebug()<<uuv_yaw;
//        if (uuv_depth < 1e-2 || uuv_depth > 1e9) uuv_depth =0;
    ui->lineEdit_depth->setText(str_depth.setNum(uuv_depth,'f',2));
        ui->lineEdit_depth->setFont(FontUdpData);
        ui->lineEdit_depth->setAlignment(Qt::AlignCenter);
}

void MainWindow::changeDuration(qint64 seconds)
{
    qDebug() << seconds;
}

void MainWindow::show_time()
{
    QDateTime time=QDateTime::currentDateTime();			    //(a)
//    QString Text=time.toString("yyyy年/MM月/dd日/ddd  hh:mm:ss");		//(b)
    QString Text=time.toString("MM/dd/yyyy hh:mm:ss ");		//(b)
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
    Text.prepend(QString::number(qgc_server_port) + "  Time: ");
    Text.prepend(host + ":");
    Text.prepend("Local UDP Address: ");
    ui->label_status->setText(Text);
    QFont NanumGothic_R("NanumGothic-Regular", 10, QFont::Normal);
    ui->statusBar->addPermanentWidget( ui->label_status,0);
    ui->label_status->setFont(NanumGothic_R);

}

void MainWindow::show_video()
{
    // Criando 4 QVideoWidget
    QVideoWidget *_vw1 = new QVideoWidget;
//    QVideoWidget *_vw2 = new QVideoWidget;
    // Criando 4 QMediaPlayer
    QMediaPlayer *_player1 = new QMediaPlayer;
//    QMediaPlayer *_player2 = new QMediaPlayer;
    // Criando um QGridLayout de 2x1
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(_vw1,0,0,1,1);
//    layout->addWidget(_vw2,0,1,1,1);
//    QWidget *win = new QWidget();
//    win->setLayout(layout);
    ui->widget_video->setLayout(layout);
    // QMediaPlayer -> QVideoWidget
    _player1->setVideoOutput(_vw1);
//    _player2->setVideoOutput(_vw2);
    // Links de RTSP e Videos
    nbSource1 =dest_server_ip;
    nbSource1.append(":" + QString::number(qgc_video1_port));
    nbSource1.prepend("rtsp://");
    nbSource1.append("/unicast");

    nbSource2 =dest_server_ip;
    nbSource2.append(":" + QString::number(qgc_video2_port));
    nbSource2.prepend("rtsp://");
    nbSource2.append("/unicast");

    const QUrl url1 = QUrl(nbSource1);
    qDebug()<<nbSource1;
      const QNetworkRequest requestRtsp1(url1);
//      const QNetworkRequest requestRtsp2(url2);
      _player1->setMedia(requestRtsp1);
//    _player2->setMedia(requestRtsp2);
      _player1->play();
//    _player2->play();
}
MainWindow::~MainWindow()
{
    delete qgc_message;
    delete ui;
    close();
}

void MainWindow::on_videoButton_released()
{
    show_video();
}

void MainWindow::on_secondVideo_clicked()
{
//    secondvideo secondDialog;
//    secondDialog.setModal(true);
//    secondDialog.exec();
    secondVideo = new secondvideo(nbSource2,this);
    secondVideo->show();
}
