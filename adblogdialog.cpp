#include "adblogdialog.h"
#include "ui_adblogdialog.h"

#include <QMessageBox>

#include <QDir>
#include <QFile>
#include <QTextStream>
#include<QClipboard>




#ifdef Q_OS_LINUX
 int os1=0;
#elif defined(Q_OS_WIN)
  int os1=1;
#elif defined(Q_OS_MAC)
int os1=2;
#endif

QString logdir;
QString logfile1 = "adbfire.log";
QString logfile2 = "adbfire.old.log";
QString content;
bool getfile=true;

QClipboard *clipboard = QApplication::clipboard();

void getlog()

{

    QString fn = "";

    if (getfile)
       fn = logfile1;
    else
        fn=logfile2;

    QFile file(logdir+fn);

    if(!file.exists())
       {
       QMessageBox::critical(0, "","Can't find "+fn+"\n",QMessageBox::Cancel);
       getfile=true;
       return;
        }

    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    content = stream.readAll();
    file.close();


}



adblogDialog::adblogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::adblogDialog)
{
    ui->setupUi(this);


    if (os1 == 1)
       {
        logdir = "./";
       }

  if (os1 == 2)

       {
         logdir = QCoreApplication::applicationDirPath();
         logdir = logdir+"/adbfiles/";
       }

  if (os1 == 0)

       {
         logdir = QCoreApplication::applicationDirPath();
         logdir = logdir+"/adbfiles/";
       }

     getlog();
     ui->logfileName->setText("adbfire.log");
     ui->logBrowser->setPlainText(content);


}

adblogDialog::~adblogDialog()
{
    delete ui;

}

void adblogDialog::on_copyButton_clicked()
{
 clipboard->setText(content);
}

void adblogDialog::on_swapButton_clicked()
{
getfile = !getfile;

if (getfile)
    ui->logfileName->setText("adbfire.log");
else
 ui->logfileName->setText("adbfire.old.log");

getlog();
ui->logBrowser->setPlainText(content);

}
