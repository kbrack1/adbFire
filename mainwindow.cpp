#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog2.h"
#include "helpdialog.h"
#include "uninstalldialog.h"
#include "llamadialog.h"
#include "usbfiledialog.h"
#include "uuiddialog.h"
#include "creditsdialog.h"
#include <QMessageBox>
#include <QTableWidget>
#include <QResource>
#include <QProcess>
#include <QDir>
#include <QFileDialog>
#include <QFile>
#include <QProgressBar>
#include <QTimer>
#include <QLabel>
#include <QInputDialog>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <preferencesdialog.h>
#include <QElapsedTimer>
#include <QTextStream>
#include <QDate>
#include <QRegularExpression>
#include <QStringList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDesktopServices>
#include <QUrl>
#include <QPixmap>
#include <QIcon>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtNetwork/QNetworkInterface>
#include <adblogdialog.h>

// #include <QDebug>

// busybox mount -t cifs //192.168.1.3/avi /sdcard/samba -o username=guest

#ifdef Q_OS_LINUX
 int os=0;
#elif defined(Q_OS_WIN)
  int os=1;
#elif defined(Q_OS_MAC)
int os=2;
#endif

const QString version = "1.15";

bool isConnected = false;
bool serverRunning = false;
bool  is_packageInstalled = false;
bool  mounted_op = false;
bool firstrun=true;
bool dbexists = false;
bool updatecheck = true;
bool versioncheck = true;
bool sshcheck = false;
bool killadbBool = true;
bool resetadbBool = true;

QString port = ":5555";
QString filename = "";
QString adbdir =  "";
QString adb = "";
QString xmldir = "";
QString splashdir = "";
QString recoverydir = "";
QString hdir = "";
QString daddr="";
QString sldir = "";
QString pushdir = "";
QString pulldir = "";
QString xbmcpackage ="";
QString sshpassword = "";
QString dbstring = "";



QString adbstr1 = "ADB running. ";
QString adbstr2 = "ADB not running. ";
QString devstr1 = "  Device connected";
QString devstr2 = "  Device not connected.";

QString amazon_update1 = "Amazon updates on.      ";
QString amazon_update0 = "Amazon updates off.     ";

QString sshserver1 = "SSH server on       ";
QString sshserver2 = "SSH server off      ";


int usbcheck;
int ftvupdate;
int checkversion;

int tsvalue = 4000;

QSqlDatabase db;


/////////////////////////////////////////
void delayTimer(int rdelay)
{

 QElapsedTimer rtimer;

int nMilliseconds;
 int i = 0;

 rtimer.start();

while(i == 0)
  {
    qApp->processEvents();
     nMilliseconds = rtimer.elapsed();
   if (nMilliseconds >= rdelay)
       break;
}


}



//////////////////////////////////////////////
void rotate_logfile()

{

 QFile file(adbdir+"adbfire.old.log");

 if( file.exists() )
     QFile::remove(adbdir+"adbfire.old.log");


QFile file2(adbdir+"adbfire.log");

if( file2.exists() )
    file2.rename(adbdir+"adbfire.old.log");


}


//////////////////////////////////////////////
void logfile(QString line)

{


QFile file(adbdir+"adbfire.log");
   if(!file.open(QFile::WriteOnly | QFile::Text | QFile::Append))
      {
       QMessageBox::critical(0, "","Can't create logfile!\n",QMessageBox::Cancel);
       return;
      }

   QTextStream out(&file);
            out  << line << endl;

 }


/////////////////////////////////////////
void rebootDevice(QString reboot)
{

 QElapsedTimer rtimer;
 int nMilliseconds;

QProcess reboot_device;
rtimer.start();
reboot_device.setProcessChannelMode(QProcess::MergedChannels);
QString cstring = adb + " -s " +daddr+port + reboot;
reboot_device.start(cstring);
reboot_device.waitForStarted();
while(reboot_device.state() != QProcess::NotRunning)
  {
    qApp->processEvents();
     nMilliseconds = rtimer.elapsed();
   if (nMilliseconds >= 5000)
       break;
}


}



///////////////////////////////////////////////
QString RunProcess(QString cstring)
{
 QProcess run_command;
 run_command.setProcessChannelMode(QProcess::MergedChannels);
 run_command.start(cstring);

 run_command.waitForStarted();

 while(run_command.state() != QProcess::NotRunning)
     qApp->processEvents();

 QString command=run_command.readAll();

 return command;
}


/////////////////////////////
bool is_su()
{

QString cstring = adb + " shell ls /system/xbin/su";
QString command=RunProcess(cstring);

if (command.contains("No such file or directory"))
   {
    logfile(cstring);
    logfile("su not found");
    return false;
    }

else

   {
   return true;
    }


}


////////////////////////////////////////////////
 QString strip (QString str)
{
    str = str.simplified();
    str.replace( " ", "" );
    return str;
}

 //////////////////////////////////////////
 void kill_server()
{

 QString cstring = adb + " kill-server";
 QString command=RunProcess(cstring);
 serverRunning = false;
 
}


 //////////////////////////////////////
void start_server()
{

 QString cstring = adb + " start-server";
 QString command=RunProcess(cstring);

     if (command.contains("daemon started successfully"))
        {
           logfile("server started");
           logfile(cstring);
           logfile(command);
           serverRunning = true;
          }

         else
        {
         logfile("start-server failed!");
         logfile(cstring);
         logfile(command);
         serverRunning = false;
         }
}


/////////////////////////////////////////////////////
bool is_package(QString package)
{

    QString cstring = adb + " -s " + daddr + port + " shell pm list packages ";
    QString command=RunProcess(cstring);

        if (command.contains(package))
            {
            logfile(package+ " is installed");
            logfile(cstring);
            // logfile(command);
            is_packageInstalled = true;
            }
            else
            {
            logfile(package+ " not found");
            logfile(cstring);
            // logfile(command);
            is_packageInstalled = false;}

        return  is_packageInstalled;
}

/////////////////////////////////////////////////////
bool mount_system(QString mnt)
{
       QString cstring = adb + " -s " +daddr+port+ " shell su -c mount -o remount,"+mnt+ " /system";
       QString command=RunProcess(cstring);



        if (command.isEmpty())
          {
            logfile("/system mounted "+mnt);
            logfile(cstring);
            logfile(command);
            return true;
          }
            else
          {
            logfile("/system not mounted "+mnt);
            logfile(cstring);
            logfile(command);
            return false;
          }

}

/////////////////////////////////////////////////////
bool mount_root(QString mnt)
{
       QString cstring = adb + " -s " +daddr+port+ " shell su -c mount -o remount,"+mnt+ " /";
       QString command=RunProcess(cstring);

       logfile(cstring);
       logfile(command);

    if (command.isEmpty())
           {
             logfile("/ mounted "+mnt);
             logfile(cstring);
             logfile(command);
             return true;
            }
            else
            {
               logfile("/ not mounted "+mnt);
               logfile(cstring);
               logfile(command);
               return false;
               }

}


/////////////////////////////////////////////////////
bool amazon_updates(QString onoff)
{


       QString cstring = adb + " -s " + daddr+port + " shell su -c pm "+ onoff + " com.amazon.dcp";
       QString command=RunProcess(cstring);



       logfile("amazon updates");

        if (onoff == "enable")
           {

            if (command.contains("enabled"))
               {
                logfile(cstring);
                logfile(command);
                return true;
            }

            else

            {
                logfile(cstring);
                logfile(command);
                return false;
            }


           }




        if (onoff == "disable")
           {

            if (command.contains("disabled"))
              {
                logfile(cstring);
                logfile(command);
                return true;
            }

            else

            {
                logfile(cstring);
                logfile(command);
                return false;
            }

           }

        return false;
}


////////////////////////////////
void createTables()
{


    logfile("creating adbfire.db");

    QString sqlstatement = "create table device(id int primary key, name varchar(20),sldir varchar(100),pushdir varchar(100),pulldir varchar(100), xbmcpackage varchar(50) , sshpassword varchar(10) , versioncheck int)";

    QSqlQuery query;
    query.exec(sqlstatement);

    if (query.lastError().isValid())
     {
        logfile(sqlstatement);
        logfile("SqLite error:" + query.lastError().text());
        logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
       }


    sqlstatement="insert into device values(1, '','"+hdir+"','"+hdir+"','"+hdir+"' ,'org.xbmc.xbmc','password', 1 )";
    query.exec(sqlstatement);

    if (query.lastError().isValid())
     {
      logfile(sqlstatement);
      logfile("SqLite error:" + query.lastError().text());
      logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
    }

}


////////////////////////////////
void updateTables()
{


 // QMessageBox::critical(0, "",daddr,QMessageBox::Cancel);


    logfile("updating database adbfire.db");

    QString str1;
    QString str2;
    QString str3;
    QString str4;
    QString str5;

    str1.setNum(usbcheck);
    str2.setNum(ftvupdate);
    str3.setNum(checkversion);



    QSqlQuery query;

    QString sqlstatement = "UPDATE device SET name='"+daddr+"' WHERE Id=1";
     query.exec(sqlstatement);

     if (query.lastError().isValid())
      {
        logfile(sqlstatement);
        logfile("SqLite error:" + query.lastError().text());
        logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
       }


    sqlstatement = "UPDATE device SET sldir='"+sldir+"' WHERE Id=1";
     query.exec(sqlstatement);


     if (query.lastError().isValid())
      {
        logfile(sqlstatement);
        logfile("SqLite error:" + query.lastError().text());
        logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
       }

    sqlstatement = "UPDATE device SET pushdir='"+pushdir+"' WHERE Id=1";
     query.exec(sqlstatement);

     if (query.lastError().isValid())
      {
        logfile(sqlstatement);
        logfile("SqLite error:" + query.lastError().text());
        logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
       }

    sqlstatement = "UPDATE device SET pulldir='"+pulldir+"' WHERE Id=1";
     query.exec(sqlstatement);


     if (query.lastError().isValid())
      {
        logfile(sqlstatement);
        logfile("SqLite error:" + query.lastError().text());
        logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
       }

    sqlstatement = "UPDATE device SET xbmcpackage='"+xbmcpackage+"' WHERE Id=1";
     query.exec(sqlstatement);

     if (query.lastError().isValid())
      {
        logfile(sqlstatement);
        logfile("SqLite error:" + query.lastError().text());
        logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
       }


     sqlstatement = "UPDATE device SET sshpassword='"+sshpassword+"' WHERE Id=1";
      query.exec(sqlstatement);

      if (query.lastError().isValid())
       {
         logfile(sqlstatement);
         logfile("SqLite error:" + query.lastError().text());
         logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
        }

     sqlstatement = "UPDATE device SET versioncheck='"+str3+"' WHERE Id=1";
      query.exec(sqlstatement);

      if (query.lastError().isValid())
       {
         logfile(sqlstatement);
         logfile("SqLite error:" + query.lastError().text());
         logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
        }

      sqlstatement = "UPDATE device SET resetadb='"+str4+"' WHERE Id=1";
       query.exec(sqlstatement);

       if (query.lastError().isValid())
        {
          logfile(sqlstatement);
          logfile("SqLite error:" + query.lastError().text());
          logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
         }



       sqlstatement = "UPDATE device SET killadb='"+str5+"' WHERE Id=1";
        query.exec(sqlstatement);

        if (query.lastError().isValid())
         {
           logfile(sqlstatement);
           logfile("SqLite error:" + query.lastError().text());
           logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
          }



}

////////////////////////////////
void readTables()
{

  QString sqlstatement;


    logfile("reading database");

     QSqlQuery query;


     sqlstatement= "SELECT name FROM device";
     query.exec(sqlstatement);
         while (query.next()) {
              daddr = query.value(0).toString();
         }


         // QMessageBox::critical(0, "",daddr,QMessageBox::Cancel);


         if (query.lastError().isValid())
          {
            logfile(sqlstatement);
            logfile("SqLite error:" + query.lastError().text());
            logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
           }


         sqlstatement="SELECT sldir FROM device";
         query.exec(sqlstatement);
           while (query.next()) {
                sldir = query.value(0).toString();
          }



           if (query.lastError().isValid())
            {
              logfile(sqlstatement);
              logfile("SqLite error:" + query.lastError().text());
              logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
             }


           sqlstatement="SELECT pushdir FROM device";
           query.exec(sqlstatement);
            while (query.next()) {
                 pushdir = query.value(0).toString();
           }



            if (query.lastError().isValid())
             {
               logfile(sqlstatement);
               logfile("SqLite error:" + query.lastError().text());
               logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
              }


           sqlstatement="SELECT pulldir FROM device";
            query.exec(sqlstatement);
            while (query.next()) {
                  pulldir = query.value(0).toString();
            }


            if (query.lastError().isValid())
             {
               logfile(sqlstatement);
               logfile("SqLite error:" + query.lastError().text());
               logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
              }



            sqlstatement="SELECT xbmcpackage FROM device";
            query.exec(sqlstatement);
            while (query.next()) {
                  xbmcpackage = query.value(0).toString();
            }


            if (query.lastError().isValid())
             {
               logfile(sqlstatement);
               logfile("SqLite error:" + query.lastError().text());
               logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
              }


            sqlstatement="SELECT sshpassword FROM device";
            query.exec(sqlstatement);
            while (query.next()) {
                  sshpassword = query.value(0).toString();
            }


            if (query.lastError().isValid())
             {
               logfile(sqlstatement);
               logfile("SqLite error:" + query.lastError().text());
               logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
              }



            sqlstatement="SELECT versioncheck FROM device";
            query.exec(sqlstatement);
            while (query.next()) {
                  checkversion = query.value(0).toInt();
            }

            if (query.lastError().isValid())
             {
               logfile(sqlstatement);
               logfile("SqLite error:" + query.lastError().text());
               logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
              }






     if (sldir.isEmpty())
         sldir = hdir;

     if (pushdir.isEmpty())
      pushdir = hdir;

     if (pulldir.isEmpty())
      pulldir = hdir;

     if (xbmcpackage.isEmpty())
         xbmcpackage = "org.xbmc.xbmc";

     if (checkversion==0)
         versioncheck=false;
     else
         versioncheck=true;



}


//////////////////////////////////////////////
void open_pref_database()

{

    QFile Fout(dbstring);

    if(!Fout.exists())
    {
      dbexists = false;
    }
    else
    {
      dbexists = true;
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbstring);


    if (!db.open()) {
        QMessageBox::critical(0, qApp->tr("Cannot open database"),
            "Database error:\n"+dbstring
                     , QMessageBox::Cancel);
     logfile("error opening database "+dbstring);

    }



}

//////////////////////////////////
bool isConnectedToNetwork()
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




//////////////////////////////////////////////
 MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{


     if (os == 1)
        {
         adbdir = "./";
         adb = adbdir+"adb.exe";
        }

   if (os == 2)

        {
          adbdir = QCoreApplication::applicationDirPath();
           adbdir = adbdir+"/adbfiles/";
           adb = adbdir+"adb.osx";
        }

   if (os == 0)

        {
          adbdir = QCoreApplication::applicationDirPath();
           adbdir = adbdir+"/adbfiles/";
           adb = adbdir+"adb.linux";
        }



      dbstring = adbdir+"adbfire.db";
      xmldir = adbdir+"remotes/";
      splashdir = adbdir+"splash/";
      recoverydir = adbdir+"recovery/";

     ui->setupUi(this);


     ui->statusBar->addPermanentWidget(ui->update_status);
     ui->statusBar->addPermanentWidget(ui->server_running);
     ui->statusBar->addPermanentWidget(ui->device_connected);
     ui->statusBar->addPermanentWidget( ui->progressBar);
     ui->progressBar->setHidden(true);

    this->setFixedSize(this->size().width(), this->size().height());

     QPixmap pix(":/assets/donate.png");
     QIcon icon(pix);
     ui->donate->setIcon(icon);
     ui->donate->setIconSize(pix.size());

     ui->usbBox->setDisabled(true);
     ui->server_running->setText(adbstr2);
     ui->device_connected->setText(devstr2);
     ui->update_status->setText("");



  rotate_logfile();


  logfile("adbFire logfile");

  hdir = QDir::homePath();
  open_pref_database();

   if (!dbexists)
    createTables();

   readTables();

   ui->device->setText(daddr);

    if (!(os == 1))
      {  QString cstring = "chmod 0755 "+adb;
         QString command=RunProcess(cstring);
       }

   if (resetadbBool)
         kill_server();

    start_server();
    logfile("starting server");
    if (serverRunning)
     { ui->server_running->setText(adbstr1);

       if (!daddr.isEmpty() && firstrun)
          {  on_connButton_clicked();

           }
    }

    else
    { ui->server_running->setText(adbstr2);
      ui->device_connected->setText(devstr2);
      ui->update_status->setText("");
      ui->usbBox->setDisabled(true);
    }


    if (checkversion==1)
         get_data();


    QString cstring = adb + " -s " + daddr + port + " shell sshstatus";
    QString command=RunProcess(cstring);


    if (command.contains("not found"))
       {
        logfile("ssh check");
        logfile(cstring);
        logfile(command);



       }



}


MainWindow::~MainWindow()
{
    if (ui->usbBox->isChecked())
        usbcheck = 1;
    else
        usbcheck = 0;


    if (updatecheck)
        ftvupdate = 1;
    else
        ftvupdate = 0;

     if(killadbBool)
         kill_server();


    open_pref_database();
    logfile("open database for update");
    updateTables();
    logfile("closing database");
    db.close();
    logfile("closing program");
    delete ui;

}


//////////////////////////////////////////////////////////////////////
void MainWindow::get_data() {
   QNetworkRequest request;
   request.setUrl(QUrl("http://www.jocala.com/version.txt"));

   QNetworkAccessManager *nam = new QNetworkAccessManager();
   QNetworkReply *reply = nam->get(request);

   connect(reply, SIGNAL(finished()),
           this, SLOT(onReqCompleted()));



}

void MainWindow::onReqCompleted() {
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

       s1 = strip(s1);

       int err = reply->error();

       QString s2 = QString::number(err);

       if (version != s1)
       {
           QMessageBox::StandardButton reply;
            reply = QMessageBox::question(0, "", "adbFire version "+s1+" is ready. Download?",
                    QMessageBox::Yes|QMessageBox::No);
              if (reply == QMessageBox::Yes)
               {
                  QString link = "http://www.jocala.com/adbfire.html";
                  QDesktopServices::openUrl(QUrl(link));
                }

       }

       delete reply;

}




//////////////////////////////////////////////////////////////////////
void MainWindow::on_actionQuit_triggered()
{
    QCoreApplication::quit();
}
/////////////////////////////////////////////////////
void MainWindow::TimerEvent()
{
  int value = ui->progressBar->value();

  if (value >= 100)
      {
         value = 0;
         ui->progressBar->reset();
     }


  ui->progressBar->setValue(value+1);

  // zzzz
}


////////////////////////////////////////////////////////////////////////////
void MainWindow::on_sideload_Button_clicked()
{


    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }


    QElapsedTimer rtimer;
    int nMilliseconds;
    rtimer.start();

   QString fileName = QFileDialog::getOpenFileName(this,
          tr("Select app to install"), sldir , tr("APK Files (*.apk)"));


    if (!fileName.isEmpty() )
    {

    QFileInfo finfo(fileName);
    sldir = finfo.absolutePath();

    QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "Install", "Install "+fileName+"?\n",
                                    QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes)
      {


          ui->progressBar->setHidden(false);
          ui->progressBar->setValue(0);

          QTimer *timer = new QTimer(this);
          connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
          timer->start(tsvalue);

          QString cstring = adb + " install -r " + '"'+ fileName+'"';

          QString command=RunProcess(cstring);

           ui->progressBar->setHidden(true);

           logfile(cstring);
           logfile(command);

           nMilliseconds = rtimer.elapsed();
           logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );


           if (command.contains("Success"))

               QMessageBox::information(this,"","Installed");
              else      
               QMessageBox::critical(this,"","Install failed");

    }

  }



}


///////////////////////////////////////////////////////////////////////////
void MainWindow::on_uninstall_Button_clicked()
{

QString package = "";
QString cstring;
bool keepbox = false;

    if (!isConnected)
           { QMessageBox::critical(
                 this,
                 tr("adbFire"),
                 tr("Device not connected"));
              return;
        }

    QElapsedTimer rtimer;
    int nMilliseconds;
    rtimer.start();


    logfile("open uninstall dialog");

    uninstallDialog dialog;
    dialog.setModal(true);
    if(dialog.exec() == QDialog::Accepted)
    {

    package = dialog.packageName();
    keepbox = dialog.keepBox();

    }

    else return;


    if (package.isEmpty())
       {
        QMessageBox::critical(this,"","No file selected");
        return;
        }



            if ( !is_package(package))
               { QMessageBox::critical(
                     this,
                     "",
                     package +" not installed");
                  return;

            logfile("Error: "+ package +" not installed");
            }


            QMessageBox::StandardButton reply;
                  reply = QMessageBox::question(this, "Uninstall", "Uninstall "+package+"?",
                     QMessageBox::Yes|QMessageBox::No);
                  if (reply == QMessageBox::Yes) {

                      ui->progressBar->setHidden(false);
                      ui->progressBar->setValue(0);

                      QTimer *timer = new QTimer(this);
                      connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
                      timer->start(tsvalue);


                      if (!keepbox)
                      cstring = adb + " -s " +daddr+port+ " shell pm uninstall " + package;
                      else
                      cstring = adb + " -s " +daddr+port+ " shell pm uninstall -k " + package;

                      QString command=RunProcess(cstring);


                      ui->progressBar->setHidden(true);

                      logfile(cstring);

                      nMilliseconds = rtimer.elapsed();
                      logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );


                      if (!command.contains("Success"))

                         {   logfile(command);
                          QMessageBox::critical(
                                      this,
                                     "",
                                      "Uninstall failed");
                        }
                          else
                      {
                            logfile(command);
                             QMessageBox::information(
                                      this,
                                      "",
                                      "Uninstalled");
                       }


                  }
}


/////////////////////////////////////////////////////////////////////////
void MainWindow::on_connButton_clicked()
{


    if (ui->device->text().isEmpty())
    {

        QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device address required."));
          return;
    }

    QElapsedTimer rtimer;
    int nMilliseconds;
    rtimer.start();



    port = ":5555";

    daddr = ui->device->text();
    QString cstring = adb + " connect " +daddr+port;

    QString command=RunProcess(cstring);

    if (command.contains("connected to"))
           isConnected=true;
       else
        isConnected=false;

     if(isConnected)
       {



         cstring = adb + " -s " + daddr+port + " shell pm list packages -d";
         command=RunProcess(cstring);

         logfile("check amazon update status");
         logfile(cstring);
         logfile(command);

         if (command.contains("package:com.amazon.dcp"))
         ui->update_status->setText(amazon_update0);
         else
          ui->update_status->setText(amazon_update1);


         cstring = adb + " -s " + daddr + port +  " shell ls /etc/init.d/01mntdrives";
         command=RunProcess(cstring);


                 logfile("check usb persistence");
                 logfile(command);
                 logfile(cstring);

                  if (command.contains("No such file or directory"))
                      ui->usbBox->setChecked(false);
                  else
                     ui->usbBox->setChecked(true);



         ui->device_connected->setText(devstr1);
         ui->server_running->setText(adbstr1);
         serverRunning = true;
         ui->usbBox->setDisabled(false);


     }
       else
        { ui->device_connected->setText(devstr2);
         ui->usbBox->setDisabled(true);

         if (!firstrun)
         QMessageBox::critical(
                      this,
                      tr("adbFire"),
                      tr("Device not connected"));
     }

        firstrun=false;

        nMilliseconds = rtimer.elapsed();
        logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );

}





////////////////////////////////////////////////////////////////
void MainWindow::on_disButton_clicked()
{

    if (ui->device->text().isEmpty())
    {

        QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device address required."));
          return;
    }


    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }


    QElapsedTimer rtimer;
    int nMilliseconds;
    rtimer.start();

        ui->usbBox->setDisabled(true);
        daddr = ui->device->text();
        QString cstring = adb + " disconnect "+daddr+port ;

        QString command=RunProcess(cstring);

        isConnected=false;
         ui->device_connected->setText("  Device not connected.");
         ui->update_status->setText("");
         nMilliseconds = rtimer.elapsed();
         logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );

}


///////////////////////////////////////////////////////////////////////
void MainWindow::on_astart_Button_clicked()
{

    QElapsedTimer rtimer;
    int nMilliseconds;
    rtimer.start();


    kill_server();
    start_server();
    if (serverRunning)
      ui->server_running->setText(adbstr1);
    else
     ui->server_running->setText(adbstr2);

    nMilliseconds = rtimer.elapsed();
    logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );



}


////////////////////////////////////////////////////////////////////
void MainWindow::on_akill_Button_clicked()
{

    QElapsedTimer rtimer;
    int nMilliseconds;
    rtimer.start();


    QString cstring = adb + "  disconnect "+daddr+port ;
    QString command=RunProcess(cstring);

    isConnected=false;
     ui->device_connected->setText(devstr2);
      ui->update_status->setText("");

    kill_server();

    if (serverRunning)
      ui->server_running->setText(adbstr1);
    else
     ui->server_running->setText(adbstr2);

    nMilliseconds = rtimer.elapsed();
    logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );


}


/////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionAbout_triggered()
{


    Dialog2 dialog2;
    dialog2.setModal(true);
    dialog2.setvLabel(version);
    dialog2.exec();

}


//////////////////////////////////////////////
void MainWindow::on_actionHelp_triggered()
{
    logfile("opening help");
    helpDialog helpdialog;
    helpdialog.setModal(true);
    helpdialog.exec();
}



/////////////////////////////////////////////
void MainWindow::on_backupButton_clicked()
{

    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }

    is_package(xbmcpackage);

   if (!is_packageInstalled)
      { QMessageBox::critical(
            this,
            "",
            "Kodi not installed");
         return;
   }


   QElapsedTimer rtimer;
   int nMilliseconds;
   rtimer.start();



QString hdir = QDir::homePath();
 QString command = "";
 QString xpath = "/sdcard/Android/data/"+xbmcpackage;

 QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Backup Destination"),
                                              hdir,
                                              QFileDialog::ShowDirsOnly
                                              | QFileDialog::DontResolveSymlinks);


 if (!dir.isEmpty() )
 {


 QMessageBox::StandardButton reply;
   reply = QMessageBox::question(this, "Backup", "backup xbmc to "+dir,
                                 QMessageBox::Yes|QMessageBox::No);
   if (reply == QMessageBox::Yes) {


       ui->progressBar->setHidden(false);
       ui->progressBar->setValue(0);

       QTimer *timer = new QTimer(this);
       connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
       timer->start(tsvalue);

       QString cstring = adb + " -s " + daddr + port + " pull "+xpath+" "+'"'+dir+'"';
       QString command=RunProcess(cstring);

       if (command.contains("bytes"))


          {
           logfile(cstring);
           logfile(command);
           QMessageBox::information(
                       this,
                      "",
                      "Backup done" );
           }
           else

       {
           logfile(cstring);
           logfile(command);

           QMessageBox::critical(
                       this,
                       "",
                    "Backup failed");
       }


   }




}

 nMilliseconds = rtimer.elapsed();
 logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );


 ui->progressBar->setHidden(true);

}



////////////////////////////////////////////////////////////////////
 void MainWindow::on_rootButton_clicked()
 {

     QString rootfile1 = adbdir+"tr.apk";
     QString rootfile2 = adbdir+"su.apk";

     bool file1 = false;
     bool file2 = false;

                        if (!isConnected)
                           { QMessageBox::critical(
                                 this,
                                 tr("adbFire"),
                                 tr("Device not connected"));
                              return;
                        }

                        QElapsedTimer rtimer;
                        int nMilliseconds;
                        rtimer.start();



                        QFile Fout1(rootfile1);

                        if(!Fout1.exists())
                        {

                            QMessageBox::critical(
                               this,
                               tr("adbFire"),
                               rootfile1+" not found.");
                               logfile(rootfile1+" not found.");

                               return;
                        }


                        QFile Fout2(rootfile2);

                        if(!Fout2.exists())
                        {

                            QMessageBox::critical(
                               this,
                               tr("adbFire"),
                               rootfile2+" not found.");
                               logfile(rootfile2+" not found.");
                               return;
                        }

                              ui->progressBar->setHidden(false);
                              ui->progressBar->setValue(0);

                              QTimer *timer = new QTimer(this);
                              connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
                              timer->start(tsvalue);

                              QString cstring = adb + " -s " + daddr + port + " install -r "+rootfile1;

                            QString command=RunProcess(cstring);

                              if (!command.contains("Success"))
                                 {
                                  logfile(cstring);
                                  logfile(command);
                                  QMessageBox::information(
                                              this,
                                              "",
                                              "tr.apk install failed");                                 
                                   ui->progressBar->setHidden(true);
                                   return;
                                 }

                              else
                                  file1 = true;

                              logfile(cstring);
                              logfile(command);

                              cstring = adb + " -s " + daddr + port + " install -r "+rootfile2;

                              command=RunProcess(cstring);
                                     
                                     ui->progressBar->setHidden(true);

                                     if (!command.contains("Success"))
                                        {
                                         logfile(cstring);
                                         logfile(command);
                                         QMessageBox::information(
                                                     this,
                                                     "",
                                                     "su.apk install failed");

                                        }
                                     else
                                    file2 = true;

                                     logfile(cstring);
                                     logfile(command);

 if (file1 && file2)
 {

  QMessageBox::information( this,"","Root software installed.\nRun towelroot on the FireTV\n to finish rooting process.");
  logfile("Root software installed.\nRun towelroot on the FireTV\n to finish rooting process.");

  nMilliseconds = rtimer.elapsed();
  logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );


 }

 }



///////////////////////////////////////////
void MainWindow::on_fpushButton_clicked()

{
    if (!isConnected)
       { QMessageBox::critical(
             this,
             "adbFire",
             devstr2);
          return;
    }



    QElapsedTimer rtimer;
    int nMilliseconds;
    rtimer.start();



 QString xpath = "";

 QString hidden;


 if (xbmcpackage.contains(".kodi"))
    hidden=".kodi";
  else
      hidden=".xbmc";



 QString cname = ui->comboBox->currentText();

switch(ui->comboBox->currentIndex() ){
case 0:
xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/addons/";
break;

case 1:
xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/userdata/keymaps";
break;

case 2:
xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/media/";
break;

case 3:
xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/sounds/";
break;

case 4:
xpath = "/sdcard/Android/data/org.xbmc.xbmc/files/"+hidden+"/system/";
break;

case 5:
xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/userdata/";
break;

case 6:
xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/temp/";
break;

case 7:
xpath = "/sdcard/";
break;

default:
xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/addons/";
break;
}


is_package(xbmcpackage);

if (xpath != "/sdcard/")
   if (!is_packageInstalled)
     { QMessageBox::critical(
           this,
           "",
           "Kodi not installed");
       logfile("xbmc not installed. cant push to it.");
        return;
     }



 QString fileName = QFileDialog::getOpenFileName(this,
 "Push file to "+cname, pushdir, tr("Files (*)"));


 if (!fileName.isEmpty() )
 {

     QFileInfo finfo(fileName);
     pushdir = finfo.absolutePath();

     QString cstring = adb + " -s " + daddr + port +  " shell ls "+xpath;

     QString command=RunProcess(cstring);

      if (command.contains("No such file or directory"))
       { QMessageBox::critical(
                      this,
                     "",
                      "Destination path missing. Has Kodi had its first run to set up internal folders?");
          logfile(xpath);
          logfile("Destination path missing. Has Kodi had its first run to set up internal folders?");
          return;
      }





     QMessageBox::StandardButton reply;
       reply = QMessageBox::question(this, "Push", "Push "+fileName+" to "+cname+"?",
           QMessageBox::Yes|QMessageBox::No);
       if (reply == QMessageBox::Yes) {


           ui->progressBar->setHidden(false);
           ui->progressBar->setValue(0);


           QTimer *timer = new QTimer(this);
           connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
           timer->start(tsvalue);

           cstring = adb + " -s " + daddr + port +  " push "+'"'+fileName+'"'+ " "+xpath;

           command=RunProcess(cstring);

           if (command.contains("bytes"))
            {
               logfile(cstring);
               logfile(command);
               QMessageBox::information(
                           this,
                          "",
                          "File Pushed." );
           }
               else
           {
               logfile(cstring);
               logfile(command);

               QMessageBox::critical(
                           this,
                           "",
                        "Push failed ");
           }

   }

}

 ui->progressBar->setHidden(true);
 nMilliseconds = rtimer.elapsed();
 logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );



}



/////////////////////////////////////////////////
void MainWindow::on_restoreButton_clicked()

{
    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }

    is_package(xbmcpackage);

   if (!is_packageInstalled)
      { QMessageBox::critical(
            this,
            "",
            "Kodi not installed");
         return;
   }




   QElapsedTimer rtimer;
   int nMilliseconds;
   rtimer.start();




 QString xpath = "/sdcard/Android/data/";

 QString cstring;
 QString command;


 cstring = adb + " shell su -c ps | grep "+xbmcpackage;
 command=RunProcess(cstring);


 if (command.contains(xbmcpackage))
    { QMessageBox::critical(this,"","Cannot restore while\n"+xbmcpackage+" is running!");
      logfile(xbmcpackage+" running. Restore failed");
      return;
 }


 QString dir = QFileDialog::getExistingDirectory(this, tr("Choose restore folder"),
                                              hdir,
                                              QFileDialog::ShowDirsOnly
                                              | QFileDialog::DontResolveSymlinks);

 if (!dir.isEmpty() )
 {


 QMessageBox::StandardButton reply;
   reply = QMessageBox::question(this, "Restore", "Restore Kodi from "+dir+"\n"+"\n"+"This will overwrite the existing setup!",
                                 QMessageBox::Yes|QMessageBox::No);
   if (reply == QMessageBox::Yes) {


       ui->progressBar->setHidden(false);
       ui->progressBar->setValue(0);

       QTimer *timer = new QTimer(this);
       connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
       timer->start(tsvalue);

       cstring = adb + " -s " + daddr + port + " shell rm -r "+xpath +xbmcpackage;

       command=RunProcess(cstring);

       cstring = adb + " -s " + daddr + port +  " push "+'"'+dir+'"'+ " "+xpath+xbmcpackage;

        command=RunProcess(cstring);

        ui->progressBar->setHidden(true);

        logfile(cstring);
        logfile(command);

        nMilliseconds = rtimer.elapsed();
        logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );



        if (command.contains("bytes"))

       {
           QMessageBox::information(
                       this,
                      "",
                      "Restore complete." );
       }
           else

       {

           QMessageBox::critical(
                       this,
                       "",
                    "Restore failed ");
       }

   }

}

}

//////////////////////////////////////////
void MainWindow::on_pushRemote_clicked()
{
    if (!isConnected)
       { QMessageBox::critical(
             this,
             "adbFire",
             devstr2);
          return;
    }

    is_package(xbmcpackage);

   if (!is_packageInstalled)
      { QMessageBox::critical(
            this,
            "",
            "Kodi not installed");
         return;
   }


   QString hidden;

   if (xbmcpackage.contains(".kodi"))
      hidden=".kodi";
    else
        hidden=".xbmc";

QString  xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/userdata/keymaps/";

QElapsedTimer rtimer;
int nMilliseconds;
rtimer.start();



 QString fileName = QFileDialog::getOpenFileName(this,
 "Choose remote xml file", xmldir, tr("Files (*.xml)"));

 if (!fileName.isEmpty() )
 {


      QString cstring = adb + " -s " + daddr + port +  " shell ls "+xpath;

       logfile(cstring);

      QString command=RunProcess(cstring);

      if (command.contains("No such file or directory"))
       {
          logfile(cstring);
          logfile(command);
          QMessageBox::critical(
                      this,
                     "",
                      "Destination path missing");
                      return;
      }


     QMessageBox::StandardButton reply;
       reply = QMessageBox::question(this, "Push", fileName+" selected. Continue?",
           QMessageBox::Yes|QMessageBox::No);
       if (reply == QMessageBox::Yes) {


           cstring = adb + " -s " + daddr + port +  " push "+'"'+fileName+'"'+ " "+xpath+"/keyboard.xml";

           command=RunProcess(cstring);

           logfile(cstring);
           logfile(command);

           nMilliseconds = rtimer.elapsed();
           logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );



           if (command.contains("bytes"))


           {

               QMessageBox::information(
                           this,
                          "",
                          "Remote xml installed." );
           }
               else

           {

               QMessageBox::critical(
                           this,
                           "",
                        "Remote xml installation failed.");}


   }

}

}

////////////////////////////////////////////////
void MainWindow::on_consoleButton_clicked()
{

   // /Applications/Utilities/Terminal.app/Contents/MacOS/Terminal /Users/jeff/Desktop

    if (!isConnected)
          { QMessageBox::critical(
                this,
                "adbFire",
                devstr2);
          return;
       }



    logfile("detaching console process");

    QString cstring = "";


    if (os == 1)

       {
       cstring = "cmd /k  adb -s  "  + daddr + port + " shell";
       QProcess::startDetached(cstring);
       }


     else

       {


         QString commstr = adbdir+"console.sh";
         QFile file(commstr);

             if(!file.open(QFile::WriteOnly |
                           QFile::Text))
             {



                 logfile("error creating console.sh!");
                 QMessageBox::critical(this,"","Error creating command file!");
                 return;
             }



             QTextStream out(&file);
             out  << "#!/bin/sh" << endl;
             out  <<  adb+ " shell" << endl;


             file.flush();
             file.close();

       cstring = "chmod 0755 " + commstr ;
       QString command=RunProcess(cstring);

      //  cstring = "gnome-terminal -e "+adbdir+"console.sh";
      //  cstring = "x-terminal-emulator -e "+adbdir+"console.sh";

       if (os == 0)
         cstring = "x-terminal-emulator -e "+adbdir+"console.sh";
       if (os == 2)
        cstring = "open -a Terminal.app "+adbdir+"console.sh";



        QProcess::startDetached(cstring);
        }


}

//////////////////////////////////////////////////
void MainWindow::on_actionPreferences_triggered()
{


    QString cstring;
    QString command;
    QString oldpass;


    oldpass = sshpassword;


    if (is_su())
       {
        cstring = adb + " -s " + daddr+port + " shell su -c ls /system/etc/init.d/02sshd";
        command=RunProcess(cstring);

       logfile(cstring);
       logfile(command);

       if (command.contains("No such file or directory"))
          {
           sshcheck = false;
          }
        else
       {
        sshcheck = true;
       }


       cstring = adb + " -s " + daddr+port + " shell pm list packages -d";
         command=RunProcess(cstring);

        logfile(cstring);
        logfile(command);

        if (command.contains("package:com.amazon.dcp"))
           {
            ui->update_status->setText(amazon_update0);
            updatecheck = false;
           }

        else

        {
         ui->update_status->setText(amazon_update1);
         updatecheck = true;
        }

    }

 else

  {

    sshcheck = false;
    updatecheck = true;
    }








    logfile("opening preferences dialog");

    bool currentupdate = updatecheck;

    preferencesDialog dialog;
    dialog.setPackagename(xbmcpackage);
    dialog.setSSHpassword(sshpassword);
    dialog.setPulldir(pulldir);
    dialog.setftvUpdate(updatecheck);
    dialog.setsshBoot(sshcheck);
    dialog.setversioncheck(versioncheck);
    dialog.setversionLabel(version);

    dialog.setModal(true);


    if(dialog.exec() == QDialog::Accepted)
    {

    mount_system("rw");

    xbmcpackage = dialog.xbmcpackageName();
    sshpassword = dialog.sshpassword();
    pulldir = dialog.pulldir();


    updatecheck = dialog.updatecheck();
    versioncheck = dialog.versioncheck();
    sshcheck = dialog.sshcheck();




    if (versioncheck)
        checkversion = 1;
    else
        checkversion = 0;




  if (isConnected)
  {




 if (is_su())
    {


      if (!sshcheck)

      {

            cstring = adb + " -s " + daddr+port + " shell su -c rm /system/etc/init.d/02sshd";
            command=RunProcess(cstring);

           logfile(cstring);
           logfile(command);

           if (command.isEmpty())
               logfile("/system/etc/init.d/02sshd deleted");
           else
               logfile("/system/etc/init.d/02sshd not deleted");



      }


       else

      {
          logfile("write 02sshd");

          QString hashbang = "#!/system/bin/sh";
          QString filename = adbdir+"02sshd";
          QString makepst = "/system/xbin/sshstart " + sshpassword;


          QFile file(filename);

            //  if(!file.open(QFile::WriteOnly |
                          //  QFile::Text))

      if(!file.open(QFile::WriteOnly))

              {

                  logfile(cstring);
                  logfile(command);
                  logfile("error creating 02sshd file.");
                  QMessageBox::critical(this,"","Error creating 02sshd file!");
                  return;
              }


              QTextStream out(&file);
              out  << hashbang + "\n";
              out  << makepst + "\n";

              file.flush();
              file.close();

              mount_system("rw");

               cstring = adb + " -s " + daddr + port +  " push " +filename+ " /sdcard/";
               command=RunProcess(cstring);
               if (!command.contains("bytes"))
                    {


                      logfile(cstring);
                      logfile(command);
                      logfile("error pushing shell script to device!");

                       QMessageBox::critical(this,"","Error pushing file frpm PC to device!");
                       mount_system("ro");
                       return;
                     }



              cstring = adb + " -s " + daddr+port + " shell su -c cp " + " /sdcard/02sshd /system/etc/init.d";
              QString command=RunProcess(cstring);

              if (!command.isEmpty())
                   {

                     logfile(cstring);
                     logfile(command);
                     logfile("file copy error, sdcard to system/etc");

                     QMessageBox::critical(this,"","Error: cp /sdcard/01mountusb /system/etc/init.d failed");
                      mount_system("ro");
                      return;
                    }


              logfile(cstring);
              logfile(command);

              cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 " + " /system/etc/init.d/02sshd";
              command=RunProcess(cstring);

              if (!command.isEmpty())
                   {

                  logfile(cstring);
                  logfile(command);
                  logfile("chmod error, system/etc/init.d/02sshd");

                      QMessageBox::critical(this,"","Error: chmod of /system/etc/init.d/02sshd failed");
                      mount_system("ro");
                      return;
                    }


              logfile(cstring);
              logfile(command);

              cstring = adb + " -s " + daddr+port + " shell rm " + " /sdcard/02sshd";
             command=RunProcess(cstring);

              if (!command.isEmpty())
                   {

                  logfile(cstring);
                  logfile(command);
                  logfile("error: rm /sdcard/02sshd");                
                    }


              cstring = adb + " -s " + daddr + port +  " shell ls /system/etc/init.d/02sshd";
              command=RunProcess(cstring);

              if (!command.contains("/system/etc/init.d/02sshd"))
                   {

                  logfile(cstring);
                  logfile(command);
                  logfile("Error: /etc/init.d/02sshd not created. SSH is not persistent");

                  QMessageBox::critical(this,"","Error: /etc/init.d/02sshd not created.SSH is not persistent");
                    }

               else
                    {

                  logfile(cstring);
                  logfile(command);
                  logfile("SSH is now persistent");
                  // QMessageBox::information(this,"","SSH is persistent");

              }

             mount_system("ro");
             QFile::remove(filename);

      }

}

      if (updatecheck != currentupdate) //  has update pref changed
   {

     logfile("update preference changed.");

    if ( is_su() )
     {

    if (updatecheck)
     {
       if (amazon_updates("enable"))
         { // QMessageBox::information(this,"","Amazon updates on\ncom.amazon.dcp enabled");
           logfile("Amazon updates on, com.amazon.dcp enabled");


           cstring = adb + " -s " + daddr+port + " shell pm list packages -d";
           command=RunProcess(cstring);

           logfile(cstring);
           logfile(command);

           if (!command.contains("package:com.amazon.dcp"))
            ui->update_status->setText(amazon_update1);

       }


       else
        {QMessageBox::critical(this,"","Problem: com.amazon.dcp not enabled!");
        logfile("Problem: com.amazon.dcp not enabled!");
        ui->update_status->setText(amazon_update0);
       }
    }

    else
    {
        if (amazon_updates("disable"))
           {// QMessageBox::information(this,"","Amazon updates off\ncom.amazon.dcp disabled");
            logfile("Amazon updates off, com.amazon.dcp disabled");


            cstring = adb + " -s " + daddr+port + " shell pm list packages -d";
            command=RunProcess(cstring);

            logfile(cstring);
            logfile(command);

            if (command.contains("package:com.amazon.dcp"))
            ui->update_status->setText(amazon_update0);

        }
      else
         { QMessageBox::critical(this,"","Problem: com.amazon.dcp not disabled!");
            logfile("Problem: com.amazon.dcp not disabled!");
            ui->update_status->setText(amazon_update1);
          }
    }
  }

}  //  has update pref changed

}



    open_pref_database();
    updateTables();
    db.close();

    }

   else return;

}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionReboot_triggered()
{

     QString cstring;

    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }


    /*

   if (!is_su())
      { QMessageBox::critical(
            this,
            "",
            "Root required!");
         return;
   }

*/


   QMessageBox::StandardButton reply;
     reply = QMessageBox::question(this, "Reboot Device", "Reboot Device?",
         QMessageBox::Yes|QMessageBox::No);
     if (reply == QMessageBox::Yes) {
         isConnected=false;
         ui->device_connected->setText(devstr2);
         ui->update_status->setText("");
        logfile("rebooting device");
        rebootDevice(" reboot");
     }

}

///////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionRecovery_triggered()
{

    QString cstring;

    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }

  /*
   if (!is_su())
      { QMessageBox::critical(
            this,
            "",
            "Root required!");
         return;
   }

*/

   QMessageBox::StandardButton reply;
     reply = QMessageBox::question(this, "", "Reboot Recovery?",
         QMessageBox::Yes|QMessageBox::No);
     if (reply == QMessageBox::Yes) {
         isConnected=false;
         ui->device_connected->setText(devstr2);
         ui->update_status->setText("");
         logfile("rebooting device recovery");
         rebootDevice(" reboot recovery");
        }

}
///////////////////////////////////////////////
void MainWindow::on_screenshotButton_clicked()
{

    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }


    QElapsedTimer rtimer;
    int nMilliseconds;
    rtimer.start();



        QDateTime dateTime = QDateTime::currentDateTime();
        QString dtstr = dateTime.toString("MMddyyhhmmss");

        QString cstring = adb +" -s " + daddr+port +" shell screencap -p /sdcard/"+dtstr+".png";


    QString command=RunProcess(cstring);

    if (!command.isEmpty())

    {
        logfile(cstring);
        logfile(command);

        QMessageBox::critical(
                    this,
                   "",
                    "Screenshot failed");
    }

    else
    {

        logfile(cstring);
        logfile(command);

        QString cstring = adb +" -s " + daddr+port +" pull /sdcard/"+dtstr+".png " +pulldir;

        command=RunProcess(cstring);

        logfile(cstring);
        logfile(command);


        if (!command.contains("bytes"))
         {
            logfile(cstring);
            logfile(command);
            QMessageBox::critical(
                        this,
                       "",
                        "Screenshot failed");
        }
        else
        {
            logfile(cstring);
            logfile(command);

            QMessageBox::information(
                           this,
                          "",
                           "Screenshot "+dtstr+ " copied to "+pulldir);
        }

        cstring = adb +" -s " + daddr+port +" shell rm /sdcard/"+dtstr+".png " ;
        command=RunProcess(cstring);
        logfile(cstring);


    }

    nMilliseconds = rtimer.elapsed();
    logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );


}


//////////////////////////////////////////////////////
void MainWindow::on_actionInstall_busybox_triggered()
{

    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }


 //

   if (!is_su())
      { QMessageBox::critical(
            this,
            "",
            "Root required!");
         return;
   }


   QElapsedTimer rtimer;
   int nMilliseconds;
   rtimer.start();


   QMessageBox::StandardButton reply;
         reply = QMessageBox::question(this, "Busybox", "Install Busybox?",
            QMessageBox::Yes|QMessageBox::No);
         if (reply == QMessageBox::No)
         return;


         ui->progressBar->setHidden(false);
         ui->progressBar->setValue(0);


         QTimer *timer = new QTimer(this);
         connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
         timer->start(tsvalue);

    logfile("starting busybox install");


    QString busybox1 = adbdir+"busybox";
    QString busybox2 = adbdir+"samba.tar";
    QString busybox3 = adbdir+ "install-recovery-2.sh";
    QString busybox4 = adbdir+"install-recovery.sh";
    QString busybox5 = adbdir+"xbin.tar";



     QFile Fout1(busybox1);

         if(!Fout1.exists())
            {

            QMessageBox::critical(
            this,
            tr("adbFire"),
            busybox1+" not found.");

            logfile(busybox1+" not found.");
            ui->progressBar->setHidden(true);
            return;
             }


        QFile Fout2(busybox2);

       if(!Fout2.exists())
          {

            QMessageBox::critical(
             this,
             tr("adbFire"),
              busybox2+" not found.");
            logfile(busybox2+" not found.");
            ui->progressBar->setHidden(true);
             return;
           }


       QFile Fout3(busybox3);

      if(!Fout3.exists())
         {

           QMessageBox::critical(
            this,
            tr("adbFire"),
             busybox3+" not found.");
           logfile(busybox3+" not found.");
           ui->progressBar->setHidden(true);
            return;
          }


      QFile Fout4(busybox4);

      if(!Fout4.exists())
         {

           QMessageBox::critical(
            this,
            tr("adbFire"),
             busybox4+" not found.");
           logfile(busybox4+" not found.");
           ui->progressBar->setHidden(true);
            return;
          }


      QFile Fout5(busybox5);

      if(!Fout5.exists())
         {

           QMessageBox::critical(
            this,
            tr("adbFire"),
             busybox5+" not found.");
           logfile(busybox5+" not found.");
           ui->progressBar->setHidden(true);
            return;
          }






      QString cstring = adb + " -s " + daddr + port + " push "+busybox1+ " /sdcard/";

      QString command=RunProcess(cstring);

         if (!command.contains("bytes"))
           { QMessageBox::critical(
            this,
             "",
             "file1: busybox install failed ");
             logfile("file1: busybox install failed ");
             logfile(cstring);
              logfile(command);
              ui->progressBar->setHidden(true);
             return;
             }



         logfile(cstring);
         logfile(command);

              cstring = adb + " -s " + daddr + port + " push "+busybox2+ " /sdcard/";
              command=RunProcess(cstring);


              if (!command.contains("bytes"))
                 { QMessageBox::critical(
                              this,
                              "",
                              "file2: busybox install failed");
                  logfile("file2: busybox install failed ");
                  logfile(cstring);
                  logfile(command);
                  ui->progressBar->setHidden(true);
                  return;
                 }


              logfile(cstring);
              logfile(command);


               cstring = adb + " -s " + daddr + port + " push "+busybox3+ " /sdcard/";
               command=RunProcess(cstring);

               if (!command.contains("bytes"))
                  { QMessageBox::critical(
                               this,
                               "",
                               "file3: busybox install failed");
                   logfile("file3: busybox install failed ");
                   logfile(cstring);
                   logfile(command);
                   ui->progressBar->setHidden(true);
                   return;
                  }


               logfile(cstring);
               logfile(command);


                cstring = adb + " -s " + daddr + port + " push "+busybox4+ " /sdcard/";
                command=RunProcess(cstring);


                if (!command.contains("bytes"))
                   { QMessageBox::critical(
                                this,
                                "",
                                "file4: busybox install failed");

                    logfile("file4: busybox install failed ");
                    logfile(cstring);
                    logfile(command);
                    ui->progressBar->setHidden(true);
                    return;
                   }


                logfile(cstring);
                logfile(command);

                 cstring = adb + " -s " + daddr + port + " push "+busybox5+ " /sdcard/";

                 command=RunProcess(cstring);

                 if (!command.contains("bytes"))
                    { QMessageBox::critical(
                                 this,
                                 "",
                                 "file5: busybox install failed");

                     logfile("file5: busybox install failed ");
                     logfile(cstring);
                     logfile(command);
                     ui->progressBar->setHidden(true);
                     return;
                    }

                 logfile(cstring);
                 logfile(command);


      mount_system("rw");


     cstring = adb + " -s " + daddr+port + " shell su -c cp " + " /sdcard/busybox /system/xbin";
     command=RunProcess(cstring);
     
     logfile(cstring);
     logfile(command);

     cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 /system/xbin/busybox";
     command=RunProcess(cstring);

     logfile(cstring);
     logfile(command);



     cstring = adb + " -s " + daddr+port + " shell su -c /system/xbin/busybox tar xf /sdcard/xbin.tar -C /system";
     command=RunProcess(cstring);

     logfile(cstring);
     logfile(command);

     cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 /system/xbin/*.sh";
     command=RunProcess(cstring);

     logfile(cstring);
     logfile(command);

     cstring = adb + " -s " + daddr+port + " shell su -c cp /system/xbin/mntdrives1.sh /system/xbin/mntdrives.sh";
     command=RunProcess(cstring);

     logfile(cstring);
     logfile(command);

     cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 /system/xbin/01mntdrives";
     command=RunProcess(cstring);
     logfile(cstring);
     logfile(command);

     cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 /system/xbin/02sshd";
     command=RunProcess(cstring);
     logfile(cstring);
     logfile(command);

     cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 /system/xbin/mount.exfat-fuse";
     command=RunProcess(cstring);
     logfile(cstring);
     logfile(command);

     cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 /system/xbin/ntfs-3g";
     command=RunProcess(cstring);
     logfile(cstring);
     logfile(command);


     cstring = adb + " -s " + daddr+port + " shell su -c /system/xbin/binstall.sh";
     command=RunProcess(cstring);

     logfile(cstring);
     logfile(command);
     
      cstring = adb + " -s " + daddr + port +  " shell ls /system/xbin/which";
      command=RunProcess(cstring);
      
      logfile(cstring);
      logfile(command);

        if (command.contains("No such file or directory"))
          { logfile("busybox install failed!");
            QMessageBox::critical( this,"","Busybox not installed!");
        }

        else
        {

            cstring = adb + " -s " + daddr+port + " shell su -c rm /sdcard/xbin.tar";
            command=RunProcess(cstring);

            logfile(cstring);
            logfile(command);

            cstring = adb + " -s " + daddr+port + " shell su -c tar xf /sdcard/samba.tar -C /data/data";
            command=RunProcess(cstring);

            logfile(cstring);
            logfile(command);

            cstring = adb + " -s " + daddr+port + " shell su -c ln -s /data/data/com.funkyfresh.samba/files/samba-rc /system/xbin/samba";
            command=RunProcess(cstring);

            logfile(cstring);
            logfile(command);

         cstring = adb + " -s " + daddr+port + " shell su -c rm /sdcard/samba.tar";
         command=RunProcess(cstring);

         logfile(cstring);
         logfile(command);


         cstring = adb + " -s " + daddr+port + " shell su -c mkdir /system/etc/init.d";
         command=RunProcess(cstring);

         logfile(cstring);
         logfile(command);

         cstring = adb + " -s " + daddr+port + " shell su -c chmod 777 /system/etc/init.d";
         command=RunProcess(cstring);

         logfile(cstring);
         logfile(command);



         cstring = adb + " -s " + daddr+port + " shell su -c cp /sdcard/install-recovery*  /system/etc/";
         command=RunProcess(cstring);

         logfile(cstring);
         logfile(command);

         cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 /system/etc/install-recovery*";
         command=RunProcess(cstring);

         logfile(cstring);
         logfile(command);


         cstring = adb + " -s " + daddr+port + " shell su -c rm /sdcard/install-recovery*";
         command=RunProcess(cstring);

         logfile(cstring);
         logfile(command);


         cstring = adb + " -s " + daddr+port + " shell su -c cp /system/etc/init.bueller.sh  /system/etc/init.bueller.sh.old";
         command=RunProcess(cstring);

         logfile(cstring);
         logfile(command);


         cstring = adb + " -s " + daddr+port + " shell su -c cp /system/xbin/init.bueller.sh  /system/etc/";
         command=RunProcess(cstring);

         logfile(cstring);
         logfile(command);

         logfile(cstring);
         logfile(command);
         QMessageBox::information( this,"","Busybox installed!");
         logfile("busybox installed!");
         ui->progressBar->setHidden(false);


        }

mount_system("ro");



nMilliseconds = rtimer.elapsed();
logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );

ui->progressBar->setHidden(true);

}

///////////////////////////////////////////////////////
void MainWindow::on_actionUninstall_Busybox_triggered()
{

    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }


  //

   if (!is_su())
      { QMessageBox::critical(
            this,
            "",
            "Root required!");
         return;
   }



   QString command;
   QString cstring;

   QString umntstring = "/system/xbin/umount /storage/usb/drive*/";
   QString rmsd = "rm -r /storage/usb/drive*/";
   QString rmsam = "rm -r /data/data/com.funkyfresh.samba/";
   QString rmsh = "rm /system/etc/install-recovery-2.sh";


   cstring = adb + " -s " + daddr + port +  " shell ls /system/xbin/busybox";
   command=RunProcess(cstring);

   logfile(cstring);
   logfile(command);

     if (command.contains("No such file or directory"))
        {
         QMessageBox::critical( this,"","Busybox not installed!");
         return;
        }

   QMessageBox::StandardButton reply;
         reply = QMessageBox::question(this, "Busybox", "Uninstall Busybox?",
            QMessageBox::Yes|QMessageBox::No);
         if (reply == QMessageBox::No)
         return;


         ui->progressBar->setHidden(false);
         ui->progressBar->setValue(0);

         QTimer *timer = new QTimer(this);
         connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
         timer->start(tsvalue);



         QElapsedTimer rtimer;
         int nMilliseconds;
         rtimer.start();

         logfile("busybox uninstall");

         cstring = adb + " -s " + daddr+port + " shell su -c ls /storage/usb/drive*/";
         command=RunProcess(cstring);

       if (!command.contains("No such file or directory"))

        {

             logfile(cstring);
             logfile(command);

             mount_root("rw");



             cstring = adb + " -s " + daddr+port + " shell su -c /data/data/com.funkyfresh.samba/files/samba-rc stop";
             command=RunProcess(cstring);;

             logfile(cstring);
             logfile(command);

             cstring = adb + " -s " + daddr+port + " shell su -c " + rmsam;
             command=RunProcess(cstring);;

             logfile(cstring);
             logfile(command);

         cstring = adb + " -s " + daddr+port + " shell su -c " + rmsh;
         command=RunProcess(cstring);;

         logfile(cstring);
         logfile(command);



          cstring = adb + " -s " + daddr+port + " shell su -c " + umntstring;
          command=RunProcess(cstring);;

          logfile(cstring);
          logfile(command);

          cstring = adb + " -s " + daddr+port + " shell su -c " + rmsd;
          command=RunProcess(cstring);

          logfile(cstring);
          logfile(command);

          mount_root("ro");

      }



        mount_system("rw");


        cstring = adb + " -s " + daddr + port +  " shell ls /system/xbin/sshstop";
        command=RunProcess(cstring);

        logfile(cstring);
        logfile(command);


         cstring = adb + " -s " + daddr+port + " shell su -c /system/xbin/buninstall.sh";
         command=RunProcess(cstring);

         logfile(cstring);
         logfile(command);

         cstring = adb + " -s " + daddr + port +  " shell ls /system/xbin/busybox";
         command=RunProcess(cstring);
         
         logfile(cstring);
         logfile(command);

           if (command.contains("No such file or directory"))
            {
               logfile("busybox uninstalled");
               QMessageBox::information( this,"","Busybox uninstalled"); }
           else
           { logfile("busybox uninstall failed");
               QMessageBox::critical( this,"","Busybox not uninstalled!");}






           mount_system("ro");

    nMilliseconds = rtimer.elapsed();
    logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );

    ui->progressBar->setHidden(true);
    ui->usbBox->setChecked(false);

}

/////////////////////////////////////////////
void MainWindow::on_fdellButton_clicked()
{

    if (!isConnected)
          { QMessageBox::critical(
                this,
                "adbFire",
                devstr2);
             return;
       }


    QElapsedTimer rtimer;
    int nMilliseconds;
    rtimer.start();




     QString xpath = "";
     QString cname = ui->comboBox->currentText();


     QString hidden;

    if (xbmcpackage.contains(".kodi"))
       hidden=".kodi";
     else
         hidden=".xbmc";


     switch(ui->comboBox->currentIndex() ){
     case 0:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/addons/";
     break;

     case 1:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/userdata/keymaps/";
     break;

     case 2:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/media/";
     break;

     case 3:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/sounds/";
     break;

     case 4:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/system/";
     break;

     case 5:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/userdata/";
     break;

     case 6:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/temp/";
     break;

     case 7:
     xpath = "/sdcard/";
     break;

     default:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/addons/";
     break;
     }



  is_package(xbmcpackage);

  if (xpath != "/sdcard/")
     if (!is_packageInstalled)
       { QMessageBox::critical(
             this,
             "",
             "Kodi not installed");
         logfile(xpath);
         logfile("xbmc not installed, path not found");
          return;
       }



  QString cstring;
  QString pullfile;


  //cstring = adb + " shell su -c find " +xpath+ " -type f ";
  //QString command=RunProcess(cstring);

  cstring = adb + " shell ls " +xpath;
  QString command=RunProcess(cstring);



  if (command.isEmpty())
     { QMessageBox::critical(this,"","No files found");
      logfile("no files found");
      logfile(cstring);
      logfile(command);
     return;
      }


  QFile file21(adbdir+"temp.txt");

    if(!file21.open(QFile::WriteOnly |
                  QFile::Text))
    {

        logfile(adbdir+"temp.txt");
        logfile("error creating file");
        QMessageBox::critical(this,"","Error creating file!");
        return;
    }


    QTextStream out1(&file21);
    out1  << command << endl;

    file21.flush();
    file21.close();



  usbfileDialog sddialog;
  sddialog.setModal(true);
  sddialog.setData("Select file to delete");
  if(sddialog.exec() == QDialog::Accepted)
  pullfile = sddialog.binfileName();
  else return;

  if (pullfile.isEmpty())
     {
      QMessageBox::critical(this,"","No file selected");
      return;
      }

  QMessageBox::StandardButton reply3;
    reply3 = QMessageBox::question(this, "", "Delete "+pullfile,
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply3 == QMessageBox::No)
        return;



            // cstring = adb + " -s " + daddr + port +  " shell rm "+pullfile;
           //  command=RunProcess(cstring);
             
             cstring = adb + " -s " + daddr + port +  " shell rm "+xpath+pullfile;
             command=RunProcess(cstring);




             if (command.contains("exist"))
              {
                 logfile(cstring);
                 logfile(command);
                 logfile( "Deletion failed");
                 QMessageBox::critical(
                             this,
                            "",
                             "Deletion failed");}
                 else
                  {
                    logfile(cstring);
                    logfile(command);
                    logfile("Deletion succeeded");
                    QMessageBox::information(
                             this,
                             "",
                             "Deletion succeeded");}


 nMilliseconds = rtimer.elapsed();
 logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );


}

//////////////////////////////////////////////////////
void MainWindow::on_actionFirmware_install_triggered()
{


    bool usbstick = false;
    QString cstring;
    QString command;
    QString updatezip = "--update_package=/cache/update.zip";
    QString commstr = adbdir+"command";
    QString tmpstr = adbdir+"tmpstr";


    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }


   if (!is_su())
      { QMessageBox::critical(
            this,
            "",
            "Root required!");
         return;
   }


 logfile("firmware installation query");

 QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "", "Install Firmware?",
                                 QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No)
      {logfile("firmware installation cancelled");
        return;}


    QElapsedTimer rtimer;
     int nMilliseconds;

    rtimer.start();



    logfile("firmware installation started");


        cstring = adb + " -s " + daddr+port + " shell su -c chmod 777 /cache";
        command=RunProcess(cstring);

        cstring = adb + " -s " + daddr+port + " shell su -c chmod 777 /cache/recovery";
        command=RunProcess(cstring);


        if (command.contains("No such file or directory"))
         {

            logfile(cstring);
            logfile(command);
            logfile("/cache/recovery/ not found");

            cstring = adb + " -s " + daddr+port + " shell su -c mkdir -p /cache/recovery/";
            command=RunProcess(cstring);

            logfile(cstring);
            logfile(command);

            if (!command.isEmpty())
             {

                QMessageBox::critical(this,"","Firmware installation aborted. See log.");
                logfile("Firmware installation aborted.");
                return;
             }



            cstring = adb + " -s " + daddr+port + " shell su -c chmod 777 /cache/recovery";
            command=RunProcess(cstring);

            logfile(cstring);
            logfile(command);

            if (!command.isEmpty())
             {  logfile("chmod error. cancelling firmware installation");
                QMessageBox::information( this,"","Error: "+command);
                return; }


        }




    QFile file(commstr);

        if(!file.open(QFile::WriteOnly |
                      QFile::Text))
        {

            logfile(cstring);
            logfile(command);
            logfile("error creating command file. cancelling firmware installation.");
            QMessageBox::critical(this,"","Error creating command file!");
            return;
        }


        QTextStream out(&file);
        out  << updatezip << endl;

        file.flush();
        file.close();



    cstring = adb + " -s " + daddr + port + " push "+commstr+ " /sdcard/";
    command=RunProcess(cstring);
    
    if (!command.contains("bytes"))
     { QMessageBox::information( this,"","Error: "+command);

        logfile(cstring);
        logfile(command);
        logfile("error pushing command file. cancelling firmware installation.");

        return; }

    cstring = adb + " -s " + daddr+port + " shell su -c cp " + " /sdcard/command /cache/recovery/";
    command=RunProcess(cstring);

    if (!command.isEmpty())
     { QMessageBox::information( this,"","Error: "+command);

        logfile(cstring);
        logfile(command);
        logfile("error cp command file. cancelling firmware installation.");

        return; }

    logfile(cstring);
    logfile(command);

    cstring = adb + " -s " + daddr+port + " shell su -c chmod 777 /cache/recovery/command";
    command=RunProcess(cstring);

    if (!command.isEmpty())
     { QMessageBox::information( this,"","Error: "+command);

        logfile(cstring);
        logfile(command);
        logfile("error chmod command file. cancelling firmware installation.");

        return; }

    logfile(cstring);
    logfile(command);

    QString fileName;

    QMessageBox::StandardButton reply2;
      reply2 = QMessageBox::question(this, "", "Are firmware files on usb storage?",
                                    QMessageBox::Yes|QMessageBox::No);
      if (reply2 == QMessageBox::Yes)
         {

          logfile("searching usb for files");

          cstring = adb + " shell su -c find /storage/usb -name *.bin -o -name *.zip";
          command=RunProcess(cstring);

          if (command.isEmpty())
             { QMessageBox::critical(this,"","No files found");

              logfile(cstring);
              logfile("no files found");
              return;
              }


          logfile(cstring);
          logfile(command);

          QFile file21(adbdir+"temp.txt");

            if(!file21.open(QFile::WriteOnly |
                          QFile::Text))
            {
                QMessageBox::critical(this,"","Error creating file!");

                logfile(cstring);
                logfile(command);
                logfile("error creating "+adbdir+ "temp.txt");
                return;
            }


            QTextStream out1(&file21);
            out1  << command << endl;

            file21.flush();
            file21.close();



          usbfileDialog sddialog;
          sddialog.setModal(true);
          sddialog.setData("Select firmware file");
          if(sddialog.exec() == QDialog::Accepted)
          fileName = sddialog.binfileName();
          else return;


          if (fileName.isEmpty())
             {
              QMessageBox::critical(this,"","No file selected");

              logfile("no file selected");
              return;
              }



          usbstick = true;
          cstring = adb + " -s " + daddr+port + " shell ls "+fileName;
          command=RunProcess(cstring);

          if (command.contains("No such file or directory"))
           {

              logfile(cstring);
              logfile("no such file or directory");
              QMessageBox::critical(this,"","Error: "+command);
              return;
          }

          }

         else

      {
          usbstick = false;
          fileName = QFileDialog::getOpenFileName(this,
          "Choose firmware file", hdir, tr("Files (*)"));

      }



    if (!fileName.isEmpty() )
    {

     ui->progressBar->setHidden(false);
     ui->progressBar->setValue(0);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
    timer->start(tsvalue);

    if (!usbstick)
      {
        logfile("pushing "+fileName+" to /sdcard/");
        cstring = adb + " -s " +daddr+port+" push "  + '"' +   fileName + '"'   + " /sdcard/update.zip";
        command=RunProcess(cstring);

        logfile(cstring);
        logfile(command);

        if (!command.contains("bytes"))
        {
          logfile("pushing "+fileName+" to /sdcard/ failed. Cancelling firmware installation");
           return;
        }

      }


    if (!usbstick)
    cstring = adb + " -s " + daddr+port + " shell su -c cp " + " /sdcard/update.zip /cache/";
    else
    cstring = adb + " -s " + daddr+port + " shell su -c cp " + fileName + " /cache/update.zip";

    command=RunProcess(cstring);

    if (!command.isEmpty())
    {

     nMilliseconds = rtimer.elapsed();
     logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );

      logfile("cp of firmware file to /cache failed. Cancelling firmware installation");
       return;
    }

    
   if (!usbstick)
   {
    cstring = adb + " -s " + daddr+port + " shell rm /sdcard/update.zip";
    command=RunProcess(cstring);
    logfile(cstring);
    logfile(command);

   }

   logfile(cstring);
   logfile(command);



    cstring = adb + " -s " + daddr+port + " shell rm /sdcard/command";
    command=RunProcess(cstring);

    logfile(cstring);
    logfile(command);


    QFile::remove(commstr);
    QFile Fout(commstr);

    if(Fout.exists())
    {
        QMessageBox::critical(this,"","Error: rm of PC: "+commstr+" script");
        logfile("Error: rm of PC: "+commstr+" script");
    }



    ui->progressBar->setHidden(true);

    nMilliseconds = rtimer.elapsed();
   logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );


    QMessageBox::StandardButton reply3;
      reply3 = QMessageBox::question(this, "", "Reboot to recovery?\nFirmware:"+fileName,
                                    QMessageBox::Yes|QMessageBox::No);
      if (reply3 == QMessageBox::No)
       {


          logfile("firmware installation cancelled by user");

          cstring = adb + " -s " + daddr+port + " shell su -c rm /cache/update.zip";
          command=RunProcess(cstring);

          logfile(cstring);
          logfile(command);

          cstring = adb + " -s " + daddr+port + " shell su -c rm /cache/recovery/command";
          command=RunProcess(cstring);


          logfile(cstring);
          logfile(command);

          return;

       }

      logfile("rebooting to recovery");

      rebootDevice(" reboot recovery");
     
     isConnected=false;
     ui->device_connected->setText(devstr2);

    }


}


//////////////////////////////////////////////
void MainWindow::on_mntButton_clicked()
{

    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }


  //

   if (!is_su())
      { QMessageBox::critical(
            this,
            "",
            "Root required!");
         return;
   }



   QElapsedTimer rtimer;
   int nMilliseconds;
   rtimer.start();



QString cstring;

   ui->progressBar->setHidden(false);
   ui->progressBar->setValue(0);

   QTimer *timer = new QTimer(this);
   connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
   timer->start(tsvalue);

   cstring = adb + " -s " + daddr + port +  " shell ls /system/xbin/mount";
   QString command=RunProcess(cstring);


     if (command.contains("No such file or directory"))
      { QMessageBox::critical( this,"","Busybox required for USB drive. Install it from the menu.");

         logfile(cstring);
         logfile(command);
         logfile("Busybox required for USB drive. Install it from the menu.");
         return;
     }



       cstring = adb + " -s " + daddr+port + " shell su -c /system/xbin/mntdrives.sh";
       command=RunProcess(cstring);

       logfile(cstring);
       logfile(command);

       ui->progressBar->setHidden(true);

       if (command.isEmpty()  || command.contains("FUSE"))
        {
           QMessageBox::information( this,"","USB drive(s) mounted");

           logfile("USB drive(s) mounted");
           logfile(cstring);
           logfile(command);


           cstring = adb + " -s " + daddr+port + " shell su -c /data/data/com.funkyfresh.samba/files/samba-rc start";
           command=RunProcess(cstring);

           logfile(cstring);
           logfile(command);


           return;
        }



       if (command.contains("resource busy"))
        {
           QMessageBox::information( this,"","USB drive already mounted");

           logfile("USB drive already mounted");
           logfile(cstring);
           logfile(command);

           return;
        }


       if (!command.isEmpty())
        {
           logfile(cstring);
           logfile(command);
           logfile("error mounting usb drive");
           QMessageBox::information( this,"","Error: "+command);
           return;
        }


       nMilliseconds = rtimer.elapsed();
       logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );



}


//////////////////////////////////////////
void MainWindow::on_umntButton_clicked()
{

    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }


  //

   if (!is_su())
      { QMessageBox::critical(
            this,
            "",
            "Root required!");
         return;
   }

   QElapsedTimer rtimer;
   int nMilliseconds;
   rtimer.start();




   QString cstring = adb + " -s " + daddr + port +  " shell ls /system/xbin/umount";
   QString command=RunProcess(cstring);


     if (command.contains("No such file or directory"))
      { QMessageBox::critical( this,"","Busybox required for USB drive. Install it from the menu.");

         logfile(cstring);
         logfile(command);
         logfile("Busybox required, not found");
         return;
     }


        ui->progressBar->setHidden(false);
        ui->progressBar->setValue(0);

        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
        timer->start(tsvalue);



    QString umntstring = "/system/xbin/umount /storage/usb/*/";
    QString rmsd = "rm -r /storage/usb/*/";


    mount_root("rw");

       logfile("stopping samba");
       cstring = adb + " -s " + daddr+port + " shell su -c /data/data/com.funkyfresh.samba/files/samba-rc stop";
       command=RunProcess(cstring);

       logfile(cstring);
       logfile(command);

       cstring = adb + " -s " + daddr+port + " shell su -c " + umntstring;
       command=RunProcess(cstring);

        ui->progressBar->setHidden(true);

       if (command.contains("failed"))
         { logfile(cstring);
           logfile(command);
           logfile("error unmounting usb drive");
           QMessageBox::critical( this,"","USB drive not found!");}
       else
        {           
           cstring = adb + " -s " + daddr+port + " shell su -c " + rmsd;
           command=RunProcess(cstring);
           QMessageBox::information( this,"","USB Drive(s) unmounted.");
           logfile(cstring);
           logfile(command);
           logfile("usb drive unmounted");
        }

 mount_root("ro");

 nMilliseconds = rtimer.elapsed();
 logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );




}


////////////////////////////////////////
void MainWindow::on_rwButton_clicked()
{
    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }


   //

   if (!is_su())
      { QMessageBox::critical(
            this,
            "",
            "Root required!");
         return;
   }


   QElapsedTimer rtimer;
   int nMilliseconds;
   rtimer.start();



   if (mount_system("rw"))

      {

       logfile("filesystem remounted r/w");
       QMessageBox::information(
                   this,
                  "",
                   "Filesystem mounted r/w");}
    else

   {

       logfile("filesystem not remounted!");
       QMessageBox::critical(
                      this,
                     "",
                      "Error: Filesystem not remounted r/w!");}


   nMilliseconds = rtimer.elapsed();
   logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );


}

////////////////////////////////////////
void MainWindow::on_roButton_clicked()
{
    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }


 //

   if (!is_su())
      { QMessageBox::critical(
            this,
            "",
            "Root required!");
         return;
   }


   QElapsedTimer rtimer;
   int nMilliseconds;
   rtimer.start();




   if (mount_system("ro"))
    {

       logfile("filesystem remounted r/o");

       QMessageBox::information(
                   this,
                  "",
                   "Filesystem mounted r/o");}
    else

   {
       logfile("filesystem not remounted r/o!");
       QMessageBox::critical(
                      this,
                     "",
                      "Error: Filesystem not remounted r/o!");}


   nMilliseconds = rtimer.elapsed();
   logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );


}



/////////////////////////////////////////
void MainWindow::on_fpullButton_clicked()
{

    if (!isConnected)
          { QMessageBox::critical(
                this,
                "adbFire",
                devstr2);
             return;
       }


    QElapsedTimer rtimer;
    int nMilliseconds;
    rtimer.start();




     QString xpath = "";
     QString cname = ui->comboBox->currentText();
     QString fileName;
     QString cstring;

     QString hidden;

     if (xbmcpackage.contains(".kodi"))
        hidden=".kodi";
      else
          hidden=".xbmc";

     switch(ui->comboBox->currentIndex() ){
     case 0:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/addons/";
     break;

     case 1:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/userdata/keymaps/";
     break;

     case 2:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/media/";
     break;

     case 3:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/sounds/";
     break;

     case 4:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/system/";
     break;

     case 5:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/userdata/";
     break;

     case 6:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/temp/";
     break;

     case 7:
     xpath = "/sdcard/";
     break;


     default:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/addons/";
     break;
     }

     is_package(xbmcpackage);

     if (xpath != "/sdcard/" )
        if (!is_packageInstalled)
          { QMessageBox::critical(
                this,
                "",
                "Kodi not installed");

            logfile("xbmc not installed");
            logfile("install xbmc for "+xpath);

             return;
          }

     // cstring = adb + " shell find " +xpath+ " -type f ";

     cstring = adb + " shell ls " +xpath;
     QString command=RunProcess(cstring);


     if (command.isEmpty())
        { QMessageBox::critical(this,"","No files found");

         logfile(cstring);
         logfile(command);
         logfile("no files found!");
         return;
         }


     QFile file21(adbdir+"temp.txt");

       if(!file21.open(QFile::WriteOnly |
                     QFile::Text))
       {
           QMessageBox::critical(this,"","Error creating file!");
           logfile("error creating file: "+adbdir+"temp.txt");

           return;
       }


       QTextStream out1(&file21);
       out1  << command << endl;

       file21.flush();
       file21.close();



     usbfileDialog sddialog;
     sddialog.setModal(true);
     sddialog.setData("Select file to pull");
     if(sddialog.exec() == QDialog::Accepted)
     fileName = sddialog.binfileName();
     else return;

     if (fileName.isEmpty())
        {
         QMessageBox::critical(this,"","No file selected");
         logfile("no file selected");
         return;
         }


     ui->progressBar->setHidden(false);
     ui->progressBar->setValue(0);


     QTimer *timer = new QTimer(this);
     connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
     timer->start(tsvalue);


             cstring = adb + " -s " + daddr + port +  " pull "+xpath+fileName+" "+pulldir;
             command=RunProcess(cstring);


             if (command.contains("exist"))
              {
                 logfile(cstring);
                 logfile(command);
                 logfile("pull failed");
                 QMessageBox::critical(
                             this,
                            "",
                             "Pull failed");}
                 else
             {
                 logfile(cstring);
                 logfile(command);
                 logfile("pull succeeded");
                 QMessageBox::information(
                             this,
                             "",
                             "Pull succeeded");}

             ui->progressBar->setHidden(true);
             nMilliseconds = rtimer.elapsed();
             logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );


}


//////////////////////////////////////////////
void MainWindow::on_pushSplash_clicked()
{
    if (!isConnected)
       { QMessageBox::critical(
             this,
             "adbFire",
             devstr2);
          return;
    }

    is_package(xbmcpackage);

   if (!is_packageInstalled)
      { QMessageBox::critical(
            this,
            "",
            "Kodi not installed");
         return;
   }


   QString hidden;

   if (xbmcpackage.contains(".kodi"))
      hidden=".kodi";
    else
        hidden=".xbmc";


QString  xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/media/";

QElapsedTimer rtimer;
int nMilliseconds;
rtimer.start();



 QString fileName = QFileDialog::getOpenFileName(this,
 "Choose splash screen file", splashdir, tr("Files (*.png)"));

 if (!fileName.isEmpty() )
 {


      QString cstring = adb + " -s " + daddr + port +  " shell ls "+xpath;

       logfile(cstring);

      QString command=RunProcess(cstring);

      if (command.contains("No such file or directory"))
       {
          logfile(cstring);
          logfile(command);
          QMessageBox::critical(
                      this,
                     "",
                      "Destination path missing");
                      return;
      }


     QMessageBox::StandardButton reply;
       reply = QMessageBox::question(this, "Push", fileName+" selected. Continue?",
           QMessageBox::Yes|QMessageBox::No);
       if (reply == QMessageBox::Yes) {


           cstring = adb + " -s " + daddr + port +  " push "+'"'+fileName+'"'+ " "+xpath+"/splash.png";

           command=RunProcess(cstring);

           logfile(cstring);
           logfile(command);

           nMilliseconds = rtimer.elapsed();
           logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );



           if (command.contains("bytes"))


           {

               QMessageBox::information(
                           this,
                          "",
                          "Splash screen installed." );
           }
               else

           {

               QMessageBox::critical(
                           this,
                           "",
                        "Splash screen installation failed.");}


   }

}

}


///////////////////////////////////////////
void MainWindow::on_llamaButton_clicked()
{

    if (!isConnected)
       { QMessageBox::critical(
             this,
             "adbFire",
             devstr2);
          return;
    }


    is_package(xbmcpackage);

   if (!is_packageInstalled)
      { QMessageBox::critical(
            this,
            "",
            "Kodi not installed");
         return;
   }


   QString llamadir;

   if (xbmcpackage.contains(".kodi"))
      llamadir="kodillama";
    else
      llamadir="xbmcllama";



 QString llama = adbdir+"llama.apk";
 QString command;
 QString cstring;
 QString icontype;
 QString str = "";

 bool classicTV = false;
 bool llamaInstall = false;
 bool isLlama = false;

 bool llamaRadio1;
 bool llamaRadio2;
 bool llamaRadio3;
 bool llamaRadio4;

 int llamaEvent = 5;
 int ctvIcon = 5;

 bool ctvRadio1;
 bool ctvRadio2;
 bool ctvRadio3;
 bool ctvRadio4;

  QFile Fout1(llama);
   if(!Fout1.exists())
    {
    QMessageBox::critical(this,"",llama+" not found.");
    logfile(llama+" not found.");
     return;
    }


   QElapsedTimer rtimer;
   int nMilliseconds;
   rtimer.start();

 is_package("org.ikonotv.smarttv");
   if (is_packageInstalled)
    classicTV = true;

   is_package("com.kebab.Llama");
     if (is_packageInstalled)
      isLlama = true;



   logfile("opening Llama dialog");

    llamaDialog dialog;

    dialog.setModal(true);

    if(dialog.exec() == QDialog::Accepted)
    {


       llamaInstall = dialog.llamacheck();

       llamaRadio1 = dialog.llamaradio1();
       llamaRadio2 = dialog.llamaradio2();
       llamaRadio3 = dialog.llamaradio3();
       llamaRadio4 = dialog.llamaradio4();

       ctvRadio1 = dialog.ctvradio1();
       ctvRadio2 = dialog.ctvradio2();
       ctvRadio3 = dialog.ctvradio3();
       ctvRadio4 = dialog.ctvradio4();

       if (llamaRadio1)
           llamaEvent = 1;

       if (llamaRadio2)
           llamaEvent = 2;

       if (llamaRadio3)
           llamaEvent = 3;

       if (llamaRadio4)
           llamaEvent = 4;

       if (ctvRadio1)
           ctvIcon = 1;

       if (ctvRadio2)
           ctvIcon = 2;

       if (ctvRadio3)
           ctvIcon = 3;

       if (ctvRadio4)
           ctvIcon = 4;


       if (!classicTV )
         if (ctvIcon < 4)
          {
           QMessageBox::information(this,"","Please install ikono TV");
           ui->progressBar->setHidden(true);
           return;
         }


       if (!classicTV && llamaInstall)
         if (llamaEvent > 1)
          {
           QMessageBox::information(this,"","Please install ikono TV");
           ui->progressBar->setHidden(true);
           return;
         }



       if (llamaInstall)

        {


           ui->progressBar->setHidden(false);
           ui->progressBar->setValue(0);

           QTimer *timer = new QTimer(this);
           connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
           timer->start(tsvalue);

           cstring = adb + " -s " + daddr + port + " install -r "+llama;
           command=RunProcess(cstring);

           if (!command.contains("Success"))
              {

               logfile("llama.apk install failed");
               logfile(cstring);
               logfile(command);
               QMessageBox::information(
                           this,
                           "",
                           "llama.apk install failed");
                ui->progressBar->setHidden(true);
                return;
              }

           else
           {
               isLlama = true;
               str="Llama installed\n";
           }

       }



     if (llamaEvent < 4 && isLlama)
     {

         cstring = adb + " -s " + daddr+port + " shell rm -r /sdcard/Llama";
         command=RunProcess(cstring);

         logfile(cstring);
         logfile(command);


         if (llamaEvent == 1)
         {
          cstring = adb + " push "+adbdir+llamadir+"/llama_boot /sdcard/Llama/";
          command=RunProcess(cstring);
          logfile(cstring);
          logfile(command);
          }


       if (llamaEvent == 2)
         {
           cstring = adb + " push "+adbdir+llamadir+"/llama_ctv /sdcard/Llama/";
           command=RunProcess(cstring);
           logfile(cstring);
           logfile(command);
         }


       if (llamaEvent == 3)
         {

           cstring = adb + " push "+adbdir+llamadir+"/llama_both /sdcard/Llama";
           command=RunProcess(cstring);
           logfile(cstring);
           logfile(command);
         }





     }


   if (classicTV && ctvIcon < 4)

   {


           if (ctvIcon == 1)
            {

               icontype = "XBMC icon applied";
               cstring = adb + " shell rm -r /sdcard/.imagecache/com.amazon.venezia/org.ikonotv.smarttv";
               command=RunProcess(cstring);
               logfile(cstring);
               logfile(command);

               cstring = adb + " shell mkdir -p /sdcard/.imagecache/com.amazon.venezia/org.ikonotv.smarttv";
               command=RunProcess(cstring);
               logfile(cstring);
               logfile(command);

               cstring = adb + " push "+adbdir+ "xbmc.icon /sdcard/.imagecache/com.amazon.venezia/org.ikonotv.smarttv";
               command=RunProcess(cstring);
               logfile(cstring);
               logfile(command);
           }


           if (ctvIcon == 2)
            {

               icontype = "Kodi icon applied";
               cstring = adb + " shell rm -r /sdcard/.imagecache/com.amazon.venezia/org.ikonotv.smarttv";
               command=RunProcess(cstring);
               logfile(cstring);
               logfile(command);

               cstring = adb + " shell mkdir -p /sdcard/.imagecache/com.amazon.venezia/org.ikonotv.smarttv";
               command=RunProcess(cstring);
               logfile(cstring);
               logfile(command);

               cstring = adb + " push "+adbdir+ "kodi.icon /sdcard/.imagecache/com.amazon.venezia/org.ikonotv.smarttv";
               command=RunProcess(cstring);
               logfile(cstring);
               logfile(command);
           }

           if (ctvIcon == 3)
            {


               icontype = "ikono TV icon applied";
               cstring = adb + " shell rm -r /sdcard/.imagecache/com.amazon.venezia/org.ikonotv.smarttv";
               command=RunProcess(cstring);
               logfile(cstring);
               logfile(command);

               cstring = adb + " shell mkdir -p /sdcard/.imagecache/com.amazon.venezia/org.ikonotv.smarttv";
               command=RunProcess(cstring);
               logfile(cstring);
               logfile(command);

               cstring = adb + " push "+adbdir+ "ikonotv.icon /sdcard/.imagecache/com.amazon.venezia/org.ikonotv.smarttv";
               command=RunProcess(cstring);
               logfile(cstring);
               logfile(command);
           }



      }

    }

else return;


    ui->progressBar->setHidden(true);
    logfile("exit llama function");

    if (!isLlama)
    {
        QMessageBox::information(this,"","Llama not installed. Click the Llama install checkbox to install Llama");
        logfile("Llama not installed.");
        return;
    }


       if (isLlama && llamaEvent < 4)
          { str = str + "Llama settings applied.";
            str = str + "\nPlease run Llama and import settings from USB to activate!";
          }

        if (ctvIcon < 4)
           str = str + "\n"+icontype;

        if (ctvIcon < 4 || llamaEvent < 4 || llamaInstall)
          {
            QMessageBox::information(this,"",str);
        }



    nMilliseconds = rtimer.elapsed();
    logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );
    ui->progressBar->setHidden(true);


}

/////////////////////////////////////////
void MainWindow::on_donate_clicked()
{
    QString link = "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=JA5E5UP3ZSWBN";
    QDesktopServices::openUrl(QUrl(link));

}

///////////////////////////////////////////////
void MainWindow::on_actionCredits_triggered()
{
    creditsDialog credits;
    credits.setModal(true);
    credits.exec();
}

/////////////////////////////////////////////////////////
void MainWindow::on_actionInstall_Recovery_triggered()
{


    QString cstring;
    QString command;
    QString fileName;

    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }




  //

   if (!is_su())
      { QMessageBox::critical(
            this,
            "",
            "Root required!");
         return;
   }


   cstring = adb + " -s " + daddr+port + " shell su -c cat /system/build.prop | grep 51.1.1.0";
   command=RunProcess(cstring);

   if (!command.isEmpty())
      { QMessageBox::critical(this,"","Downgrade firmware below 51.1.1.0 first!");
       logfile("firmware downgrade below 51.1.1.0 required for CWM recovery install");
       logfile(cstring);
       return;
       }

   logfile("recovery installation starts");


    QElapsedTimer rtimer;
     int nMilliseconds;

    rtimer.start();

    logfile("recovery installation started");

          fileName = QFileDialog::getOpenFileName(this,
          "Choose Recovery Image", recoverydir, tr("Files (*.img)"));

          if (!fileName.isEmpty() )
          {

      QMessageBox::StandardButton reply;
         reply = QMessageBox::question(this, "", "Install Recovery?\n\n"+fileName,
                                      QMessageBox::Yes|QMessageBox::No);
         if (reply == QMessageBox::No)
           {logfile("recovery installation cancelled");
             return;}


     ui->progressBar->setHidden(false);
     ui->progressBar->setValue(0);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
    timer->start(tsvalue);

        logfile("pushing "+fileName+" to /sdcard/");
        cstring = adb + " -s " +daddr+port+" push "  + '"' +   fileName + '"'   + " /sdcard/recovery.img";
        command=RunProcess(cstring);

        logfile(cstring);
        logfile(command);

        if (!command.contains("bytes"))
        {
          logfile("pushing "+fileName+" to /sdcard/ failed. Cancelling firmware installation");
           return;
        }



    cstring=adb + " -s " + daddr+port + " shell su -c  dd if=/sdcard/recovery.img of=/dev/block/platform/msm_sdcc.1/by-name/recovery";
    command=RunProcess(cstring);
    logfile(cstring);
    logfile(command);



 if (!command.contains("bytes transferred"))
 {
   logfile("recovery installation failed");
   logfile(cstring);
   logfile(command);
   QMessageBox::critical(this,"","Error: "+command);

 }


     cstring = adb + " -s " + daddr+port + " shell rm /sdcard/recovery.img";
     command=RunProcess(cstring);
     logfile(cstring);
     logfile(command);

   nMilliseconds = rtimer.elapsed();
   logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );
   ui->progressBar->setHidden(true);

   QMessageBox::information(this,"","New Recovery Installed");


    }

}


/////////////////////////////////////////////////
void MainWindow::on_usbBox_clicked(bool checked)
{

    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }


   if (!is_su())
      { QMessageBox::critical(
            this,
            "",
            "Root required!");
         return;
   }

QString cstring;
QString hashbang = "#!/system/bin/sh";
QString filename = adbdir+"01mntdrives";
QString makepst = "/system/xbin/mntdrives.sh samba";

cstring = adb + " -s " + daddr + port +  " shell su -c ls /system/xbin/mount";
QString command=RunProcess(cstring);

if (command.contains("No such file or directory"))
{

    logfile(cstring);
    logfile(command);
    logfile("busybox required, not found");

    QMessageBox::critical(this,"","Busybox required for USB drive. Install it from the menu.");
    ui->usbBox->setChecked(false);

    return;
}


    if (checked)
    {


        mount_system("rw");


        cstring = adb + " -s " + daddr+port + " shell su -c cp " + " /system/xbin/01mntdrives /system/etc/init.d";
        QString command=RunProcess(cstring);

        if (!command.isEmpty())
             {

               logfile(cstring);
               logfile(command);
               logfile("file copy error, xbin to system/etc");

               QMessageBox::critical(this,"","Error: cp /system/xbin/01mntdrives /system/etc/init.d failed");
                mount_system("ro");
                return;
              }


        logfile(cstring);
        logfile(command);

        cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 " + " /system/etc/init.d/01mntdrives";
        command=RunProcess(cstring);

        if (!command.isEmpty())
             {

            logfile(cstring);
            logfile(command);
            logfile("chmod error, system/etc/init.d/01mntdrives");

                QMessageBox::critical(this,"","Error: chmod of /system/etc/init.d/01mntdrives failed");
                mount_system("ro");
                return;
              }


        logfile(cstring);
        logfile(command);



        cstring = adb + " -s " + daddr + port +  " shell ls /system/etc/init.d/01mntdrives";
        command=RunProcess(cstring);

        if (!command.contains("/system/etc/init.d/01mntdrives"))
             {

            logfile(cstring);
            logfile(command);
            logfile("Error: /etc/init.d/01mntdrives not created. USB drive is not persistent");

            QMessageBox::critical(this,"","Error: /etc/init.d/01mntdrives not created. USB drive is not persistent");
              }

         else
              {

            logfile(cstring);
            logfile(command);
            logfile("USB drive is now persistent");
            QMessageBox::information(this,"","USB drive is persistent");
              }


    }



  else

    {


       mount_system("rw");

        cstring = adb + " -s " + daddr + port +  " shell su -c rm /system/etc/init.d/01mountusb";
        command=RunProcess(cstring);

        QMessageBox::information(this,"","USB drive is not persistent.");
        mount_system("ro");

        logfile(cstring);
        logfile(command);
        logfile("USB drive is not persistent");

        return;

         }


    }



/////////////////////////////////////////////////
void MainWindow::on_actionInstall_SSH_triggered()
{

    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }


   if (!is_su())
      { QMessageBox::critical(
            this,
            "",
            "Root required!");
         return;
   }


   QString cstring;
   QString command;


   cstring = adb + " -s " + daddr + port +  " shell ls /system/xbin/busybox";
   command=RunProcess(cstring);


     if (command.contains("No such file or directory"))
      { QMessageBox::critical( this,"","Busybox required for SSH. Install from the Root Menu.");

         logfile(cstring);
         logfile(command);
         logfile("Busybox required for SSH.");
         return;
     }



   logfile("ssh installation query");

   QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "", "Install SSH?",
                                   QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::No)
        {logfile("ssh installation cancelled");
          return;}





      QElapsedTimer rtimer;
         int nMilliseconds;
         rtimer.start();


         ui->progressBar->setHidden(false);
         ui->progressBar->setValue(0);


         QTimer *timer = new QTimer(this);
         connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
         timer->start(tsvalue);


          logfile("starting ssh install");




   cstring = adb + " -s " + daddr + port + " push "+adbdir+ "/ssh.tar /sdcard/";
   command=RunProcess(cstring);


   if (!command.contains("bytes"))
      { QMessageBox::critical(
                   this,
                   "",
                   "SSH install failed");
       logfile("ssh.tar: ssh install failed ");
       logfile(cstring);
       logfile(command);
       ui->progressBar->setHidden(true);
       return;
      }

   mount_system("rw");

   cstring = adb + " -s " + daddr+port + " shell su -c tar xf /sdcard/ssh.tar -C /data";
   command=RunProcess(cstring);

   logfile(cstring);
   logfile(command);

cstring = adb + " -s " + daddr+port + " shell su -c rm /sdcard/ssh.tar";
command=RunProcess(cstring);

logfile(cstring);
logfile(command);


cstring = adb + " -s " + daddr+port + " shell su -c /data/jocala/ssh/setupssh";
command=RunProcess(cstring);

logfile(cstring);
logfile(command);


cstring = adb + " -s " + daddr + port + " shell sshstatus";
command=RunProcess(cstring);


if (command.contains("not found"))
   { QMessageBox::critical(
                this,
                "",
                "SSH not installed");
    logfile("ssh check");
    logfile(cstring);
    logfile(command);
    mount_system("ro");
    return;
   }

else

   { QMessageBox::information(
                this,
                "",
                "SSH installed");
    logfile("ssh check passed");
    logfile(cstring);
    logfile(command);
     cstring = adb + " -s " + daddr + port + " shell stopssh";
     command=RunProcess(cstring);

       }




nMilliseconds = rtimer.elapsed();
logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );

ui->progressBar->setHidden(true);
mount_system("ro");

}

////////////////////////////////////////////////////
void MainWindow::on_actionUninstall_SSH_triggered()
{

    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }


   if (!is_su())
      { QMessageBox::critical(
            this,
            "",
            "Root required!");
         return;
   }


   logfile("ssh uninstallation query");

   QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "", "Uninstall SSH?",
                                   QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::No)
        {logfile("ssh uninstall cancelled");
          return;}


   mount_system("rw");

   QString cstring;
   QString command;


   cstring = adb + " -s " + daddr + port + " shell /system/xbin/sshstatus";
   command=RunProcess(cstring);


   if (command.contains("not found"))
      { QMessageBox::critical(
                   this,
                   "",
                   "SSH not installed");
       logfile("ssh check");
       logfile(cstring);
       logfile(command);
       mount_system("ro");
       return;
      }

   if (command.contains("running"))
      {

       cstring = adb + " -s " + daddr + port + " shell su -c /data/jocala/sshstop";
       command=RunProcess(cstring);
       logfile("ssh check");
       logfile(cstring);
       logfile(command);


      }


       logfile("uninstall ssh");

       cstring = adb + " -s " + daddr + port + " shell su -c /data/jocala/rmssh";
       command=RunProcess(cstring);

       logfile("rm /system/xbin/ssh");
       logfile(cstring);
       logfile(command);

       cstring = adb + " -s " + daddr + port + " shell su -c rm -r /data/jocala/";
       command=RunProcess(cstring);

       logfile("rm /system/xbin/ssh");
       logfile(cstring);
       logfile(command);


       cstring = adb + " -s " + daddr + port + " shell su -c rm -r /system/etc/init.d/02sshd";
       command=RunProcess(cstring);

       logfile("rm /system/etc/init.d/02sshd");
       logfile(cstring);
       logfile(command);


       cstring = adb + " -s " + daddr + port + " shell sshstatus";
       command=RunProcess(cstring);




       if (command.contains("not found"))
          { QMessageBox::critical(
                       this,
                       "",
                       "SSH uninstalled");
           logfile("ssh check");
           logfile(cstring);
           logfile(command);




          }


     mount_system("ro");

}

/////////////////////////////////////////////////////////////////////////
void MainWindow::on_puttyButton_clicked()
{



    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }




    if (!is_su())
       { QMessageBox::critical(
             this,
             "",
             "Root required!");
          return;
    }


    QString cstring = "";
    QString command = "";

    cstring = adb + " -s " + daddr + port + " shell sshstatus";
    command=RunProcess(cstring);


    if (command.contains("not found"))
       { QMessageBox::critical(
                    this,
                    "",
                    "SSH not installed");


        return;
       }






    logfile("detaching console process");



    if (os == 1)

       {
        cstring = "./putty.exe -ssh root@"+daddr;
       QProcess::startDetached(cstring);
       }


     else

       {


         QString commstr = adbdir+"console.sh";
         QFile file(commstr);

             if(!file.open(QFile::WriteOnly |
                           QFile::Text))
             {



                 logfile("error creating console.sh!");
                 QMessageBox::critical(this,"","Error creating command file!");
                 return;
             }



             QTextStream out(&file);
             out  << "#!/bin/sh" << endl;
             out  <<  "ssh root@"+daddr  << endl;


             file.flush();
             file.close();

       cstring = "chmod 0755 " + commstr ;
       command=RunProcess(cstring);

      //  cstring = "gnome-terminal -e "+adbdir+"console.sh";
      //  cstring = "x-terminal-emulator -e "+adbdir+"console.sh";

       if (os == 0)
         cstring = "x-terminal-emulator -e "+adbdir+"console.sh";
       if (os == 2)
        cstring = "open -a Terminal.app "+adbdir+"console.sh";



        QProcess::startDetached(cstring);
        }


}

//////////////////////////////////////////////////////////////////////
void MainWindow::on_sftpButton_clicked()
{


    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }



    if (!is_su())
       { QMessageBox::critical(
             this,
             "",
             "Root required!");
          return;
    }



    QString cstring = "";
    QString command = "";

    cstring = adb + " -s " + daddr + port + " shell sshstatus";
    command=RunProcess(cstring);


    if (command.contains("not found"))
       { QMessageBox::critical(
                    this,
                    "",
                    "SSH not installed");


        return;
       }





    logfile("detaching ssh process");




    if (os == 1)

       {
        cstring = "./psftp.exe root@"+daddr;
       QProcess::startDetached(cstring);
       }


     else

       {


         QString commstr = adbdir+"sftp.sh";
         QFile file(commstr);

             if(!file.open(QFile::WriteOnly |
                           QFile::Text))
             {



                 logfile("error creating sftp.sh!");
                 QMessageBox::critical(this,"","Error creating command file!");
                 return;
             }



             QTextStream out(&file);
             out  << "#!/bin/sh" << endl;
             out  << "sftp root@"+daddr  << endl;


             file.flush();
             file.close();

       cstring = "chmod 0755 " + commstr ;
      command=RunProcess(cstring);



       if (os == 0)
         cstring = "x-terminal-emulator -e "+adbdir+"sftp.sh";
       if (os == 2)
        cstring = "open -a Terminal.app "+adbdir+"sftp.sh";



        QProcess::startDetached(cstring);
        }


}

void MainWindow::on_actionView_Log_triggered()
{


    logfile("opening adblog dialog");

     adblogDialog dialog;
     dialog.setModal(true);
     dialog.exec();


}

////////////////////////////////////////////////
void MainWindow::on_actionSwap_data_triggered()
{



    QString cstring;
    QString command;
    QString uuid;
    QString fileName;
    bool worked = false;

    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }


   if (!is_su())
      { QMessageBox::critical(
            this,
            "",
            "Root required!");
         return;
   }


 logfile("swap /data to external ext4 drive");



    logfile("swap routine started");


          logfile("looking for drives");

          cstring = adb + " shell su -c blkid /dev/block/sd* | grep ext4";
          command=RunProcess(cstring);

          if (command.isEmpty())
             { QMessageBox::critical(this,"","No drives found");

              logfile(cstring);
              logfile("no drives found");
              return;
              }


          logfile(cstring);
          logfile(command);

          QFile file21(adbdir+"temp.txt");

            if(!file21.open(QFile::WriteOnly |
                          QFile::Text))
            {
                QMessageBox::critical(this,"","Error creating drive file!");

                logfile(cstring);
                logfile(command);
                logfile("error creating "+adbdir+ "drives temp.txt");
                return;
            }


            QTextStream out1(&file21);
            out1  << command << endl;

            file21.flush();
            file21.close();



          usbfileDialog sddialog;
          sddialog.setModal(true);
          sddialog.setData("Select drive");
          if(sddialog.exec() == QDialog::Accepted)
          fileName = sddialog.binfileName();
          else return;


          if (fileName.isEmpty())
             {
              QMessageBox::critical(this,"","No drive selected");
              logfile("no drive selected");
              return;
              }


          QStringList driveElements = fileName.split(" ");

          int r = driveElements.size();

          if (r == 4)
            uuid = driveElements[2];

          if (r == 3)
            uuid = driveElements[1];

          if (r < 3 || r > 4)
           { QMessageBox::critical(this,"","Unknown problem with this drive\nPlease check format");
             return; }


          QMessageBox::StandardButton reply2;
            reply2 = QMessageBox::question(this, "", "Drive "+uuid+" selected.\n\nProceed with /data swap prep for this drive? This may be a lengthy process, depending on the amount of material on your Fire TV /data partition. \n\nA popup message will appear when prep is finished",
                                          QMessageBox::Ok|QMessageBox::Cancel);
            if (reply2 == QMessageBox::No)
                return;


            QElapsedTimer rtimer;
            int nMilliseconds;
            rtimer.start();


            ui->progressBar->setHidden(false);
            ui->progressBar->setValue(0);

            QTimer *timer = new QTimer(this);
            connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
            timer->start(tsvalue);


         mount_system("rw");

         cstring = adb + " shell su -c rm -r /system/temp";
         command=RunProcess(cstring);

         cstring = adb + " shell su -c mkdir /system/temp";
         command=RunProcess(cstring);

         logfile(cstring);
         logfile(command);

         cstring = adb + " shell su -c busybox mount "+uuid+ " /system/temp";
         command=RunProcess(cstring);

         logfile(cstring);
         logfile(command);

         cstring = adb + " shell su -c cp -av /data/. /system/temp/";
         command=RunProcess(cstring);

          logfile(cstring);
          logfile(command);

          cstring = adb + " shell su -c ls /system/temp/data/data";
          command=RunProcess(cstring);

          if (command.contains("No such file or directory"))
           worked = true;
           else
           worked = false;

           logfile(cstring);
           logfile(command);

          cstring = adb + " shell su -c busybox umount /system/temp/";
          command=RunProcess(cstring);

           logfile(cstring);
           logfile(command);

           cstring = adb + " shell su -c rm -r /system/temp/";
           command=RunProcess(cstring);

            logfile(cstring);
            logfile(command);


          cstring = adb + " shell su -c cp /system/xbin/mntdata.orig /system/xbin/mntdata.sh";
          command=RunProcess(cstring);

          logfile(cstring);
          logfile(command);

          cstring = adb + " shell su -c sed -i  's/fakeuuid/"+uuid+"/g' /system/xbin/mntdata.sh";
          command=RunProcess(cstring);

          logfile(cstring);
          logfile(command);


           ui->progressBar->setHidden(true);

      if (worked)
        {
           QMessageBox::StandardButton reply3;
             reply3 = QMessageBox::question(this, "", "Drive "+uuid+ "has been prepared to become /data when you reboot.\n\nWhen you reboot, you may see your screen go white or pixelated. This is temporary and does not not harm your devices.\n\nShould the reboot seem to go awry, simply disconnect the /data drive and reboot normally.\n\nReboot now?",
                                           QMessageBox::Yes|QMessageBox::No);
             if (reply3 == QMessageBox::Yes)
                {
                   ui->device_connected->setText(devstr2);
                   mount_system("ro");
                   rebootDevice(" reboot");
                 }
        }

      else

       {

          QMessageBox::critical(this,"","Drive not properly prepared. Check logs and examine drive.");

      }

          mount_system("ro");


          nMilliseconds = rtimer.elapsed();
          logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );



          return;




    }


/////////////////////////////////////////////////////////
void MainWindow::on_actionUnlock_Bootloader_triggered()
{


    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }


   if (!is_su())
      { QMessageBox::critical(
            this,
            "",
            "Root required!");
         return;
   }



    QString cstring = adb + " -s " + daddr+port + " shell su -c /system/xbin/aftv-unlock unlock";
    QString command=RunProcess(cstring);

         if (command.contains("All done"))
            {
             QMessageBox::information(this,"","Bootloader unlocked");
             }

         else

         {
             QMessageBox::critical(this,"","Bootloader NOT unlocked");
         }

         logfile(cstring);
         logfile(command);


}

////////////////////////////////////////////////////
void MainWindow::on_actionLock_Bootloader_triggered()
{


    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }


   if (!is_su())
      { QMessageBox::critical(
            this,
            "",
            "Root required!");
         return;
   }



    QString cstring = adb + " -s " + daddr+port + " shell su -c /system/xbin/aftv-unlock lock";
    QString command=RunProcess(cstring);

         if (command.contains("All done"))
            {
             QMessageBox::information(this,"","Bootloader relocked");
             }

         else

         {
             QMessageBox::critical(this,"","Bootloader NOT relocked");
         }

         logfile(cstring);
         logfile(command);


}


/////////////////////////////////////////////////////////
void MainWindow::on_actionBuild_mount_script_triggered()
{


    QMessageBox::information(this,"","Program stub");
    return;


    QString cstring;
    QString command;
    QString uuid;
    QString dtype;
    QString fileName;
    // bool worked = false;

    if (!isConnected)
       { QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Device not connected"));
          return;
    }


   if (!is_su())
      { QMessageBox::critical(
            this,
            "",
            "Root required!");
         return;
   }


 logfile("build mount script");

    logfile("swap routine started");


          logfile("looking for drives");

          cstring = adb + " shell su -c blkid /dev/block/sd*";
          command=RunProcess(cstring);

          if (command.isEmpty())
             { QMessageBox::critical(this,"","No drives found");

              logfile(cstring);
              logfile("no drives found");
              return;
              }


          logfile(cstring);
          logfile(command);

          QFile file21(adbdir+"temp.txt");

            if(!file21.open(QFile::WriteOnly |
                          QFile::Text))
            {
                QMessageBox::critical(this,"","Error creating drive file!");

                logfile(cstring);
                logfile(command);
                logfile("error creating "+adbdir+ "drives temp.txt");
                return;
            }


            QTextStream out1(&file21);
            out1  << command << endl;

            file21.flush();
            file21.close();

         bool doloop = true;
         int i = 0;

          uuidDialog sddialog;
          sddialog.setModal(true);
          sddialog.setData1("Build UUID Mount Script");

          do
          {

          if(sddialog.exec() == QDialog::Accepted)
          fileName = sddialog.uuidName();
          else return;


          if (fileName.isEmpty())
             {
              QMessageBox::critical(this,"","No drive selected");
              logfile("no drive selected");
              doloop = false;
              return;
              }


          QStringList driveElements = fileName.split(" ");

          int r = driveElements.size();

          if (r == 4)
            {
              uuid = driveElements[2];
              dtype = driveElements[3];
             }

          if (r == 3)
           {
              uuid = driveElements[1];
              dtype = driveElements[2];
            }




          if (r < 3 || r > 4)
           { QMessageBox::critical(this,"","Unknown problem with this drive\nPlease check format");
             return; }


          QString mountpoint = QInputDialog::getText(this, "Mount point",
                                                   "/storage/usb/", QLineEdit::Normal
                                                   );
            i = i + +1;

            }
            while (doloop);

        //  QMessageBox::StandardButton reply2;
         //   reply2 = QMessageBox::question(this, "", "Drive "+uuid+" selected. Enter ",
          //                                QMessageBox::Ok|QMessageBox::Cancel);
       //     if (reply2 == QMessageBox::No)
         //       return;


          //  QElapsedTimer rtimer;
          //  int nMilliseconds;
       //     rtimer.start();


       //  mount_system("ro");




       //   nMilliseconds = rtimer.elapsed();
      //    logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );



       //   return;



}




void MainWindow::on_actionConsole_triggered()
{


    // set PATH=%PATH%;C:\xampp\php

    logfile("detaching console process");

     QString cstring = "";


     if (os == 1)

        {
        cstring = "cmd /k cd " + adbdir;
        QProcess::startDetached(cstring);
        }



       if (os == 2)
         {
         QString cstring = "/Applications/Utilities/Terminal.app/Contents/MacOS/Terminal " + adbdir;
         QProcess::startDetached(cstring);
         }

       if (os == 0)
        {
           cstring = "x-terminal-emulator--working-directory="+adbdir;
           QProcess::startDetached(cstring);
         }
}
