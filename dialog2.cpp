#include "dialog2.h"
#include "ui_dialog2.h"
#include <QDesktopServices>
#include <QUrl>


void Dialog2::setvLabel(const QString &vtext)
{
    ui->vlabel->setText(vtext);
}

Dialog2::Dialog2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog2)
{
    ui->setupUi(this);

    QPixmap pix(":/assets/donate.png");
    QIcon icon(pix);
    ui->donate->setIcon(icon);
    ui->donate->setIconSize(pix.size());


}

Dialog2::~Dialog2()
{
    delete ui;
}

void Dialog2::on_pushButton_clicked()
{

}

void Dialog2::on_donate_clicked()
{
   QString link = "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=JA5E5UP3ZSWBN";
   QDesktopServices::openUrl(QUrl(link));


}
