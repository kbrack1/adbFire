#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog2.h"
#include "helpdialog.h"
#include "uninstalldialog.h"
#include "llamadialog.h"
#include "usbfiledialog.h"
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

// #include <QDebug>



#ifdef Q_OS_LINUX
 int os=0;
#elif defined(Q_OS_WIN)
  int os=1;
#elif defined(Q_OS_MAC)
int os=2;
#endif

const QString version = "1.05";

bool isConnected = false;
bool serverRunning = false;
bool  is_packageInstalled = false;
bool  mounted_op = false;
bool firstrun=true;
bool dbexists = false;
bool updatecheck = true;
bool versioncheck = true;

QString port = ":5555";
QString filename = "";
QString adbdir =  "";
QString adb = "";
QString xmldir = "";
QString splashdir = "";
QString hdir = "";
QString daddr="";
QString sldir = "";
QString pushdir = "";
QString pulldir = "";
QString xbmcpackage ="";
QString dbstring = "";



QString adbstr1 = "ADB running. ";
QString adbstr2 = "ADB not running. ";
QString devstr1 = "  Device connected";
QString devstr2 = "  Device not connected.";


int sshcheck;
int usbcheck;
int ftvupdate;
int checkversion;

int tsvalue = 4000;

QSqlDatabase db;

//////////////////////////////////////////////
void rotate_logfile()

{

 QFile file(adbdir+"adbfire.log.old");

 if( file.exists() )
     QFile::remove(adbdir+"adbfire.log.old");


QFile file2(adbdir+"adbfire.log");

if( file2.exists() )
    file2.rename(adbdir+"adbfire.log.old");


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
 run_command.waitForFinished(-1);
 QString command=run_command.readAll();
 return command;
}


///////////////////////////////////////////////
QString RunProcess2(QString cstring)
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

    QString sqlstatement = "create table device(id int primary key, name varchar(20),sldir varchar(100),pushdir varchar(100),pulldir varchar(100), xbmcpackage varchar(50) , usbcheck int, ftvupdate int, versioncheck int)";

    QSqlQuery query;
    query.exec(sqlstatement);

    if (query.lastError().isValid())
     {
        logfile(sqlstatement);
        logfile("SqLite error:" + query.lastError().text());
        logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
       }


    sqlstatement="insert into device values(1, '','"+hdir+"','"+hdir+"','"+hdir+"' ,'org.xbmc.xbmc',0,1,1 )";
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



    sqlstatement = "UPDATE device SET usbcheck='"+str1+"' WHERE Id=1";
     query.exec(sqlstatement);


     if (query.lastError().isValid())
      {
        logfile(sqlstatement);
        logfile("SqLite error:" + query.lastError().text());
        logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
       }


    sqlstatement = "UPDATE device SET ftvupdate='"+str2+"' WHERE Id=1";
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



            sqlstatement="SELECT usbcheck FROM device";
            query.exec(sqlstatement);
            while (query.next()) {
                  usbcheck = query.value(0).toInt();
            }


            if (query.lastError().isValid())
             {
               logfile(sqlstatement);
               logfile("SqLite error:" + query.lastError().text());
               logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
              }


            sqlstatement="SELECT ftvupdate FROM device";
            query.exec(sqlstatement);
            while (query.next()) {
                  ftvupdate = query.value(0).toInt();
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

     if (ftvupdate==0)
         updatecheck=false;
     else
         updatecheck=true;

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




adbdir = QCoreApplication::applicationDirPath();
adbdir = adbdir+"/adbfiles/";

QString command = "";

     ui->setupUi(this);


    this->setFixedSize(this->size().width(), this->size().height());

     QPixmap pix(":/assets/donate.png");
     QIcon icon(pix);
     ui->donate->setIcon(icon);
     ui->donate->setIconSize(pix.size());

           if (usbcheck == 1)
               ui->usbBox->setChecked(true);
           else
              ui->usbBox->setChecked(false);


    if(isConnected)
       {
         ui->device_connected->setText(devstr1);
         ui->usbBox->setDisabled(false);
        }

    else

    {
     ui->device_connected->setText(devstr2);
     ui->usbBox->setDisabled(true);
    }



    ui->statusBar->addPermanentWidget(ui->server_running);
    ui->statusBar->addPermanentWidget(ui->device_connected);
    ui->progressBar->setHidden(true);
    ui->statusBar->addPermanentWidget( ui->progressBar);


    dbstring = adbdir+"adbfire.db";
    xmldir = adbdir+"remotes/";
    splashdir = adbdir+"splash/";

 if (os == 1)
     adb = adbdir+"adb.exe";
  else
     adb = adbdir+"adb";


  // QMessageBox::critical(this,"",adb);

  rotate_logfile();


  logfile("adbFire logfile");

  hdir = QDir::homePath();
  open_pref_database();

   if (!dbexists)
    createTables();

   readTables();



   if (usbcheck == 1)
       ui->usbBox->setChecked(true);
   else
      ui->usbBox->setChecked(false);

   ui->device->setText(daddr);

    if (!(os == 1))
      {  QString cstring = "chmod 0755 "+adb;
         QString command=RunProcess(cstring);
       }

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
     ui->server_running->setText(adbstr2);


    if (checkversion==1)
         get_data();

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
  ui->progressBar->setValue(value+1);
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

          QString command=RunProcess2(cstring);

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

                      QString command=RunProcess2(cstring);


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
         ui->device_connected->setText("  Device connected.");
         ui->server_running->setText(adbstr1);
         serverRunning = true;
         ui->usbBox->setDisabled(false);
     }
       else
        { ui->device_connected->setText("  Device not connected.");
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
            "XBMC not installed");
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
       QString command=RunProcess2(cstring);

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

                              command=RunProcess2(cstring);
                                     
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


 QString cname = ui->comboBox->currentText();

switch(ui->comboBox->currentIndex() ){
case 0:
xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/addons/";
break;

case 1:
xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/userdata/keymaps";
break;

case 2:
xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/media/";
break;

case 3:
xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/sounds/";
break;

case 4:
xpath = "/sdcard/Android/data/org.xbmc.xbmc/files/.xbmc/system/";
break;

case 5:
xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/userdata/";
break;

case 6:
xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/temp/";
break;

case 7:
xpath = "/sdcard/";
break;

default:
xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/addons/";
break;
}


is_package(xbmcpackage);

if (xpath != "/sdcard/")
   if (!is_packageInstalled)
     { QMessageBox::critical(
           this,
           "",
           "XBMC not installed");
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
                      "Destination path missing. Has XBMC had its first run to set up internal folders?");
          logfile(xpath);
          logfile("Destination path missing. Has XBMC had its first run to set up internal folders?");
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

           command=RunProcess2(cstring);

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
            "XBMC not installed");
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
   reply = QMessageBox::question(this, "Restore", "Restore XBMC from "+dir+"\n"+"\n"+"This will overwrite the existing setup!",
                                 QMessageBox::Yes|QMessageBox::No);
   if (reply == QMessageBox::Yes) {


       ui->progressBar->setHidden(false);
       ui->progressBar->setValue(0);

       QTimer *timer = new QTimer(this);
       connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
       timer->start(tsvalue);

       cstring = adb + " -s " + daddr + port + " shell rm -r "+xpath +xbmcpackage;

       command=RunProcess2(cstring);

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
            "XBMC not installed");
         return;
   }


QString  xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/userdata/keymaps/";

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
       cstring = "cmd /k " +   adbdir + "adb.exe -s "  + daddr + port + " shell";
       QProcess::startDetached(cstring);
       }


     if (os == 2)
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

        cstring = "open -a Terminal.app "+adbdir+"console.sh";
        QProcess::startDetached(cstring);
        }


}

//////////////////////////////////////////////////
void MainWindow::on_actionPreferences_triggered()
{

    logfile("opening preferences dialog");

    bool currentupdate = updatecheck;

    preferencesDialog dialog;
    dialog.setPackagename(xbmcpackage);
    dialog.setPulldir(pulldir);
    dialog.setftvUpdate(updatecheck);
    dialog.setversioncheck(versioncheck);
    dialog.setversionLabel(version);

    dialog.setModal(true);


    if(dialog.exec() == QDialog::Accepted)
    {

    xbmcpackage = dialog.xbmcpackageName();
    pulldir = dialog.pulldir();


    updatecheck = dialog.updatecheck();
    versioncheck = dialog.versioncheck();


    if (versioncheck)
        checkversion = 1;
    else
        checkversion = 0;





  if (isConnected)
  {


if (updatecheck != currentupdate) //  has update pref changed
{

     logfile("update preference changed.");

    if ( is_package("eu.chainfire.supersu"))
     {

    if (updatecheck)
     {
       if (amazon_updates("enable"))
         {QMessageBox::information(this,"","Amazon updates on\ncom.amazon.dcp enabled");
           logfile("Amazon updates on, com.amazon.dcp enabled");
        }


       else
        {QMessageBox::critical(this,"","Problem: com.amazon.dcp not enabled!");
        logfile("Problem: com.amazon.dcp not enabled!");
       }
    }

    else
    {
        if (amazon_updates("disable"))
           {QMessageBox::information(this,"","Amazon updates off\ncom.amazon.dcp disabled");
            logfile("Amazon updates off, com.amazon.dcp disabled");
             }
      else
         { QMessageBox::critical(this,"","Problem: com.amazon.dcp not disabled!");
            logfile("Problem: com.amazon.dcp not disabled!");
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

    is_package("eu.chainfire.supersu");

   if (!is_packageInstalled)
      { QMessageBox::critical(
            this,
            "",
            "Root required!");
         return;
   }



   QMessageBox::StandardButton reply;
     reply = QMessageBox::question(this, "Reboot Device", "Reboot Device?",
         QMessageBox::Yes|QMessageBox::No);
     if (reply == QMessageBox::Yes) {
         isConnected=false;
         ui->device_connected->setText(devstr2);
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

    is_package("eu.chainfire.supersu");

   if (!is_packageInstalled)
      { QMessageBox::critical(
            this,
            "",
            "Root required!");
         return;
   }


   QMessageBox::StandardButton reply;
     reply = QMessageBox::question(this, "", "Reboot Recovery?",
         QMessageBox::Yes|QMessageBox::No);
     if (reply == QMessageBox::Yes) {
         isConnected=false;
         ui->device_connected->setText(devstr2);
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


    is_package("eu.chainfire.supersu");

   if (!is_packageInstalled)
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

    QString busybox1  = adbdir+"binstall.sh";
    QString busybox2 = adbdir+"buninstall.sh";
    QString busybox3 = adbdir+"busybox";
    QString busybox4 = adbdir+"ntfs-3g";
    QString busybox5 = adbdir+"mount.exfat-fuse";
    QString busybox6 = adbdir+"mntdrives.sh";
    QString busybox7 = adbdir+"samba.tar";

        bool file1 = false;
        bool file2 = false;
        bool file3 = false;
        bool file4 = false;
        bool file5 = false;
        bool file6 = false;
        bool file7 = false;

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


      QFile Fout6(busybox6);

      if(!Fout6.exists())
         {

           QMessageBox::critical(
            this,
            tr("adbFire"),
             busybox6+" not found.");
           logfile(busybox6+" not found.");
           ui->progressBar->setHidden(true);
            return;
          }


      QFile Fout7(busybox7);

      if(!Fout7.exists())
         {

           QMessageBox::critical(
            this,
            tr("adbFire"),
             busybox7+" not found.");
             logfile(busybox7+" not found.");
           ui->progressBar->setHidden(true);
            return;
          }


      QString cstring = adb + " -s " + daddr + port + " push "+busybox1+ " /sdcard/";

      QString command=RunProcess2(cstring);

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
            else
            file1 = true;


         logfile(cstring);
         logfile(command);

              cstring = adb + " -s " + daddr + port + " push "+busybox2+ " /sdcard/";
              command=RunProcess2(cstring);


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
              else
                  file2 = true;

              logfile(cstring);
              logfile(command);


               cstring = adb + " -s " + daddr + port + " push "+busybox3+ " /sdcard/";
               command=RunProcess2(cstring);

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
               else
                   file3 = true;


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
                else
                    file4 = true;


                logfile(cstring);
                logfile(command);

                 cstring = adb + " -s " + daddr + port + " push "+busybox5+ " /sdcard/";

                 command=RunProcess2(cstring);

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
                 else
                     file5 = true;

                 logfile(cstring);
                 logfile(command);

                  cstring = adb + " -s " + daddr + port + " push "+busybox6+ " /sdcard/";
                  command=RunProcess2(cstring);

                  if (!command.contains("bytes"))
                     { QMessageBox::critical(
                                  this,
                                  "",
                                  "file6: busybox install failed");

                      logfile("file6: busybox install failed ");
                      logfile(cstring);
                      logfile(command);
                      ui->progressBar->setHidden(true);
                      return;
                     }
                  else
                      file6 = true;

                  logfile(cstring);
                  logfile(command);

                  cstring = adb + " -s " + daddr + port + " push "+busybox7+ " /sdcard/";
                  command=RunProcess2(cstring);

                  if (!command.contains("bytes"))
                     { QMessageBox::critical(
                                  this,
                                  "",
                                  "file7: busybox install failed");

                      logfile("file7: busybox install failed ");
                      logfile(cstring);
                      logfile(command);
                      ui->progressBar->setHidden(true);
                      return;
                     }
                  else
                      file7 = true;

                  logfile(cstring);
                  logfile(command);



 if (file1 && file2 && file3 && file4 && file5 && file6 && file7)

 {

      mount_system("rw");


     QString cstring = adb + " -s " + daddr+port + " shell su -c cp " + " /sdcard/binstall.sh /system/xbin";
     command=RunProcess2(cstring);
     
     logfile(cstring);
     logfile(command);

     cstring = adb + " -s " + daddr+port + " shell su -c cp " + " /sdcard/buninstall.sh /system/xbin";
     command=RunProcess2(cstring);
     
     logfile(cstring);
     logfile(command);

     cstring = adb + " -s " + daddr+port + " shell su -c cp " + " /sdcard/busybox /system/xbin";
     command=RunProcess2(cstring);
     
     logfile(cstring);
     logfile(command);

     cstring = adb + " -s " + daddr+port + " shell su -c cp " + " /sdcard/ntfs-3g /system/xbin";
     command=RunProcess2(cstring);

     logfile(cstring);
     logfile(command);

     cstring = adb + " -s " + daddr+port + " shell su -c cp " + " /sdcard/mount.exfat-fuse /system/xbin";
     command=RunProcess2(cstring);

     logfile(cstring);
     logfile(command);

     cstring = adb + " -s " + daddr+port + " shell su -c cp " + " /sdcard/mntdrives.sh /system/xbin";
     command=RunProcess2(cstring);

     logfile(cstring);
     logfile(command);


     cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 /system/xbin/binstall.sh";
     command=RunProcess2(cstring);

     logfile(cstring);
     logfile(command);

     cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 /system/xbin/buninstall.sh";
     command=RunProcess2(cstring);

     logfile(cstring);
     logfile(command);

     cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 /system/xbin/busybox";
     command=RunProcess2(cstring);

     logfile(cstring);
     logfile(command);

     cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 /system/xbin/ntfs-3g";
     command=RunProcess2(cstring);

     logfile(cstring);
     logfile(command);

     cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 /system/xbin/mount.exfat-fuse";
     command=RunProcess2(cstring);

     logfile(cstring);
     logfile(command);

     cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 /system/xbin/mntdrives.sh";
     command=RunProcess2(cstring);

     logfile(cstring);
     logfile(command);

     cstring = adb + " -s " + daddr+port + " shell su -c /system/xbin/binstall.sh";
     command=RunProcess2(cstring);

     logfile(cstring);
     logfile(command);
     
      cstring = adb + " -s " + daddr + port +  " shell ls /system/xbin/which";
      command=RunProcess2(cstring);
      
      logfile(cstring);
      logfile(command);

        if (command.contains("No such file or directory"))
          { logfile("busybox install failed!");
            QMessageBox::critical( this,"","Busybox not installed!");
        }

        else
        {




         cstring = adb + " -s " + daddr+port + " shell su -c tar -xf /sdcard/samba.tar -C /data/data";
         command=RunProcess2(cstring);

         logfile(cstring);
         logfile(command);

         cstring = adb + " -s " + daddr+port + " shell su -c rm /sdcard/samba.tar";
         command=RunProcess2(cstring);

         logfile(cstring);
         logfile(command);


         QMessageBox::information( this,"","Busybox installed!");
         logfile("busybox installed!");
        }

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


    is_package("eu.chainfire.supersu");

   if (!is_packageInstalled)
      { QMessageBox::critical(
            this,
            "",
            "Root required!");
         return;
   }



   QString command;
   QString cstring;

   QString umntstring = "/system/xbin/umount /storage/usb/sd*/";
   QString rmsd = "rm -r /storage/usb/sd*/";
   QString rmsam = "rm -r /data/data/com.funkyfresh.samba/";
   QString rmsh = "rm /system/etc/install-recovery-2.sh";


   cstring = adb + " -s " + daddr + port +  " shell ls /system/xbin/busybox";
   command=RunProcess2(cstring);

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

         cstring = adb + " -s " + daddr+port + " shell su -c ls /storage/usb/sd\?\?/";
         command=RunProcess2(cstring);

       if (!command.contains("No such file or directory"))

        {

             logfile(cstring);
             logfile(command);

             mount_root("rw");



             cstring = adb + " -s " + daddr+port + " shell su -c /data/data/com.funkyfresh.samba/files/samba-rc stop";
             command=RunProcess2(cstring);;

             logfile(cstring);
             logfile(command);

             cstring = adb + " -s " + daddr+port + " shell su -c " + rmsam;
             command=RunProcess2(cstring);;

             logfile(cstring);
             logfile(command);

         cstring = adb + " -s " + daddr+port + " shell su -c " + rmsh;
         command=RunProcess2(cstring);;

         logfile(cstring);
         logfile(command);



          cstring = adb + " -s " + daddr+port + " shell su -c " + umntstring;
          command=RunProcess2(cstring);;

          logfile(cstring);
          logfile(command);

          cstring = adb + " -s " + daddr+port + " shell su -c " + rmsd;
          command=RunProcess2(cstring);

          logfile(cstring);
          logfile(command);

          mount_root("ro");

      }



        mount_system("rw");


         cstring = adb + " -s " + daddr+port + " shell su -c /system/xbin/buninstall.sh";
         command=RunProcess2(cstring);

         logfile(cstring);
         logfile(command);

         cstring = adb + " -s " + daddr + port +  " shell ls /system/xbin/busybox";
         command=RunProcess2(cstring);
         
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

     switch(ui->comboBox->currentIndex() ){
     case 0:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/addons/";
     break;

     case 1:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/userdata/keymaps/";
     break;

     case 2:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/media/";
     break;

     case 3:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/sounds/";
     break;

     case 4:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/system/";
     break;

     case 5:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/userdata/";
     break;

     case 6:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/temp/";
     break;

     case 7:
     xpath = "/sdcard/";
     break;

     default:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/addons/";
     break;
     }



  is_package(xbmcpackage);

  if (xpath != "/sdcard/")
     if (!is_packageInstalled)
       { QMessageBox::critical(
             this,
             "",
             "XBMC not installed");
         logfile(xpath);
         logfile("xbmc not installed, path not found");
          return;
       }



  QString cstring;
  QString pullfile;


  cstring = adb + " shell su -c find " +xpath+ " -type f ";
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



             cstring = adb + " -s " + daddr + port +  " shell rm "+pullfile;
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




    is_package("eu.chainfire.supersu");

   if (!is_packageInstalled)
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
    QString command=RunProcess(cstring);

    logfile(cstring);
    logfile(command);

    if (!command.isEmpty())
     { logfile("chmod error. cancelling firmware installation");
        QMessageBox::information( this,"","Error: "+command);
        return; }


    cstring = adb + " -s " + daddr+port + " shell su -c chmod 777 /cache/recovery";
    command=RunProcess(cstring);

    logfile(cstring);
    logfile(command);

    if (!command.isEmpty())
     {  logfile("chmod error. cancelling firmware installation");
        QMessageBox::information( this,"","Error: "+command);
        return; }


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
        command=RunProcess2(cstring);

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

    command=RunProcess2(cstring);

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


    is_package("eu.chainfire.supersu");

   if (!is_packageInstalled)
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
   QString command=RunProcess2(cstring);


     if (command.contains("No such file or directory"))
      { QMessageBox::critical( this,"","Busybox required for USB drive. Install it from the menu.");

         logfile(cstring);
         logfile(command);
         logfile("Busybox required for USB drive. Install it from the menu.");
         return;
     }



       cstring = adb + " -s " + daddr+port + " shell su -c /system/xbin/mntdrives.sh";
       command=RunProcess2(cstring);

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
           command=RunProcess2(cstring);

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


    is_package("eu.chainfire.supersu");

   if (!is_packageInstalled)
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


    QString umntstring = "/system/xbin/umount /storage/usb/sd*/";
    QString rmsd = "rm -r /storage/usb/sd*/";


    mount_root("rw");

       logfile("stopping samba");
       cstring = adb + " -s " + daddr+port + " shell su -c /data/data/com.funkyfresh.samba/files/samba-rc stop";
       command=RunProcess(cstring);

       logfile(cstring);
       logfile(command);

       cstring = adb + " -s " + daddr+port + " shell su -c " + umntstring;
       command=RunProcess(cstring);



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


    is_package("eu.chainfire.supersu");

   if (!is_packageInstalled)
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


    is_package("eu.chainfire.supersu");

   if (!is_packageInstalled)
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


    is_package("eu.chainfire.supersu");

   if (!is_packageInstalled)
      { QMessageBox::critical(
            this,
            "",
            "Root required!");
         return;
   }

QString cstring;
QString hashbang = "#!/system/bin/sh";
QString filename = adbdir+"install-recovery-2.sh";
QString makepst = "/system/xbin/mntdrives.sh";

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

        QFile file(filename);

          if(!file.open(QFile::WriteOnly))
                  {

                     logfile(cstring);
                     logfile(command);
                     logfile("error creating sh script on PC!");
                      QMessageBox::critical(this,"","Error creating file on PC!");
                      return;
                  }


        QTextStream out(&file);
        out << hashbang << endl << makepst << endl;
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



        cstring = adb + " -s " + daddr+port + " shell su -c cp " + " /sdcard/install-recovery-2.sh /system/etc/";
        QString command=RunProcess(cstring);

        if (!command.isEmpty())
             {

               logfile(cstring);
               logfile(command);
               logfile("file copy error, sdcard to system/etc");

               QMessageBox::critical(this,"","Error: cp /sdcard/install-recovery-2.sh /system/etc/ failed");
                mount_system("ro");
                return;
              }


        logfile(cstring);
        logfile(command);

        cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 " + " /system/etc/install-recovery-2.sh";
        command=RunProcess(cstring);

        if (!command.isEmpty())
             {

            logfile(cstring);
            logfile(command);
            logfile("chmod error, system/etc/install-recovery-2.sh");

                QMessageBox::critical(this,"","Error: chmod of install-recovery-2.sh failed");
                mount_system("ro");
                return;
              }


        logfile(cstring);
        logfile(command);

        cstring = adb + " -s " + daddr+port + " shell rm " + " /sdcard/install-recovery-2.sh";
       command=RunProcess(cstring);

        if (!command.isEmpty())
             {

            logfile(cstring);
            logfile(command);
            logfile("error: rm system/etc/install-recovery-2.sh");

              QMessageBox::critical(this,"","Error: rm of /sdcard/install-recovery-2.sh failed");
              }

         QFile::remove(filename);

         QFile Fout(filename);

         if(Fout.exists())
         {

             logfile(cstring);
             logfile(command);
             logfile("error: rm PC: install-recovery-2.sh");

             QMessageBox::critical(this,"","Error: rm of PC: install-recovery-2.sh failed");
         }


        cstring = adb + " -s " + daddr + port +  " shell ls /system/etc/install-recovery-2.sh";
        command=RunProcess(cstring);

        if (!command.contains("/system/etc/install-recovery-2.sh"))
             {

            logfile(cstring);
            logfile(command);
            logfile("Error: /etc/install-recovery-2.sh not created. USB drive is not persistent");

            QMessageBox::critical(this,"","Error: /etc/install-recovery-2.sh not created. USB drive is not persistent");
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

        cstring = adb + " -s " + daddr + port +  " shell su -c rm /system/etc/install-recovery-2.sh";
        command=RunProcess(cstring);

        QMessageBox::information(this,"","USB drive is not persistent.");
        mount_system("ro");

        logfile(cstring);
        logfile(command);
        logfile("USB drive is not persistent");

        return;

         }


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

     switch(ui->comboBox->currentIndex() ){
     case 0:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/addons/";
     break;

     case 1:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/userdata/keymaps/";
     break;

     case 2:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/media/";
     break;

     case 3:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/sounds/";
     break;

     case 4:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/system/";
     break;

     case 5:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/userdata/";
     break;

     case 6:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/temp/";
     break;

     case 7:
     xpath = "/sdcard/";
     break;


     default:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/addons/";
     break;
     }

     is_package(xbmcpackage);

     if (xpath != "/sdcard/" )
        if (!is_packageInstalled)
          { QMessageBox::critical(
                this,
                "",
                "XBMC not installed");

            logfile("xbmc not installed");
            logfile("install xbmc for "+xpath);

             return;
          }

     cstring = adb + " shell su -c find " +xpath+ " -type f ";
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


             cstring = adb + " -s " + daddr + port +  " pull "+fileName+" "+pulldir;
             command=RunProcess2(cstring);


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
            "XBMC not installed");
         return;
   }


QString  xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/media/";

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




 QString llama = adbdir+"llama.apk";
 QString command;
 QString cstring;
 QString icontype;
 QString s = "";

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

 is_package("com.adrise.profilms");
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
           QMessageBox::information(this,"","Please install Classic TV");
           ui->progressBar->setHidden(true);
           return;
         }


       if (!classicTV && llamaInstall)
         if (llamaEvent > 1)
          {
           QMessageBox::information(this,"","Please install Classic TV");
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
           command=RunProcess2(cstring);

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
               cstring = adb + " shell su -c mkdir -p /data/data/com.kebab.Llama/shared_prefs";
               command=RunProcess2(cstring);
               logfile(cstring);
               logfile(command);
           }

       }



     if (llamaEvent < 4 && isLlama)
     {

         if (llamaEvent == 1)
         {
          cstring = adb + " push "+adbdir+"llama.xbmc.boot /sdcard/EVENTS.xml";
          command=RunProcess2(cstring);
          logfile(cstring);
          logfile(command);
          }


       if (llamaEvent == 2)
         {
           cstring = adb + " push "+adbdir+"llama.ctv.only /sdcard/EVENTS.xml";
           command=RunProcess2(cstring);
           logfile(cstring);
           logfile(command);
         }


       if (llamaEvent == 3)
         {

           cstring = adb + " push "+adbdir+"llama.ctvx.boot /sdcard/EVENTS.xml";
           command=RunProcess2(cstring);
           logfile(cstring);
           logfile(command);
         }


       cstring =  adb + " shell su -c cp /sdcard/EVENTS.xml /data/data/com.kebab.Llama/shared_prefs";
       command=RunProcess2(cstring);
       logfile(cstring);
       logfile(command);


       cstring =  adb + " shell su -c chown -R install.install  /data/data/com.kebab.Llama/shared_prefs";
       command=RunProcess2(cstring);
       logfile(cstring);
       logfile(command);


       cstring =  adb + " shell su -c chown -R install.install  /data/data/com.kebab.Llama/shared_prefs/*";
       command=RunProcess2(cstring);
       logfile(cstring);
       logfile(command);

       cstring =  adb + " shell rm /sdcard/EVENTS.xml";
       command=RunProcess2(cstring);
       logfile(cstring);
       logfile(command);



     }







   if (classicTV && ctvIcon < 4)

   {




           if (ctvIcon == 1)
            {

               icontype = "XBMC icon applied";
               cstring = adb + " shell rm -r /sdcard/.imagecache/com.amazon.venezia/com.adrise.profilms";
               command=RunProcess2(cstring);
               logfile(cstring);
               logfile(command);

               cstring = adb + " shell mkdir -p /sdcard/.imagecache/com.amazon.venezia/com.adrise.profilms";
               command=RunProcess2(cstring);
               logfile(cstring);
               logfile(command);

               cstring = adb + " push "+adbdir+ "xbmc.icon /sdcard/.imagecache/com.amazon.venezia/com.adrise.profilms";
               command=RunProcess2(cstring);
               logfile(cstring);
               logfile(command);
           }


           if (ctvIcon == 2)
            {

               icontype = "Kodi icon applied";
               cstring = adb + " shell rm -r /sdcard/.imagecache/com.amazon.venezia/com.adrise.profilms";
               command=RunProcess2(cstring);
               logfile(cstring);
               logfile(command);

               cstring = adb + " shell mkdir -p /sdcard/.imagecache/com.amazon.venezia/com.adrise.profilms";
               command=RunProcess2(cstring);
               logfile(cstring);
               logfile(command);

               cstring = adb + " push "+adbdir+ "kodi.icon /sdcard/.imagecache/com.amazon.venezia/com.adrise.profilms";
               command=RunProcess2(cstring);
               logfile(cstring);
               logfile(command);
           }

           if (ctvIcon == 3)
            {


               icontype = "Classic TV icon applied";
               cstring = adb + " shell rm -r /sdcard/.imagecache/com.amazon.venezia/com.adrise.profilms";
               command=RunProcess2(cstring);
               logfile(cstring);
               logfile(command);

               cstring = adb + " shell mkdir -p /sdcard/.imagecache/com.amazon.venezia/com.adrise.profilms";
               command=RunProcess2(cstring);
               logfile(cstring);
               logfile(command);

               cstring = adb + " push "+adbdir+ "ctv.icon /sdcard/.imagecache/com.amazon.venezia/com.adrise.profilms";
               command=RunProcess2(cstring);
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
        QMessageBox::information(this,"","Llama not installed.");
        logfile("Llama not installed.");
        return;
    }


       if (isLlama && llamaEvent < 4)
           s = "Llama settings applied.";

        if (llamaInstall)
           s = s + "\nPlease run Llama on the AFTV to activate!";

        if (ctvIcon < 4)
           s = s + "\n"+icontype;


        if (ctvIcon < 4 || llamaEvent < 4 || llamaInstall)
           QMessageBox::information(this,"",s);



    nMilliseconds = rtimer.elapsed();
    logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );
    ui->progressBar->setHidden(true);



}


void MainWindow::on_donate_clicked()
{
    QString link = "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=JA5E5UP3ZSWBN";
    QDesktopServices::openUrl(QUrl(link));

}

void MainWindow::on_actionCredits_triggered()
{
    creditsDialog credits;
    credits.setModal(true);
    credits.exec();
}
