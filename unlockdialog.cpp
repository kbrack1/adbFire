#include "unlockdialog.h"
#include "ui_unlockdialog.h"


bool unlockDialog::agreeBox() {
   return ui->agreeBox->isChecked();
}

unlockDialog::unlockDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::unlockDialog)
{
    ui->setupUi(this);
}

unlockDialog::~unlockDialog()
{
    delete ui;
}
