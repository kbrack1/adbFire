#include "kodidialog.h"
#include "ui_kodidialog.h"
#include <QDesktopServices>
#include <QUrl>

kodiDialog::kodiDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::kodiDialog)
{
    ui->setupUi(this);

    QPixmap pix2(":/assets/kodi.png");
    QIcon icon2(pix2);
    ui->kodi->setIcon(icon2);
    ui->kodi->setIconSize(pix2.size());


}

kodiDialog::~kodiDialog()
{
    delete ui;
}

void kodiDialog::on_kodi_clicked()
{
    QString link = "http://kodi.tv/download/";
    QDesktopServices::openUrl(QUrl(link));
}
