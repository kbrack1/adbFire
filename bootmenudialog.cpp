#include "bootmenudialog.h"
#include "ui_bootmenudialog.h"


bool bootmenuDialog::bagreeBox() {
   return ui->bagreeBox->isChecked();
}


bootmenuDialog::bootmenuDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::bootmenuDialog)
{
    ui->setupUi(this);
}

bootmenuDialog::~bootmenuDialog()
{
    delete ui;
}
