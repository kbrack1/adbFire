#include "unbootdialog.h"
#include "ui_unbootdialog.h"



bool unbootDialog::uagreeBox() {
   return ui->uagreeBox->isChecked();
}


unbootDialog::unbootDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::unbootDialog)
{
    ui->setupUi(this);
}

unbootDialog::~unbootDialog()
{
    delete ui;
}
