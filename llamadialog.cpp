#include "llamadialog.h"
#include "ui_llamadialog.h"
// #include <QDesktopServices>
// #include <QUrl>

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

bool llamaDialog::llamaradio4() {
   return ui->llamaradio4->isChecked();
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


bool llamaDialog::ctvButton() {
   return ui->ctvButton->isChecked();
}

bool llamaDialog::kodiButton() {
   return ui->kodiButton->isChecked();
}

bool llamaDialog::xbmcButton() {
   return ui->xbmcButton->isChecked();
}

bool llamaDialog::spmcButton() {
   return ui->spmcButton->isChecked();
}

bool llamaDialog::tvmcButton() {
   return ui->tvmcButton->isChecked();
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

