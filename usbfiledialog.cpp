#include "usbfiledialog.h"
#include "ui_usbfiledialog.h"
#include <QProcess>
#include <QFile>
#include <QString>
#include <QMessageBox>
#include <QStringList>
#include <QTextStream>

#ifdef Q_OS_LINUX
 int ost1=0;
#elif defined(Q_OS_WIN)
  int ost1=1;
#elif defined(Q_OS_MAC)
int ost1=2;
#endif



///////////////////////////////////////////////////
QString usbfileDialog::binfileName() {

    if( ui->usblistWidget->selectedItems().count() == 1 )
         return ui->usblistWidget->currentItem()->text();
      else return "";
}



///////////////////////////////////////////////////////
void usbfileDialog::setData(const QString &labelText) {
  ui->title->setText(labelText);
}


///////////////////////////////////////////////////////
usbfileDialog::usbfileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::usbfileDialog)
{

    ui->setupUi(this);




  if (ost1 == 1)
     {
       tmpdir1 = "./";
       adb21 = tmpdir1+"adb.exe";

     }


   if (ost1 == 2)
     {
       tmpdir1 = QCoreApplication::applicationDirPath();
       tmpdir1 = tmpdir1+"/adbfiles/";
      adb21 = tmpdir1+"adb";
     }


 tmpstr1 = tmpdir1+"temp.txt";


    QFile file31(tmpstr1);
      if (!file31.open(QIODevice::ReadOnly | QIODevice::Text))
        {QMessageBox::critical(this,"","Error reading file!");
           return; }

      QTextStream in1(&file31);
       while (!in1.atEnd())
        {


         fline1 = in1.readLine();

           if (!fline1.isEmpty())
            {
              ui->usblistWidget->addItem(fline1);
          }

       }

         file31.close();

      QFile::remove(tmpstr1);


}

usbfileDialog::~usbfileDialog()
{
    delete ui;
}

