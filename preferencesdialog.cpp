#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include <QFileDialog>

QString pdir = "";

QString preferencesDialog::xbmcpackageName() {
   return ui->packagename->text();
}

QString preferencesDialog::pulldir() {
   return ui->pulldir->text();
}

void preferencesDialog::setPackagename(const QString &packagename)
{
    ui->packagename->setText(packagename);
}

void preferencesDialog::setPulldir(const QString &pulldir)
{
    ui->pulldir->setText(pulldir);
}


preferencesDialog::preferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::preferencesDialog)
{
    ui->setupUi(this);



}

preferencesDialog::~preferencesDialog()
{
    delete ui;
}


void preferencesDialog::on_pfolderButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose pull folder"),
                                                 ui->pulldir->text(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty() )
    {
        ui->pulldir->setText(dir);
    }
}
