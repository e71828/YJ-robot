#include "secondvideo.h"
#include "ui_secondvideo.h"

secondvideo::secondvideo(QString source, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::secondvideo), nbSource2(source)
{
    ui->setupUi(this);

    setWindowTitle(" 第二视角 ");

    QVideoWidget *_vw2 = new QVideoWidget;
    QMediaPlayer *_player2 = new QMediaPlayer;
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(_vw2,0,0,1,1);
//    QWidget *win = new QWidget;
//    win->setLayout(layout);
    ui->widget->setLayout(layout);
    _player2->setVideoOutput(_vw2);
    const QUrl url2 = QUrl(nbSource2);
    const QNetworkRequest requestRtsp2(url2);
    _player2->setMedia(requestRtsp2);
//    QUrl("gst-pipeline: udpsrc port=5000 ! \
//    application/x-rtp,encoding-name=JPEG,payload=26 ! rtpjpegdepay ! jpegdec ! \
//    xvimagesink name=qtvideosink"));
//            ;setMedia(requestRtsp2);
//            ;setMedia(QUrl("gst-pipeline: videotestsrc ! autovideosink"));
    _player2->play();

}

secondvideo::~secondvideo()
{
    delete ui;
}
