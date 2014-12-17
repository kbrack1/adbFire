#include "uuiddialog.h"
#include "ui_uuiddialog.h"
#include <QProcess>
#include <QFile>
#include <QString>
#include <QMessageBox>
#include <QStringList>
#include <QTextStream>

#ifdef Q_OS_LINUX
 int ost2=0;
#elif defined(Q_OS_WIN)
  int ost2=1;
#elif defined(Q_OS_MAC)
int ost2=2;
#endif



///////////////////////////////////////////////////
QString uuidDialog::uuidName() {

    if( ui->uuidlistWidget->selectedItems().count() == 1 )
         return ui->uuidlistWidget->currentItem()->text();
      else return "";
}



///////////////////////////////////////////////////////
void uuidDialog::setData1(const QString &labelText) {
  ui->title->setText(labelText);
}


///////////////////////////////////////////////////////
uuidDialog::uuidDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::uuidDialog)
{

    ui->setupUi(this);




  if (ost2 == 1)
     {
       tmpdir2 = "./";
       adb22 = tmpdir2+"adb.exe";

     }


  if (ost2 == 2 )
    {
       tmpdir2 = QCoreApplication::applicationDirPath();
       tmpdir2 = tmpdir2+"/adbfiles/";
      adb22 = tmpdir2+"adb.osx";
     }

  if (ost2 == 0 )
    {
       tmpdir2 = QCoreApplication::applicationDirPath();
       tmpdir2 = tmpdir2+"/adbfiles/";
      adb22 = tmpdir2+"adb.linux";
     }


 tmpstr2 = tmpdir2+"temp.txt";


    QFile file32(tmpstr2);
      if (!file32.open(QIODevice::ReadOnly | QIODevice::Text))
        {QMessageBox::critical(this,"","Error reading file!");
           return; }

      QTextStream in1(&file32);
       while (!in1.atEnd())
        {


         fline2 = in1.readLine();

           if (!fline2.isEmpty())
            {
              ui->uuidlistWidget->addItem(fline2);
          }

       }

         file32.close();

      QFile::remove(tmpstr2);


}

uuidDialog::~uuidDialog()
{
    delete ui;
}

