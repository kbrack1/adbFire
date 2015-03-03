#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include <QFileDialog>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtNetwork/QNetworkInterface>
#include <QMessageBox>
#include <QElapsedTimer>
#include <QDesktopServices>

QString pdir = "";
QString version2;
int rval1 = 0;

////////////////////////////////////////////////
 QString strip2 (QString str)
{
    str = str.simplified();
    str.replace( " ", "" );
    return str;
}
//////////////////////////////////
bool isConnectedToNetwork2()
{
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    bool result = false;

    for (int i = 0; i < ifaces.count(); i++)
    {
        QNetworkInterface iface = ifaces.at(i);
        if ( iface.flags().testFlag(QNetworkInterface::IsUp)
             && !iface.flags().testFlag(QNetworkInterface::IsLoopBack) )
        {


            // this loop is important
            for (int j=0; j<iface.addressEntries().count(); j++)
            {

                // we have an interface that is up, and has an ip address
                // therefore the link is present

                // we will only enable this check on first positive,
                // all later results are incorrect

                if (result == false)
                    result = true;
            }
        }

    }

    return result;
}



int preferencesDialog::buffermode() {
   return ui->buffermode->currentIndex();
}

QString preferencesDialog::bufferfactor() {
   return ui->bufferfactor->text();
}

QString preferencesDialog::buffersize() {
   return ui->buffersize->text();
}

QString preferencesDialog::xbmcpackageName() {
   return ui->packagename->text();
}

QString preferencesDialog::sshpassword() {
   return ui->sshpassword->text();
}

QString preferencesDialog::pulldir() {
   return ui->pulldir->text();
}

QString preferencesDialog::daddr() {
   return ui->daddr->text();
}

QString preferencesDialog::description() {
   return ui->description->text();
}

QString preferencesDialog::filepath() {
   return ui->filepath->text();
}
bool preferencesDialog::updatecheck() {
   return ui->ftvupdatesBox->isChecked();
}

bool preferencesDialog::sshcheck() {
   return ui->sshbox->isChecked();
}

bool preferencesDialog::mountcheck() {
   return ui->mountbox->isChecked();
}


bool preferencesDialog::versioncheck() {
   return ui->versioncheck->isChecked();
}


void preferencesDialog::setbuffersize(const QString &buffersize)
{
    ui->buffersize->setText(buffersize);
}

void preferencesDialog::setbuffermode(const int &buffermode)
{

    ui->buffermode->setCurrentIndex(buffermode);
}

void preferencesDialog::setbufferfactor(const QString &bufferfactor)
{
    ui->bufferfactor->setText(bufferfactor);
}

void preferencesDialog::setPackagename(const QString &packagename)
{
    ui->packagename->setText(packagename);
}

void preferencesDialog::setSSHpassword(const QString &sshpassword)
{
    ui->sshpassword->setText(sshpassword);
}

void preferencesDialog::setPulldir(const QString &pulldir)
{
    ui->pulldir->setText(pulldir);
}

void preferencesDialog::setftvUpdate(const bool &updatecheck)
{
    ui->ftvupdatesBox->setChecked(updatecheck);
}


void preferencesDialog::setsshBoot(const bool &sshcheck)
{
    ui->sshbox->setChecked(sshcheck);
}

void preferencesDialog::setmountBoot(const bool &mountcheck)
{
    ui->mountbox->setChecked(mountcheck);
}

void preferencesDialog::setversioncheck(const bool &versioncheck)
{
    ui->versioncheck->setChecked(versioncheck);
}


void preferencesDialog::setversionLabel(const QString &versiontext)
{
    ui->versionLabel->setText("adbFire version: "+ versiontext);
    version2 = versiontext;
}

void preferencesDialog::setdaddr(const QString &daddr)
{
    ui->daddr->setText(daddr);

}

void preferencesDialog::setdescription(const QString &description)
{
    ui->description->setText(description);

}

void preferencesDialog::setfilepath(const QString &filepath)
{
    ui->filepath->setText(filepath);

}
void preferencesDialog::setrecnum(const QString &recnum)
{
    ui->recnum->setText(recnum);

    if (recnum=="0")
       { ui->versioncheck->setVisible(true);
        ui->getUpdate->setVisible(true);
        }
     else
        { ui->versioncheck->setVisible(false);
         ui->getUpdate->setVisible(false);
        }

}


int preferencesDialog::returnval1() {
   return rval1;
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


void preferencesDialog::on_getUpdate_clicked() {
   QNetworkRequest request;
   request.setUrl(QUrl("http://www.jocala.com/version.txt"));

   QNetworkAccessManager *m_networkManager = new QNetworkAccessManager();
   QNetworkReply *reply = m_networkManager->get(request);

   connect(reply, SIGNAL(finished()),
           this, SLOT(onRequestCompleted()));



}

void preferencesDialog::onRequestCompleted() {
   QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());


   if (reply->error() != QNetworkReply::NoError)
   {

       int err = reply->error();
       QString s2 = QString::number(err);
       QMessageBox::critical(0, "","Network error: " + s2,QMessageBox::Cancel);
       return;
   }



   QByteArray data = reply->readAll();

   QString s1(data);

       s1 = strip2(s1);

       int err = reply->error();

       QString s2 = QString::number(err);


       if (version2 != s1)
       {
           QMessageBox::StandardButton rep;
            rep = QMessageBox::question(0, "", "Version "+s1+" is ready. Download?",
                    QMessageBox::Yes|QMessageBox::No);
              if (rep == QMessageBox::Yes)
               {
                  QString link = "http://www.jocala.com/adbfire.html";
                  QDesktopServices::openUrl(QUrl(link));

                }

       }

       else
           QMessageBox::information(0, "","No adbFire update available",QMessageBox::Cancel);

       delete reply;

}

void preferencesDialog::on_writexml_clicked()
{
   rval1 = 2;
}

void preferencesDialog::on_resetxml_clicked()
{
     ui->buffersize->setText("20971520");
     ui->buffermode->setCurrentIndex(2);
     ui->bufferfactor->setText("1");

     ui->preset1->setAutoExclusive(false);
     ui->preset2->setAutoExclusive(false);
     ui->preset3->setAutoExclusive(false);

     ui->preset1->setChecked(false);
     ui->preset2->setChecked(false);
     ui->preset3->setChecked(false);

     ui->preset1->setAutoExclusive(true);
     ui->preset2->setAutoExclusive(true);
     ui->preset3->setAutoExclusive(true);

}

void preferencesDialog::on_pushButton_clicked()
{
    rval1 = 1;
}

void preferencesDialog::on_preset1_clicked()
{

    setbuffermode(1);
    setbuffersize("0");
    setbufferfactor("10");

}

void preferencesDialog::on_preset2_clicked()
{
    setbuffermode(2);
    setbuffersize("104857600");
    setbufferfactor("1");
}

void preferencesDialog::on_preset3_clicked()
{
    setbuffermode(2);
    setbuffersize("52428800");
    setbufferfactor("1");



}

void preferencesDialog::on_kodiButton_clicked()
{
    ui->packagename->setText("org.xbmc.kodi");
    ui->filepath->setText("/files/.kodi");
}

void preferencesDialog::on_xbmcButton_clicked()
{
    ui->packagename->setText("org.xbmc.xbmc");
    ui->filepath->setText("/files/.xbmc");
}

void preferencesDialog::on_spmcButton_clicked()
{
    ui->packagename->setText("com.semperpax.spmc");
    ui->filepath->setText("/files/.spmc");
}

void preferencesDialog::on_otherButton_clicked()
{
    ui->packagename->setText("");
    ui->filepath->setText("");
}
