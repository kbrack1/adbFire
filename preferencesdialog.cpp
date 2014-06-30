#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

QString preferencesDialog::xbmcpackageName() {
   return ui->packagename->text();
}

void preferencesDialog::setPackagename(const QString &packagename)
{
    ui->packagename->setText(packagename);
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

