#include "helpdialog.h"
#include "ui_helpdialog.h"
#include <QDesktopServices>
#include <QUrl>

helpDialog::helpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::helpDialog)
{
    ui->setupUi(this);

    ui->textBrowser->setSource(*new QUrl("qrc:/assets/adbfhelp.html"));

    ui->textBrowser->setWindowTitle("adbFire Help");


}

helpDialog::~helpDialog()
{
    delete ui;
}
