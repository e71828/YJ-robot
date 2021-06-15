#ifndef SECONDVIDEO_H
#define SECONDVIDEO_H

#include <QDialog>
#include <QObject>
#include <QDebug>
#include <QThread>
#include <QUrl>
#include <QString>
#include <QGridLayout>
#include <QMediaPlayer>
#include <QVideoWidget>

namespace Ui {
class secondvideo;
}

class secondvideo : public QDialog
{
    Q_OBJECT

public:
    explicit secondvideo(QString source, QWidget *parent = nullptr);
    ~secondvideo();

private:
    Ui::secondvideo *ui;

    QString nbSource2;
};

#endif // SECONDVIDEO_H
