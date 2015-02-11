#include "videodialog.h"
#include "ui_videodialog.h"

videoDialog::videoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::videoDialog)
{
    ui->setupUi(this);
}

videoDialog::~videoDialog()
{
    delete ui;
}
