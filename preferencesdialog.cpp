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





QString preferencesDialog::xbmcpackageName() {
   return ui->packagename->text();
}

QString preferencesDialog::pulldir() {
   return ui->pulldir->text();
}

bool preferencesDialog::updatecheck() {
   return ui->ftvupdatesBox->isChecked();
}

bool preferencesDialog::versioncheck() {
   return ui->versioncheck->isChecked();
}




void preferencesDialog::setPackagename(const QString &packagename)
{
    ui->packagename->setText(packagename);
}

void preferencesDialog::setPulldir(const QString &pulldir)
{
    ui->pulldir->setText(pulldir);
}

void preferencesDialog::setftvUpdate(const bool &updatecheck)
{
    ui->ftvupdatesBox->setChecked(updatecheck);
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
           QMessageBox::information(0, "","No update available",QMessageBox::Cancel);

       delete reply;

}
