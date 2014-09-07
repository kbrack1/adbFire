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

uninstallDialog::uninstallDialog(QWidget *parent) :
    QDialog(parent),
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
        adb2 = tmpdir+"adb";
       }


    ui->setupUi(this);

tmpstr = tmpdir+"tempfl";

   QProcess packages;
   packages.setProcessChannelMode(QProcess::MergedChannels);
   cstr = adb2 + " shell pm list packages";
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


            if (!fline.startsWith("com.amazon") &&
                !fline.startsWith("com.android") &&
                !fline.startsWith("org.chromium.content") &&
                !fline.startsWith("com.qualcomm") &&
                !fline.startsWith("android"))
                  {
                     ui->unlistWidget->addItem(fline);
                  }
           }

        }

          file3.close();

       QFile::remove(tmpstr);
}

uninstallDialog::~uninstallDialog()
{
    delete ui;
}

