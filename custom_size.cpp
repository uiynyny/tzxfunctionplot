#include "custom_size.h"
#include "ui_custom_size.h"

#include <QMessageBox>

custom_size::custom_size(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::custom_size)
{
    ui->setupUi(this);
}

custom_size::~custom_size()
{
    delete ui;
}

void custom_size::on_buttonBox_accepted()
{
    if(ui->max->text().isEmpty()||ui->min->text().isEmpty()){
        QMessageBox::information(this,"error","max or min is not filled");
        min=max=-1;
    }
    max=ui->max->text().toDouble();
    min=ui->min->text().toDouble();
}

double custom_size::getmax(){
    return max;
}

double custom_size::getmin(){
    return min;
}
