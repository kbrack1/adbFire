#include "helpdialog.h"
#include "ui_helpdialog.h"


helpDialog::helpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::helpDialog)
{
    ui->setupUi(this);

    ui->textBrowser->setSource(*new QUrl("qrc:/assets/adbfire.html"));

    ui->textBrowser->setWindowTitle("adbFire Help");


}

helpDialog::~helpDialog()
{
    delete ui;
}
