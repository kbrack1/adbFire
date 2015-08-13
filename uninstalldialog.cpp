#include "uninstalldialog.h"
#include "ui_uninstalldialog.h"
#include <QProcess>
#include <QFile>
#include <QString>
#include <QMessageBox>
#include <QStringList>
#include <QTextStream>

#ifdef Q_OS_LINUX
 int ost=0;
#elif defined(Q_OS_WIN)
  int ost=1;
#elif defined(Q_OS_MAC)
int ost=2;
#endif


QString tmpdir;
QString tmpstr;
QString commstr;
QString cstr;
QString argument;
QString adb2;
QString fline;




QString uninstallDialog::packageName() {

   if( ui->unlistWidget->selectedItems().count() == 1 )
        return ui->unlistWidget->currentItem()->text();
     else return "";

}

bool uninstallDialog::keepBox() {
   return ui->keepBox->isChecked();
}


void uninstallDialog::setdescription(const QString &description)
{
    ui->description->setText(description);
}



uninstallDialog::uninstallDialog(const QString &port, const QString &daddr,QWidget *parent) :
   QDialog(parent),m_port(port),m_daddr(daddr),
    ui(new Ui::uninstallDialog)




{



    if (ost == 1)
       {
        tmpdir = "./";
         adb2 = tmpdir+"adb.exe";
       }


     if (ost == 2)
       {
         tmpdir = QCoreApplication::applicationDirPath();
         tmpdir = tmpdir+"/adbfiles/";
        adb2 = tmpdir+"adb.osx";
       }

     if (ost == 0)
       {
         tmpdir = QCoreApplication::applicationDirPath();
         tmpdir = tmpdir+"/adbfiles/";
         adb2 = tmpdir+"adb.linux";
       }


    ui->setupUi(this);

tmpstr = tmpdir+"tempfl";
QString c1;
   QProcess packages;
   packages.setProcessChannelMode(QProcess::MergedChannels);

   if (m_port.isEmpty())
   argument = " -s "+m_daddr+ " shell pm list packages";
   else
   argument = " -s "+m_daddr+":"+m_port+" shell pm list packages";

    cstr = adb2 + argument;


   // QMessageBox::critical(0, "",cstr,QMessageBox::Cancel);

   packages.start(cstr);
   packages.waitForFinished(-1);
   commstr=packages.readAll();



                QFile file2(tmpstr);

                  if(!file2.open(QFile::WriteOnly |
                                QFile::Text))
                  {
                      QMessageBox::critical(this,"","Error creating file!");
                      return;
                  }


                  QTextStream out(&file2);
                  out  << commstr << endl;

                  file2.flush();
                  file2.close();



     QFile file3(tmpstr);
       if (!file3.open(QIODevice::ReadOnly | QIODevice::Text))
         {QMessageBox::critical(this,"","Error reading file!");
            return; }

       QTextStream in(&file3);
        while (!in.atEnd())
         {


          fline = in.readLine();

            if (!fline.isEmpty())
             {

            fline.remove(0,8);


                     ui->unlistWidget->addItem(fline);

           }

        }

          file3.close();

       QFile::remove(tmpstr);
}

uninstallDialog::~uninstallDialog()
{
    delete ui;
}

