#include "datadialog.h"
#include "ui_datadialog.h"

int rval2 = 1;

int dataDialog::returnval2() {
   return rval2;
}


dataDialog::dataDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dataDialog)
{
    ui->setupUi(this);
}

dataDialog::~dataDialog()
{
    delete ui;
}

void dataDialog::on_dataButton1_clicked()
{
    rval2 = 1;
}

void dataDialog::on_dataButton2_clicked()
{
    rval2 = 2;
}

void dataDialog::on_dataButton3_clicked()
{
    rval2 = 3;
}

void dataDialog::on_dataButton4_clicked()
{
    rval2 = 4;
}
