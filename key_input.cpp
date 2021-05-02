#include "key_input.h"


key_input::key_input(QObject *parent) :
    QObject(parent)
{

}

//按钮触发方式完成计算
void key_input::key_input_btn()
{
  bool ok;
  QString tempStr;
  QString valueStr=ui->radiuslineEdit_2->text();
  int valueInt=valueStr.toInt(&ok);
  double zhou=2*PI*valueInt;//计算，周长
  double area=valueInt*valueInt*PI;//计算，面积
  ui->areaLineEdit_4->setText(tempStr.setNum(zhou));
  ui->areaLineEdit_3->setText(tempStr.setNum(area));


}


key_input::~key_input()
{

}

