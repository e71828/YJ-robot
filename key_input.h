#ifndef KEY_INPUT_H
#define KEY_INPUT_H

#include <QObject>
#include "mainwindow.h"
#include <iostream>


class key_input : public QObject
{
    Q_OBJECT

public:
    explicit key_input(QObject *parent = nullptr);
    ~key_input();

    void key_input_btn();

private slots:

  //    void key_input_btn_clicked();


private:

};


#endif // KEY_INPUT_H
