#include "creditsdialog.h"
#include "ui_creditsdialog.h"

creditsDialog::creditsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::creditsDialog)
{
    ui->setupUi(this);
}

creditsDialog::~creditsDialog()
{
    delete ui;
}
