#include "llamadialog.h"
#include "ui_llamadialog.h"


bool llamaDialog::llamacheck() {
   return ui->llamacheckBox->isChecked();
}

bool llamaDialog::llamaradio1() {
   return ui->llamaradio1->isChecked();
}

bool llamaDialog::llamaradio2() {
   return ui->llamaradio2->isChecked();
}


bool llamaDialog::llamaradio3() {
   return ui->llamaradio3->isChecked();
}


bool llamaDialog::ctvradio1() {
   return ui->ctvradio1->isChecked();
}

bool llamaDialog::ctvradio2() {
   return ui->ctvradio2->isChecked();
}


bool llamaDialog::ctvradio3() {
   return ui->ctvradio3->isChecked();
}




llamaDialog::llamaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::llamaDialog)
{
    ui->setupUi(this);
}

llamaDialog::~llamaDialog()
{
    delete ui;
}

