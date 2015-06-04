#include "mediacenterdialog.h"
#include "ui_mediacenterdialog.h"



QString mediacenterDialog::xbmcpackageName() {
   return ui->packagename->text();
}

QString mediacenterDialog::filepath() {
   return ui->filepath->text();
}



mediacenterDialog::mediacenterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mediacenterDialog)
{
    ui->setupUi(this);


    ui->packagename->setText("org.xbmc.kodi");
    ui->filepath->setText("/files/.kodi");


}

mediacenterDialog::~mediacenterDialog()
{
    delete ui;
}

void mediacenterDialog::on_kodiButton_clicked()
{
    ui->packagename->setText("org.xbmc.kodi");
       ui->filepath->setText("/files/.kodi");
}

void mediacenterDialog::on_xbmcButton_clicked()
{
    ui->packagename->setText("org.xbmc.xbmc");
        ui->filepath->setText("/files/.xbmc");
}

void mediacenterDialog::on_spmcButton_clicked()
{
    ui->packagename->setText("com.semperpax.spmc");
        ui->filepath->setText("/files/.spmc");
}

void mediacenterDialog::on_tvmcButton_clicked()
{
    ui->packagename->setText("ag.tvaddons.tvmc");
        ui->filepath->setText("/files/.tvmc");
}

void mediacenterDialog::on_otherButton_clicked()
{
    ui->packagename->setText("");
       ui->filepath->setText("");
}
