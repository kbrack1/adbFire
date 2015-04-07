#include "cifsdialog.h"
#include "ui_cifsdialog.h"

int rval = 0;

QString cifsDialog::ipaddress1() {
   return ui->ipaddress1->text();
}

QString cifsDialog::share1() {
   return ui->share1->text();
}

QString cifsDialog::mount1() {
   return ui->mount1->text();
}

bool cifsDialog::nfs1() {
   return ui->nfs1->isChecked();
}


bool cifsDialog::select1() {
   return ui->select1->isChecked();
}

////////////////

QString cifsDialog::ipaddress2() {
   return ui->ipaddress2->text();
}

QString cifsDialog::share2() {
   return ui->share2->text();
}

QString cifsDialog::mount2() {
   return ui->mount2->text();
}


bool cifsDialog::nfs2() {
   return ui->nfs2->isChecked();
}

bool cifsDialog::select2() {
   return ui->select2->isChecked();
}

////////////////////


QString cifsDialog::ipaddress3() {
   return ui->ipaddress3->text();
}

QString cifsDialog::share3() {
   return ui->share3->text();
}

QString cifsDialog::mount3() {
   return ui->mount3->text();
}

bool cifsDialog::nfs3() {
   return ui->nfs3->isChecked();
}

bool cifsDialog::select3() {
   return ui->select3->isChecked();
}


///////////////////


QString cifsDialog::ipaddress4() {
   return ui->ipaddress4->text();
}

QString cifsDialog::share4() {
   return ui->share4->text();
}

QString cifsDialog::mount4() {
   return ui->mount4->text();
}


bool cifsDialog::nfs4() {
   return ui->nfs4->isChecked();
}

bool cifsDialog::select4() {
   return ui->select4->isChecked();
}

int cifsDialog::returnval() {
   return rval;
}

void cifsDialog::setipaddress1(const QString &ipaddress1)
{
ui->ipaddress1->setText(ipaddress1);
}
void cifsDialog::setshare1(const QString &share1)
{
ui->share1->setText(share1);
}
void cifsDialog::setmount1(const QString &mount1)
{
ui->mount1->setText(mount1);
}

void cifsDialog::setnfs1(const bool &nfs1)
{

    if (nfs1)
        ui->nfs1->setChecked(true);

}

///////////////////////


void cifsDialog::setipaddress2(const QString &ipaddress2)
{
ui->ipaddress2->setText(ipaddress2);
}
void cifsDialog::setshare2(const QString &share2)
{
ui->share2->setText(share2);
}
void cifsDialog::setmount2(const QString &mount2)
{
ui->mount2->setText(mount2);
}

void cifsDialog::setnfs2(const bool &nfs2)
{

    if (nfs2)
        ui->nfs2->setChecked(true);

}

///////////////////////

void cifsDialog::setipaddress3(const QString &ipaddress3)
{
ui->ipaddress3->setText(ipaddress3);
}
void cifsDialog::setshare3(const QString &share3)
{
ui->share3->setText(share3);
}
void cifsDialog::setmount3(const QString &mount3)
{
ui->mount3->setText(mount3);
}

void cifsDialog::setnfs3(const bool &nfs3)
{

    if (nfs3)
        ui->nfs3->setChecked(true);

}

////////////////////



void cifsDialog::setipaddress4(const QString &ipaddress4)
{
ui->ipaddress4->setText(ipaddress4);
}
void cifsDialog::setshare4(const QString &share4)
{
ui->share4->setText(share4);
}
void cifsDialog::setmount4(const QString &mount4)
{
ui->mount4->setText(mount4);
}

void cifsDialog::setnfs4(const bool &nfs4)
{

    if (nfs4)
        ui->nfs4->setChecked(true);

}





/////////////////////


cifsDialog::cifsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::cifsDialog)
{
    ui->setupUi(this);
}

cifsDialog::~cifsDialog()
{
    delete ui;
}

void cifsDialog::on_mountButton_clicked()
{
    rval=1;
}

void cifsDialog::on_umountButton_clicked()
{
    rval=2;
}

void cifsDialog::on_saveButton_clicked()
{
   rval=3;
}

void cifsDialog::on_scriptButton_clicked()
{
  rval=4;
}


void cifsDialog::on_unscriptButton_clicked()
{
    rval=5;
}


void cifsDialog::on_cancelButton_clicked()
{
    rval=99;
}
