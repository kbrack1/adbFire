#include "uninstalldialog.h"
#include "ui_uninstalldialog.h"

QString uninstallDialog::packageName() {
   return ui->packagename->text();
}

bool uninstallDialog::keepBox() {
   return ui->keepBox->isChecked();
}

uninstallDialog::uninstallDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::uninstallDialog)
{
    ui->setupUi(this);



}

uninstallDialog::~uninstallDialog()
{
    delete ui;
}


void uninstallDialog::on_pushButton_clicked()
{

}
