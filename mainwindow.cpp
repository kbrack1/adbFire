#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog2.h"
#include "helpdialog.h"
#include "uninstalldialog.h"
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
// #include <QDebug>

bool isConnected = false;
bool serverRunning = false;
bool  is_packageInstalled = false;
bool  mounted_op = false;

bool ok = false;
bool firstrun=true;

QString filename = "";
QString command = "";
QString adbstr1 = "ADB running. ";
QString adbstr2 = "ADB not running. ";
QString devstr1 = "  Device connected";
QString devstr2 = "  Device not connected.";
QString adbdir =  "";
QString adb = "";
QString xmldir = "";
QString hdir = "";
QString daddr="";
QString sldir = "";
QString pushdir = "";
QString pulldir = "";
bool updatecheck = true;
QString port = ":5555";
QString xbmcpackage ="";


int sshcheck;
int usbcheck;
int ftvupdate;



int tsvalue = 4000;

QString dbstring;
bool dbexists = false;

QSqlDatabase db;



#ifdef Q_OS_LINUX
 int os=0;
#elif defined(Q_OS_WIN)
  int os=1;
#elif defined(Q_OS_MAC)
int os=2;
#endif


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
 QProcess kill_adb;
 kill_adb.setProcessChannelMode(QProcess::MergedChannels);
 QString cstring = adb + " kill-server";
 kill_adb.start(cstring);
command=kill_adb.readAll();
 kill_adb.waitForFinished(-1);
 serverRunning = false;
 
}


 //////////////////////////////////////
void start_server()
{
 QProcess start_adb;
 start_adb.setProcessChannelMode(QProcess::MergedChannels);
 QString cstring = adb + " start-server";
 start_adb.start(cstring);
 start_adb.waitForFinished(-1);
 command=start_adb.readAll();



     if (command.contains("daemon started successfully"))
         serverRunning = true;
         else
         serverRunning = false;

}


/////////////////////////////////////////////////////
bool is_package(QString package)
{

    QProcess ispackage_adb;
    ispackage_adb.setProcessChannelMode(QProcess::MergedChannels);
    QString cstring = adb + " -s " + daddr + port + " shell pm list packages ";
    ispackage_adb.start(cstring);
    ispackage_adb.waitForFinished(-1);
    command=ispackage_adb.readAll();




        if (command.contains(package))
            is_packageInstalled = true;
            else
            is_packageInstalled = false;

        return  is_packageInstalled;
}




/////////////////////////////////////////////////////
bool mount_system(QString mnt)
{


    QProcess mount_system;
       mount_system.setProcessChannelMode(QProcess::MergedChannels);
       QString cstring = adb + " -s " +daddr+port+ " shell su -c mount -o remount,"+mnt+ " /system";
       mount_system.start(cstring);
       mount_system.waitForFinished(-1);
       command=mount_system.readAll();






        if (command.isEmpty())
           return true;
            else
            return false;

        // return  mounted_op;


}

/////////////////////////////////////////////////////
bool mount_root(QString mnt)
{


    QProcess mount_system;
       mount_system.setProcessChannelMode(QProcess::MergedChannels);
       QString cstring = adb + " -s " +daddr+port+ " shell su -c mount -o remount,"+mnt+ " /";
       mount_system.start(cstring);
       mount_system.waitForFinished(-1);
       command=mount_system.readAll();






        if (command.isEmpty())
           return true;
            else
            return false;

        // return  mounted_op;


}


/////////////////////////////////////////////////////
bool amazon_updates(QString onoff)
{

       QProcess update_policy;
       update_policy.setProcessChannelMode(QProcess::MergedChannels);
       QString cstring = adb + " -s " + daddr+port + " shell su -c pm "+ onoff + " com.amazon.dcp";
       update_policy.start(cstring);
       update_policy.waitForFinished(-1);
       command=update_policy.readAll();


        if (onoff == "enable")
           { if (command.contains("enabled"))
               return true;
            else return false;
           }

        if (onoff == "disable")
           { if (command.contains("disabled"))
               return true;
            else return false;
           }

        return false;
}


////////////////////////////////
void createTables()
{

    QSqlQuery query;
    query.exec("create table device(id int primary key, name varchar(20),sldir varchar(100),pushdir varchar(100),pulldir varchar(100), xbmcpackage varchar(50) , usbcheck int, ftvupdate int)");
    query.exec("insert into device values(1, '','"+hdir+"','"+hdir+"','"+hdir+"' ,'org.xbmc.xbmc',0,0 )");

}


////////////////////////////////
void updateTables()
{

    QString str1;
    QString str2;




    str1.setNum(usbcheck);
    str2.setNum(ftvupdate);

    QSqlQuery query;

    QString sqlstatement = "UPDATE device SET name='"+daddr+"' WHERE Id=1";
    query.exec(sqlstatement);

    sqlstatement = "UPDATE device SET sldir='"+sldir+"' WHERE Id=1";
     query.exec(sqlstatement);

     sqlstatement = "UPDATE device SET pushdir='"+pushdir+"' WHERE Id=1";
      query.exec(sqlstatement);

      sqlstatement = "UPDATE device SET pulldir='"+pulldir+"' WHERE Id=1";
       query.exec(sqlstatement);

      sqlstatement = "UPDATE device SET xbmcpackage='"+xbmcpackage+"' WHERE Id=1";
       query.exec(sqlstatement);

        sqlstatement = "UPDATE device SET usbcheck='"+str1+"' WHERE Id=1";
         query.exec(sqlstatement);

         sqlstatement = "UPDATE device SET ftvupdate='"+str2+"' WHERE Id=1";
          query.exec(sqlstatement);

}

////////////////////////////////
void readTables()
{



     QSqlQuery query;
       query.exec("SELECT name FROM device");
         while (query.next()) {
              daddr = query.value(0).toString();
         }


          query.exec("SELECT sldir FROM device");
           while (query.next()) {
                sldir = query.value(0).toString();
          }

           query.exec("SELECT pushdir FROM device");
            while (query.next()) {
                 pushdir = query.value(0).toString();
           }


            query.exec("SELECT pulldir FROM device");
            while (query.next()) {
                  pulldir = query.value(0).toString();
            }

            query.exec("SELECT xbmcpackage FROM device");
            while (query.next()) {
                  xbmcpackage = query.value(0).toString();
            }


            query.exec("SELECT usbcheck FROM device");
            while (query.next()) {
                  usbcheck = query.value(0).toInt();
            }

            query.exec("SELECT ftvupdate FROM device");
            while (query.next()) {
                  ftvupdate = query.value(0).toInt();
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



   // qDebug() << query.isSelect();
   // qDebug() << query.isValid();

}

//////////////////////////////////////////////
 MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{



     ui->setupUi(this);


    this->setFixedSize(this->size().width(), this->size().height());





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

 if (os == 1)
    {
     adbdir = "./";
     adb = adbdir+"adb.exe";
     dbstring = adbdir+"adbfire.db";
     xmldir = adbdir+"xml/";
    }


  if (os == 2)
    {
     adbdir = "/Applications/adbFire/";
     adb = adbdir+"adb";

     dbstring = adbdir+"adbfire.db";
     xmldir = adbdir+"xml/";
    }

  hdir = QDir::homePath();

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

  }

   if (!dbexists)
      createTables();

   // else

   readTables();



   if (usbcheck == 1)
       ui->usbBox->setChecked(true);
   else
      ui->usbBox->setChecked(false);
    ui->device->setText(daddr);

    if (!(os == 1))
      {  QString cstring = "chmod 0755 "+adb;
         QProcess adb_chmod;
         adb_chmod.setProcessChannelMode(QProcess::MergedChannels);
         adb_chmod.start(cstring);
         adb_chmod.waitForFinished(-1);
         command=adb_chmod.readAll();
         
       }

    kill_server();
    start_server();

    if (serverRunning)
     { ui->server_running->setText(adbstr1);

       if (!daddr.isEmpty() && firstrun)
          {  on_connButton_clicked();

           }
    }

    else
     ui->server_running->setText(adbstr2);

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
    updateTables();
    db.close();
    delete ui;

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



    uninstallDialog dialog;
    dialog.setModal(true);
    if(dialog.exec() == QDialog::Accepted)
    {

    package = dialog.packageName();
    keepbox = dialog.keepBox();

    }

    else return;


            if ( !is_package(package))
               { QMessageBox::critical(
                     this,
                     "",
                     package +" not installed");
                  return;
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


                      QProcess uninstall_package;
                      uninstall_package.setProcessChannelMode(QProcess::MergedChannels);

                      if (!keepbox)
                      cstring = adb + " -s " +daddr+port+ " shell pm uninstall " + package;
                      else
                      cstring = adb + " -s " +daddr+port+ " shell pm uninstall -k " + package;

                      uninstall_package.start(cstring);

                      while(uninstall_package.state() != QProcess::NotRunning)
                          qApp->processEvents();

                      command=uninstall_package.readAll();
                      
                      ui->progressBar->setHidden(true);

                      if (!command.contains("Success"))
                       QMessageBox::critical(
                                      this,
                                     "",
                                      "Uninstall failed");
                          else                
                             QMessageBox::information(
                                      this,
                                      "",
                                      "Uninstalled");

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

    // QMessageBox::critical(this,"","xxxx");

    port = ":5555";

    daddr = ui->device->text();
    QString cstring = adb + " connect " +daddr+port;
    QProcess connect_device;
    connect_device.setProcessChannelMode(QProcess::MergedChannels);
    connect_device.start(cstring);
    connect_device.waitForFinished(-1);
    command=connect_device.readAll();



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
}

/////////////////////////////////////////////////////////////////////
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

  command = "";

   QString fileName = QFileDialog::getOpenFileName(this,
          tr("Select app to install"), sldir , tr("APK Files (*.apk)"));






    // qDebug() << fileInfo.absolutePath();
   // qDebug() << fileInfo.fileName();

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


          QProcess install_apk;
          install_apk.setProcessChannelMode(QProcess::MergedChannels);
          QString cstring = adb + " install -r " + '"'+ fileName+'"';
          install_apk.start(cstring);

           while(install_apk.state() != QProcess::NotRunning)
               qApp->processEvents();

           command=install_apk.readAll();
           
           ui->progressBar->setHidden(true);

           if (command.contains("Success"))
              QMessageBox::information(
                          this,
                         "",
                          "Installed");
              else
              QMessageBox::critical(
                          this,
                          "",
                          "Install failed");

          }

 }



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



        ui->usbBox->setDisabled(true);
        daddr = ui->device->text();
        QString cstring = adb + " disconnect "+daddr+port ;
        QProcess disconnect_device;
        disconnect_device.setProcessChannelMode(QProcess::MergedChannels);
        disconnect_device.start(cstring);
        disconnect_device.waitForFinished(-1);
        command=disconnect_device.readAll();
        

        isConnected=false;
         ui->device_connected->setText("  Device not connected.");

}


///////////////////////////////////////////////////////////////////////
void MainWindow::on_astart_Button_clicked()
{

   kill_server();
    start_server();
    if (serverRunning)
      ui->server_running->setText(adbstr1);
    else
     ui->server_running->setText(adbstr2);

}


////////////////////////////////////////////////////////////////////
void MainWindow::on_akill_Button_clicked()
{


    QString cstring = adb + "  disconnect "+daddr+port ;
    QProcess disconnect_device;
    disconnect_device.setProcessChannelMode(QProcess::MergedChannels);
    disconnect_device.start(cstring);
    disconnect_device.waitForFinished(-1);
    command=disconnect_device.readAll();
    

    isConnected=false;
     ui->device_connected->setText(devstr2);


    kill_server();

    if (serverRunning)
      ui->server_running->setText(adbstr1);
    else
     ui->server_running->setText(adbstr2);
}




//////////////////////////////////////////////////////////////////////
void MainWindow::on_actionQuit_triggered()
{

    QCoreApplication::quit();
}

/////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionAbout_triggered()
{
    Dialog2 dialog2;
    dialog2.setModal(true);
    dialog2.exec();

}


//////////////////////////////////////////////
void MainWindow::on_actionHelp_triggered()
{
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


QString hdir = QDir::homePath();
 command = "";
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


       QProcess pull_adb;
       pull_adb.setProcessChannelMode(QProcess::MergedChannels);

       QString cstring = adb + " -s " + daddr + port + " pull "+xpath+" "+'"'+dir+'"';

       pull_adb.start(cstring);

        while(pull_adb.state() != QProcess::NotRunning)
            qApp->processEvents();

        command = pull_adb.readAll();
        

       if (command.contains("bytes"))
        QMessageBox::information(
                       this,
                      "",
                      "Backup done" );
           else
            QMessageBox::critical(
                       this,
                       "",
                    "Backup failed");


   }




}

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


                        QFile Fout1(rootfile1);

                        if(!Fout1.exists())
                        {

                            QMessageBox::critical(
                               this,
                               tr("adbFire"),
                               rootfile1+" not found.");
                               return;
                        }


                        QFile Fout2(rootfile2);

                        if(!Fout2.exists())
                        {

                            QMessageBox::critical(
                               this,
                               tr("adbFire"),
                               rootfile2+" not found.");
                               return;
                        }


                        command = "";

                              ui->progressBar->setHidden(false);
                              ui->progressBar->setValue(0);

                              QTimer *timer = new QTimer(this);
                              connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
                              timer->start(tsvalue);



                              QProcess install_apk1;
                              QString cstring = adb + " -s " + daddr + port + " install -r "+rootfile1;
                               install_apk1.start(cstring);

                               while(install_apk1.state() != QProcess::NotRunning)
                                   qApp->processEvents();

                               command=install_apk1.readAll();
                               

                              if (!command.contains("Success"))
                                 { QMessageBox::information(
                                              this,
                                              "",
                                              "tr.apk install failed");
                                   QFile::remove(rootfile1);
                                   ui->progressBar->setHidden(true);
                                   return;
                                 }
                              else
                                  file1 = true;


                                  command = "";



                                    QProcess install_apk2;
                                     cstring = adb + " -s " + daddr + port + " install -r "+rootfile2;
                                     install_apk2.start(cstring);

                                     while(install_apk2.state() != QProcess::NotRunning)
                                         qApp->processEvents();

                                     command=install_apk2.readAll();
                                     
                                     ui->progressBar->setHidden(true);

                                     if (!command.contains("Success"))
                                        { QMessageBox::information(
                                                     this,
                                                     "",
                                                     "su.apk install failed");

                                        }
                                     else
                                         file2 = true;

 if (file1 && file2)
 {

  QMessageBox::information( this,"","Root software installed.\nRun towelroot on the FireTV\n to finish rooting process.");

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
        return;
     }



 QString fileName = QFileDialog::getOpenFileName(this,
 "Push file to "+cname, pushdir, tr("Files (*)"));






 if (!fileName.isEmpty() )
 {

     QFileInfo finfo(fileName);
     pushdir = finfo.absolutePath();


     QProcess check_dir;
      check_dir.setProcessChannelMode(QProcess::MergedChannels);
      QString cstring = adb + " -s " + daddr + port +  " shell ls "+xpath;
      check_dir.start(cstring);
      check_dir.waitForFinished(-1);
      command=check_dir.readAll();
      

      if (command.contains("No such file or directory"))
       { QMessageBox::critical(
                      this,
                     "",
                      "Destination path missing. Has XBMC had its first run to set up internal folders?");
                      return;
      }


     QMessageBox::StandardButton reply;
       reply = QMessageBox::question(this, "Push", "Push "+fileName+" to "+cname+"?",
           QMessageBox::Yes|QMessageBox::No);
       if (reply == QMessageBox::Yes) {

           QProcess push_adb;
           push_adb.setProcessChannelMode(QProcess::MergedChannels);

           cstring = adb + " -s " + daddr + port +  " push "+'"'+fileName+'"'+ " "+xpath;

           push_adb.start(cstring);

           // push_adb.waitForFinished(-1);

            while(push_adb.state() != QProcess::NotRunning)
                qApp->processEvents();

            command = push_adb.readAll();
            

           if (command.contains("bytes"))
            QMessageBox::information(
                           this,
                          "",
                          "File Pushed." );
               else
                QMessageBox::critical(
                           this,
                           "",
                        "Push failed ");


   }



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



     // QString pulldir = QDir::homePath();

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
             return;
          }


      QString pullfile = QInputDialog::getText(this,"" ,
        "Filename:", QLineEdit::Normal,
        "", &ok);

             if (!ok)
               return;

                         QProcess check_dir;
                          check_dir.setProcessChannelMode(QProcess::MergedChannels);
                          QString cstring = adb + " -s " + daddr + port +  " shell ls "+xpath+pullfile;
                          check_dir.start(cstring);
                          check_dir.waitForFinished(-1);
                          command=check_dir.readAll();
                          

                          if (command.contains("No such file or directory"))
                           { QMessageBox::critical(
                                          this,
                                         "",
                                          "No such file or directory");
                                          return;
                          }


             QProcess pull_file;
             pull_file.setProcessChannelMode(QProcess::MergedChannels);
             cstring = adb + " -s " + daddr + port +  " pull "+xpath+pullfile+" "+pulldir;
             pull_file.start(cstring);
             pull_file.waitForFinished(-1);
             command=pull_file.readAll();
             

             if (command.contains("exist"))
              QMessageBox::critical(
                             this,
                            "",
                             "Pull of "+pullfile+" failed");
                 else
                  QMessageBox::information(
                             this,
                             "",
                             "Pull of "+pullfile+" succeeded");

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

 command = "";

 // "shell mkdir -p /sdcard/Android/data/org.xbmc.xbmc/files"

 QString xpath = "/sdcard/Android/data/";

 QString cstring;

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


       QProcess rm_dir;
       rm_dir.setProcessChannelMode(QProcess::MergedChannels);
       cstring = adb + " -s " + daddr + port + " shell rm -r "+xpath +xbmcpackage;
       rm_dir.start(cstring);

       while(rm_dir.state() != QProcess::NotRunning)
           qApp->processEvents();

       command = rm_dir.readAll();
       

       QProcess restore_xbmc;
       restore_xbmc.setProcessChannelMode(QProcess::MergedChannels);

       cstring = adb + " -s " + daddr + port +  " push "+'"'+dir+'"'+ " "+xpath+xbmcpackage;

       restore_xbmc.start(cstring);

        while(restore_xbmc.state() != QProcess::NotRunning)
            qApp->processEvents();

        command = restore_xbmc.readAll();
        
        ui->progressBar->setHidden(true);

       if (command.contains("bytes"))
        QMessageBox::information(
                       this,
                      "",
                      "Restore complete." );
           else
            QMessageBox::critical(
                       this,
                       "",
                    "Restore failed ");




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


 QString fileName = QFileDialog::getOpenFileName(this,
 "Choose remote xml file", xmldir, tr("Files (*.xml)"));

 if (!fileName.isEmpty() )
 {

     QProcess check_dir;
      check_dir.setProcessChannelMode(QProcess::MergedChannels);
      QString cstring = adb + " -s " + daddr + port +  " shell ls "+xpath;
      check_dir.start(cstring);
      check_dir.waitForFinished(-1);
      command=check_dir.readAll();
      

      if (command.contains("No such file or directory"))
       { QMessageBox::critical(
                      this,
                     "",
                      "Destination path missing");
                      return;
      }


     QMessageBox::StandardButton reply;
       reply = QMessageBox::question(this, "Push", fileName+" selected. Continue?",
           QMessageBox::Yes|QMessageBox::No);
       if (reply == QMessageBox::Yes) {

           QProcess push_adb;
           push_adb.setProcessChannelMode(QProcess::MergedChannels);

           cstring = adb + " -s " + daddr + port +  " push "+'"'+fileName+'"'+ " "+xpath+"/keyboard.xml";

           push_adb.start(cstring);

           // push_adb.waitForFinished(-1);

            while(push_adb.state() != QProcess::NotRunning)
                qApp->processEvents();

            command = push_adb.readAll();
            

           if (command.contains("bytes"))
            QMessageBox::information(
                           this,
                          "",
                          "Remote xml installed." );
               else
                QMessageBox::critical(
                           this,
                           "",
                        "Remote xml installation failed.");


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

QString cstring = "";

    if (os == 1)
       {
       cstring = "cmd /k adb -s "  + daddr + port + " shell";
       QProcess::startDetached(cstring);
       }


     if (os == 2)
       {
        cstring = "open -a Terminal.app /Applications/adbFire/console.sh";
        QProcess::startDetached(cstring);
        }


}

//////////////////////////////////////////////////
void MainWindow::on_actionPreferences_triggered()
{

    // updatecheck = true;

    preferencesDialog dialog;
    dialog.setPackagename(xbmcpackage);
    dialog.setPulldir(pulldir);
    dialog.setftvUpdate(updatecheck);
    dialog.setModal(true);


    if(dialog.exec() == QDialog::Accepted)
    {

    xbmcpackage = dialog.xbmcpackageName();
    pulldir = dialog.pulldir();
    updatecheck = dialog.updatecheck();


   if ( is_package("eu.chainfire.supersu"))
     {

    if (updatecheck)
     {
       if (amazon_updates("enable"))
       QMessageBox::information(this,"","Amazon updates on\ncom.amazon.dcp enabled");
     else
        QMessageBox::critical(this,"","Problem: com.amazon.dcp not enabled!");
    }

    else
    {
        if (amazon_updates("disable"))
        QMessageBox::information(this,"","Amazon updates off\ncom.amazon.dcp disabled");
      else
         QMessageBox::critical(this,"","Problem: com.amazon.dcp not disabled!");
    }
  }

    updateTables();

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




          QElapsedTimer rtimer;
          int nMilliseconds;

            QProcess reboot_device;
            rtimer.start();
            reboot_device.setProcessChannelMode(QProcess::MergedChannels);
            cstring = adb + " -s " +daddr+port+ " reboot";
            reboot_device.start(cstring);


             while(reboot_device.state() != QProcess::NotRunning)
               {
                 qApp->processEvents();
                  nMilliseconds = rtimer.elapsed();
                if (nMilliseconds >= 5000)
                    break;
             }

             


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


  QElapsedTimer rtimer;
   int nMilliseconds;


   QProcess reboot_recovery;
    rtimer.start();
   reboot_recovery.setProcessChannelMode(QProcess::MergedChannels);
   cstring = adb + " -s " +daddr+port+ " reboot recovery";
   reboot_recovery.start(cstring);

   while(reboot_recovery.state() != QProcess::NotRunning)
     {
       qApp->processEvents();
        nMilliseconds = rtimer.elapsed();
      if (nMilliseconds >= 5000)
          break;
   }

    

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


        QDateTime dateTime = QDateTime::currentDateTime();
        QString dtstr = dateTime.toString("MMddyyhhmmss");


    QProcess screen_shot;
    screen_shot.setProcessChannelMode(QProcess::MergedChannels);
    QString cstring = adb +" -s " + daddr+port +" shell screencap -p /sdcard/"+dtstr+".png";
    screen_shot.start(cstring);
    screen_shot.waitForFinished(-1);
    command=screen_shot.readAll();
    

    if (!command.isEmpty())
     QMessageBox::critical(
                    this,
                   "",
                    "Screenshot failed");

    else
    {

        QProcess get_shot;
        get_shot.setProcessChannelMode(QProcess::MergedChannels);
        QString cstring = adb +" -s " + daddr+port +" pull /sdcard/"+dtstr+".png " +pulldir;
        get_shot.start(cstring);
        get_shot.waitForFinished(-1);
        command=get_shot.readAll();
        


        if (!command.contains("bytes"))
         QMessageBox::critical(
                        this,
                       "",
                        "Screenshot failed");
        else

            QMessageBox::information(
                           this,
                          "",
                           "Screenshot "+dtstr+ " copied to "+pulldir);


        QProcess del_shot;
        del_shot.setProcessChannelMode(QProcess::MergedChannels);
        cstring = adb +" -s " + daddr+port +" shell rm /sdcard/"+dtstr+".png " ;
        del_shot.start(cstring);
        del_shot.waitForFinished(-1);
        command=del_shot.readAll();
        

    }



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


   QMessageBox::StandardButton reply;
         reply = QMessageBox::question(this, "Busybox", "Install Busybox?",
            QMessageBox::Yes|QMessageBox::No);
         if (reply == QMessageBox::No)
         return;


    QString busybox1  = adbdir+"binstall.sh";
    QString busybox2 = adbdir+"buninstall.sh";
    QString busybox3 = adbdir+"busybox";
    QString busybox4 = adbdir+"ntfs-3g";
    QString busybox5 = adbdir+"mount.exfat-fuse";
    QString busybox6 = adbdir+"mntdrives.sh";

        bool file1 = false;
        bool file2 = false;
        bool file3 = false;
        bool file4 = false;
        bool file5 = false;
        bool file6 = false;

     QFile Fout1(busybox1);

         if(!Fout1.exists())
            {

            QMessageBox::critical(
            this,
            tr("adbFire"),
            busybox1+" not found.");
             return;
             }


        QFile Fout2(busybox2);

       if(!Fout2.exists())
          {

            QMessageBox::critical(
             this,
             tr("adbFire"),
              busybox2+" not found.");
             return;
           }


       QFile Fout3(busybox3);

      if(!Fout3.exists())
         {

           QMessageBox::critical(
            this,
            tr("adbFire"),
             busybox3+" not found.");
            return;
          }


      QFile Fout4(busybox4);

      if(!Fout4.exists())
         {

           QMessageBox::critical(
            this,
            tr("adbFire"),
             busybox4+" not found.");
            return;
          }


      QFile Fout5(busybox5);

      if(!Fout5.exists())
         {

           QMessageBox::critical(
            this,
            tr("adbFire"),
             busybox5+" not found.");
            return;
          }


      QFile Fout6(busybox6);

      if(!Fout6.exists())
         {

           QMessageBox::critical(
            this,
            tr("adbFire"),
             busybox6+" not found.");
            return;
          }


      command = "";


      QProcess install_apk1;
       QString cstring = adb + " -s " + daddr + port + " push "+busybox1+ " /sdcard/";
        install_apk1.start(cstring);


        install_apk1.waitForFinished(-1);

       command=install_apk1.readAll();
        

         if (!command.isEmpty())
           { QMessageBox::critical(
            this,
             "",
             "busybox install failed ");
              //ui->progressBar->setHidden(true);
             return;
             }
            else
            file1 = true;


             command = "";

             QProcess install_apk2;
              cstring = adb + " -s " + daddr + port + " push "+busybox2+ " /sdcard/";
              install_apk2.start(cstring);

              install_apk2.waitForFinished(-1);

              command=install_apk2.readAll();
              
              //ui->progressBar->setHidden(true);

              if (!command.isEmpty())
                 { QMessageBox::information(
                              this,
                              "",
                              "busybox install failed");

                 }
              else
                  file2 = true;




              command = "";

              QProcess install_apk3;
               cstring = adb + " -s " + daddr + port + " push "+busybox3+ " /sdcard/";
               install_apk3.start(cstring);

               install_apk3.waitForFinished(-1);

               command=install_apk3.readAll();
               
               //ui->progressBar->setHidden(true);

               if (!command.isEmpty())
                  { QMessageBox::information(
                               this,
                               "",
                               "busybox install failed");

                  }
               else
                   file3 = true;

               QProcess install_apk4;
                cstring = adb + " -s " + daddr + port + " push "+busybox4+ " /sdcard/";
                install_apk4.start(cstring);

                install_apk4.waitForFinished(-1);

                command=install_apk4.readAll();

               // ui->progressBar->setHidden(true);

                if (!command.isEmpty())
                   { QMessageBox::information(
                                this,
                                "",
                                "busybox install failed");

                   }
                else
                    file4 = true;


                QProcess install_apk5;
                 cstring = adb + " -s " + daddr + port + " push "+busybox5+ " /sdcard/";
                 install_apk5.start(cstring);

                 install_apk5.waitForFinished(-1);

                 command=install_apk5.readAll();

                // ui->progressBar->setHidden(true);

                 if (!command.isEmpty())
                    { QMessageBox::information(
                                 this,
                                 "",
                                 "busybox install failed");

                    }
                 else
                     file5 = true;


                 QProcess install_apk6;
                  cstring = adb + " -s " + daddr + port + " push "+busybox6+ " /sdcard/";
                  install_apk6.start(cstring);

                  install_apk6.waitForFinished(-1);

                  command=install_apk6.readAll();

                 // ui->progressBar->setHidden(true);

                  if (!command.isEmpty())
                     { QMessageBox::information(
                                  this,
                                  "",
                                  "busybox install failed");

                     }
                  else
                      file6 = true;







 if (file1 && file2 && file3 && file4 && file5 && file6)

 {

      mount_system("rw");

     QProcess copy_script1;
     copy_script1.setProcessChannelMode(QProcess::MergedChannels);
     QString cstring = adb + " -s " + daddr+port + " shell su -c cp " + " /sdcard/binstall.sh /system/xbin";
     copy_script1.start(cstring);
     copy_script1.waitForFinished(-1);
     command=copy_script1.readAll();
     


     QProcess copy_script2;
     copy_script2.setProcessChannelMode(QProcess::MergedChannels);
     cstring = adb + " -s " + daddr+port + " shell su -c cp " + " /sdcard/buninstall.sh /system/xbin";
     copy_script2.start(cstring);
     copy_script2.waitForFinished(-1);
     command=copy_script2.readAll();
     


     QProcess copy_script3;
     copy_script3.setProcessChannelMode(QProcess::MergedChannels);
     cstring = adb + " -s " + daddr+port + " shell su -c cp " + " /sdcard/busybox /system/xbin";
     copy_script3.start(cstring);
     copy_script3.waitForFinished(-1);
     command=copy_script3.readAll();
     


     QProcess copy_script4;
     copy_script4.setProcessChannelMode(QProcess::MergedChannels);
     cstring = adb + " -s " + daddr+port + " shell su -c cp " + " /sdcard/ntfs-3g /system/xbin";
     copy_script4.start(cstring);
     copy_script4.waitForFinished(-1);
     command=copy_script4.readAll();


     QProcess copy_script5;
     copy_script5.setProcessChannelMode(QProcess::MergedChannels);
     cstring = adb + " -s " + daddr+port + " shell su -c cp " + " /sdcard/mount.exfat-fuse /system/xbin";
     copy_script5.start(cstring);
     copy_script5.waitForFinished(-1);
     command=copy_script5.readAll();

     QProcess copy_script6;
     copy_script6.setProcessChannelMode(QProcess::MergedChannels);
     cstring = adb + " -s " + daddr+port + " shell su -c cp " + " /sdcard/mntdrives.sh /system/xbin";
     copy_script6.start(cstring);
     copy_script6.waitForFinished(-1);
     command=copy_script6.readAll();



     QProcess chmod_script1;
     chmod_script1.setProcessChannelMode(QProcess::MergedChannels);
     cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 /system/xbin/binstall.sh";
     chmod_script1.start(cstring);
     chmod_script1.waitForFinished(-1);
     command=chmod_script1.readAll();
     


     QProcess chmod_script2;
     chmod_script2.setProcessChannelMode(QProcess::MergedChannels);
     cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 /system/xbin/buninstall.sh";
     chmod_script2.start(cstring);
     chmod_script2.waitForFinished(-1);
     command=chmod_script2.readAll();




     QProcess chmod_script3;
     chmod_script3.setProcessChannelMode(QProcess::MergedChannels);
     cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 /system/xbin/busybox";
     chmod_script3.start(cstring);
     chmod_script3.waitForFinished(-1);
     command=chmod_script3.readAll();

     
     QProcess chmod_script4;
     chmod_script4.setProcessChannelMode(QProcess::MergedChannels);
     cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 /system/xbin/ntfs-3g";
     chmod_script4.start(cstring);
     chmod_script4.waitForFinished(-1);
     command=chmod_script4.readAll();


     QProcess chmod_script5;
     chmod_script5.setProcessChannelMode(QProcess::MergedChannels);
     cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 /system/xbin/mount.exfat-fuse";
     chmod_script5.start(cstring);
     chmod_script5.waitForFinished(-1);
     command=chmod_script5.readAll();


     QProcess chmod_script6;
     chmod_script6.setProcessChannelMode(QProcess::MergedChannels);
     cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 /system/xbin/mntdrives.sh";
     chmod_script6.start(cstring);
     chmod_script6.waitForFinished(-1);
     command=chmod_script5.readAll();

     QProcess run_script1;
     run_script1.setProcessChannelMode(QProcess::MergedChannels);
     cstring = adb + " -s " + daddr+port + " shell su -c /system/xbin/binstall.sh";
     run_script1.start(cstring);
     run_script1.waitForFinished(-1);
     command=run_script1.readAll();
     



      QProcess check_dir;
      check_dir.setProcessChannelMode(QProcess::MergedChannels);
      cstring = adb + " -s " + daddr + port +  " shell ls /system/xbin/which";
      check_dir.start(cstring);
      check_dir.waitForFinished(-1);
      command=check_dir.readAll();
      

        if (command.contains("No such file or directory"))
          QMessageBox::critical( this,"","Busybox not installed!");
        else
         QMessageBox::information( this,"","Busybox installed!");

 }

mount_system("ro");

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


   QMessageBox::StandardButton reply;
         reply = QMessageBox::question(this, "Busybox", "Uninstall Busybox?",
            QMessageBox::Yes|QMessageBox::No);
         if (reply == QMessageBox::No)
         return;

         QString cstring;
         QString umntstring = "/system/xbin/umount /storage/sd\?\?/";
         QString rmsd = "rm -r /storage/sd\?\?/";




         QProcess check_usb;
         check_usb.setProcessChannelMode(QProcess::MergedChannels);
         cstring = adb + " -s " + daddr+port + " shell su -c ls /storage/sd\?\?/";
         check_usb.start(cstring);
         check_usb.waitForFinished(-1);
         command=check_usb.readAll();

         if (!command.contains("No such file or directory"))

         {
             mount_root("rw");


            QProcess umount_system;
            umount_system.setProcessChannelMode(QProcess::MergedChannels);
            cstring = adb + " -s " + daddr+port + " shell su -c " + umntstring;
            umount_system.start(cstring);
            umount_system.waitForFinished(-1);
            command=umount_system.readAll();


            QProcess rm_sd;
            rm_sd.setProcessChannelMode(QProcess::MergedChannels);
            cstring = adb + " -s " + daddr+port + " shell su -c " + rmsd;
            rm_sd.start(cstring);
            rm_sd.waitForFinished(-1);
            command=rm_sd.readAll();

             mount_root("ro");

           }



             mount_system("rw");

         QProcess run_script1;
         run_script1.setProcessChannelMode(QProcess::MergedChannels);
         cstring = adb + " -s " + daddr+port + " shell su -c /system/xbin/buninstall.sh";
         run_script1.start(cstring);
         run_script1.waitForFinished(-1);
         

        command = "";

        QProcess check_dir;
         check_dir.setProcessChannelMode(QProcess::MergedChannels);
         cstring = adb + " -s " + daddr + port +  " shell ls /system/xbin/which";
         check_dir.start(cstring);
         check_dir.waitForFinished(-1);
         command=check_dir.readAll();
         

           if (command.contains("No such file or directory"))
             QMessageBox::information( this,"","Busybox uninstalled");
           else
            QMessageBox::critical( this,"","Busybox not uninstalled!");

    mount_system("ro");

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
          return;
       }





      QString pullfile = QInputDialog::getText(this,"" ,
        "Filename:", QLineEdit::Normal,
        "", &ok);

             if (!ok)
               return;

                         QProcess check_dir;
                          check_dir.setProcessChannelMode(QProcess::MergedChannels);
                          QString cstring = adb + " -s " + daddr + port +  " shell ls "+xpath+pullfile;
                          check_dir.start(cstring);
                          check_dir.waitForFinished(-1);
                          command=check_dir.readAll();
                          

                          if (command.contains("No such file or directory"))
                           { QMessageBox::critical(
                                          this,
                                         "",
                                          "No such file or directory");
                                          return;
                          }


             QProcess pull_file;
             pull_file.setProcessChannelMode(QProcess::MergedChannels);
             cstring = adb + " -s " + daddr + port +  " shell rm "+xpath+pullfile;
             pull_file.start(cstring);
             pull_file.waitForFinished(-1);
             command=pull_file.readAll();
             

             if (command.contains("exist"))
              QMessageBox::critical(
                             this,
                            "",
                             "Deletion of "+pullfile+" failed");
                 else
                  QMessageBox::information(
                             this,
                             "",
                             "Deletion of "+pullfile+" succeeded");

}

//////////////////////////////////////////////////////
void MainWindow::on_actionFirmware_install_triggered()
{


    bool usbstick = false;
    QString cstring;
    QString updatezip = "--update_package=/cache/update.zip";
    QString commstr = adbdir+"command";
    QString tmpstr = adbdir+"tmpstr";
    bool ok;

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
     reply = QMessageBox::question(this, "", "Install Firmware?",
                                   QMessageBox::Yes|QMessageBox::No);
     if (reply == QMessageBox::No)
         return;





     QProcess step_1;
    step_1.setProcessChannelMode(QProcess::MergedChannels);
    cstring = adb + " -s " + daddr+port + " shell su -c chmod 777 /cache";
    step_1.start(cstring);
    step_1.waitForFinished(-1);
    command=step_1.readAll();

    
    if (!command.isEmpty())
     { QMessageBox::information( this,"","Error: "+command);
        return; }

    QProcess step_2;
    step_2.setProcessChannelMode(QProcess::MergedChannels);
    cstring = adb + " -s " + daddr+port + " shell su -c chmod 777 /cache/recovery";
    step_2.start(cstring);
    step_2.waitForFinished(-1);
    command=step_2.readAll();
    

    if (!command.isEmpty())
     { QMessageBox::information( this,"","Error: "+command);
        return; }


    QFile file(commstr);

        if(!file.open(QFile::WriteOnly |
                      QFile::Text))
        {
            QMessageBox::critical(this,"","Error creating file!");
            return;
        }


        QTextStream out(&file);
        out  << updatezip << endl;

        file.flush();
        file.close();


    QProcess step_3;

    step_3.setProcessChannelMode(QProcess::MergedChannels);
    cstring = adb + " -s " + daddr + port + " push "+commstr+ " /sdcard/";
    step_3.start(cstring);
    step_3.waitForFinished(-1);
    command=step_3.readAll();
    
    if (!command.contains("bytes"))
     { QMessageBox::information( this,"","Error: "+command);
        return; }

    QProcess step_4;
    step_4.setProcessChannelMode(QProcess::MergedChannels);
    cstring = adb + " -s " + daddr+port + " shell su -c cp " + " /sdcard/command /cache/recovery/";
    step_4.start(cstring);
    step_4.waitForFinished(-1);
    command=step_4.readAll();



    QProcess step_5;
    step_5.setProcessChannelMode(QProcess::MergedChannels);
    cstring = adb + " -s " + daddr+port + " shell su -c chmod 777 /cache/recovery/command";
    step_5.start(cstring);
    step_5.waitForFinished(-1);
    command=step_5.readAll();


    QString fileName;

    QMessageBox::StandardButton reply2;
      reply2 = QMessageBox::question(this, "", "Is update.zip on usb storage?",
                                    QMessageBox::Yes|QMessageBox::No);
      if (reply2 == QMessageBox::Yes)
         {


          QProcess update_zip_count;
          update_zip_count.setProcessChannelMode(QProcess::MergedChannels);
          cstring = adb + " -s " + daddr+port + " shell su -c find /storage/usb/ -name update.zip | wc -l" ;
          update_zip_count.start(cstring);
          update_zip_count.waitForFinished(-1);
          command=update_zip_count.readAll();



          int value = command.toInt(&ok);
          if (!ok)
              {QMessageBox::critical(this,"","Error 1: update.zip search");
              return;
          }

          if (value<=0)
          {QMessageBox::critical(this,"","update.zip not found");
          return; }

          if (value>1)
          {QMessageBox::critical(this,"","More than 1 update.zip found!");
          return; }





       QProcess update_zip_existx;
       update_zip_existx.setProcessChannelMode(QProcess::MergedChannels);
       cstring = adb + " -s " + daddr+port + " shell su -c find /storage/usb/ -name update.zip" ;
       //cstring = adb + " -s " + daddr+port + " shell pm list packages" ;
       update_zip_existx.start(cstring);
       update_zip_existx.waitForFinished(-1);
       command=update_zip_existx.readAll();


          usbstick = true;

          if (command.contains("/update.zip"))
            fileName = command;

          QProcess update_zip_exist;
          update_zip_exist.setProcessChannelMode(QProcess::MergedChannels);
          cstring = adb + " -s " + daddr+port + " shell ls "+fileName;
          update_zip_exist.start(cstring);
          update_zip_exist.waitForFinished(-1);
          command=update_zip_exist.readAll();

          if (command.contains("No such file or directory"))
           {
              QMessageBox::information( this,"","Error: "+command);
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
      { QProcess step_6;
       step_6.setProcessChannelMode(QProcess::MergedChannels);




       cstring = adb + " -s " +daddr+port+" push "  + '"' +   fileName + '"'   + " /sdcard/update.zip";

       step_6.start(cstring);


    while(step_6.state() != QProcess::NotRunning)
        qApp->processEvents();

    command=step_6.readAll();

 }


    QProcess step_7;
    step_7.setProcessChannelMode(QProcess::MergedChannels);

    if (!usbstick)
    cstring = adb + " -s " + daddr+port + " shell su -c cp " + " /sdcard/update.zip /cache/";
    else
    cstring = adb + " -s " + daddr+port + " shell su -c cp " + fileName + " /cache/";

    step_7.start(cstring);




    while(step_7.state() != QProcess::NotRunning)
        qApp->processEvents();


    command=step_7.readAll();
    

   if (!usbstick)
   {
    QProcess step_8;
    step_8.setProcessChannelMode(QProcess::MergedChannels);
    cstring = adb + " -s " + daddr+port + " shell rm /sdcard/update.zip";
    step_8.start(cstring);
    step_8.waitForFinished(-1);
    command=step_8.readAll();
    
   }



    QProcess step_9;
    step_9.setProcessChannelMode(QProcess::MergedChannels);
    cstring = adb + " -s " + daddr+port + " shell rm /sdcard/command";
    step_9.start(cstring);
    step_9.waitForFinished(-1);
    command=step_9.readAll();


    QFile::remove(commstr);
    QFile Fout(commstr);

    if(Fout.exists())
    {
        QMessageBox::critical(this,"","Error: rm of PC: "+commstr+" script");
    }



    ui->progressBar->setHidden(true);



    QMessageBox::StandardButton reply3;
      reply3 = QMessageBox::question(this, "", "Reboot to recovery?\nFirmware:"+fileName,
                                    QMessageBox::Yes|QMessageBox::No);
      if (reply3 == QMessageBox::No)
       {

          QProcess rm_update1;
          rm_update1.setProcessChannelMode(QProcess::MergedChannels);
          cstring = adb + " -s " + daddr+port + " shell su -c rm /cache/update.zip";
          rm_update1.start(cstring);
          rm_update1.waitForFinished(-1);
          command=rm_update1.readAll();


          QProcess rm_update2;
          rm_update2.setProcessChannelMode(QProcess::MergedChannels);
          cstring = adb + " -s " + daddr+port + " shell su -c rm /cache/recovery/command";
          rm_update2.start(cstring);
          rm_update2.waitForFinished(-1);
          command=rm_update2.readAll();


          return;

       }


    QElapsedTimer rtimer;
     int nMilliseconds;

    QProcess reboot_recovery;
     rtimer.start();
    reboot_recovery.setProcessChannelMode(QProcess::MergedChannels);
    cstring = adb + " -s " +daddr+port+ " reboot recovery";
    reboot_recovery.start(cstring);


    while(reboot_recovery.state() != QProcess::NotRunning)
      {
        qApp->processEvents();
         nMilliseconds = rtimer.elapsed();
       if (nMilliseconds >= 5000)
           break;
    }

     
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


QString cstring;

   ui->progressBar->setHidden(false);
   ui->progressBar->setValue(0);

   QTimer *timer = new QTimer(this);
   connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
   timer->start(tsvalue);

   QProcess check_bb;
   check_bb.setProcessChannelMode(QProcess::MergedChannels);
   cstring = adb + " -s " + daddr + port +  " shell ls /system/xbin/mount";
   check_bb.start(cstring);
   check_bb.waitForFinished(-1);
   command=check_bb.readAll();


     if (command.contains("No such file or directory"))
      { QMessageBox::critical( this,"","Busybox required for USB drive. Install it from the menu.");
       return;
     }


    QProcess mount_system;
       mount_system.setProcessChannelMode(QProcess::MergedChannels);
       cstring = adb + " -s " + daddr+port + " shell su -c /system/xbin/mntdrives.sh";
       mount_system.start(cstring);


       while(mount_system.state() != QProcess::NotRunning)
           qApp->processEvents();


       command=mount_system.readAll();

       ui->progressBar->setHidden(true);

       if (command.isEmpty()  || command.contains("FUSE"))
        {
           QMessageBox::information( this,"","USB drive(s) mounted");
           return;
        }



       if (command.contains("resource busy"))
        {
           QMessageBox::information( this,"","USB drive already mounted");
           return;
        }


       if (!command.isEmpty())
        {
           QMessageBox::information( this,"","Error: "+command);
           return;
        }

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




   QProcess check_bb;
   check_bb.setProcessChannelMode(QProcess::MergedChannels);
   QString cstring = adb + " -s " + daddr + port +  " shell ls /system/xbin/umount";
   check_bb.start(cstring);
   check_bb.waitForFinished(-1);
   command=check_bb.readAll();


     if (command.contains("No such file or directory"))
      { QMessageBox::critical( this,"","Busybox required for USB drive. Install it from the menu.");
       return;
     }


    QString umntstring = "/system/xbin/umount /storage/sd\?\?/";
    QString rmsd = "rm -r /storage/sd\?\?/";

    mount_root("rw");

       QProcess umount_system;
       umount_system.setProcessChannelMode(QProcess::MergedChannels);
       cstring = adb + " -s " + daddr+port + " shell su -c " + umntstring;
       umount_system.start(cstring);
       umount_system.waitForFinished(-1);
       command=umount_system.readAll();
       

       if (command.contains("failed"))
         QMessageBox::critical( this,"","USB drive not found!");
       else
        {
           QProcess rm_sd;
           rm_sd.setProcessChannelMode(QProcess::MergedChannels);
           cstring = adb + " -s " + daddr+port + " shell su -c " + rmsd;
           rm_sd.start(cstring);
           rm_sd.waitForFinished(-1);
           command=rm_sd.readAll();

           QMessageBox::information( this,"","USB Drive unmounted.");
           mount_root("ro");
        }


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




   if (mount_system("rw"))
    QMessageBox::information(
                   this,
                  "",
                   "Filesystem mounted r/w");
    else

       QMessageBox::critical(
                      this,
                     "",
                      "Error: Filesystem not remounted!!");


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


   if (mount_system("ro"))
    QMessageBox::information(
                   this,
                  "",
                   "Filesystem mounted r/o");
    else

       QMessageBox::critical(
                      this,
                     "",
                      "Error: Filesystem not remounted!!");

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

QProcess busybox_installed;
busybox_installed.setProcessChannelMode(QProcess::MergedChannels);
cstring = adb + " -s " + daddr + port +  " shell su -c ls /system/xbin/mount";

 busybox_installed.start(cstring);
 busybox_installed.waitForFinished(-1);
 command = busybox_installed.readAll();

if (command.contains("No such file or directory"))
{
    QMessageBox::critical(this,"","Busybox required for USB drive. Install it from the menu.");
    ui->usbBox->setChecked(false);
    return;
}




    if (checked)
    {

        QFile file(filename);

          if(!file.open(QFile::WriteOnly))
                  {
                      QMessageBox::critical(this,"","Error creating file on PC!");
                      return;
                  }


        QTextStream out(&file);
        out << hashbang << endl << makepst << endl;
        file.flush();
        file.close();

        mount_system("rw");

         QProcess push_adb;
         push_adb.setProcessChannelMode(QProcess::MergedChannels);
         cstring = adb + " -s " + daddr + port +  " push " +filename+ " /sdcard/";
         push_adb.start(cstring);
         push_adb.waitForFinished(-1);
         command = push_adb.readAll();
         if (!command.contains("bytes"))
              {
                 QMessageBox::critical(this,"","Error pushing file frpm PC to device!");
                 mount_system("ro");
                 return;
               }


        QProcess move_file;
        move_file.setProcessChannelMode(QProcess::MergedChannels);
        cstring = adb + " -s " + daddr+port + " shell su -c cp " + " /sdcard/install-recovery-2.sh /system/etc/";
        move_file.start(cstring);
        move_file.waitForFinished(-1);
        command=move_file.readAll();

        if (!command.isEmpty())
             {
                QMessageBox::critical(this,"","Error: cp /sdcard/install-recovery-2.sh /system/etc/ failed");
                mount_system("ro");
                return;
              }


        QProcess chmod_file;
        move_file.setProcessChannelMode(QProcess::MergedChannels);
        cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 " + " /system/etc/install-recovery-2.sh";
        chmod_file.start(cstring);
        chmod_file.waitForFinished(-1);
        command=chmod_file.readAll();

        if (!command.isEmpty())
             {
                QMessageBox::critical(this,"","Error: chmod of install-recovery-2.sh failed");
                mount_system("ro");
                return;
              }


        QProcess rm_file1;
        rm_file1.setProcessChannelMode(QProcess::MergedChannels);
        cstring = adb + " -s " + daddr+port + " shell rm " + " /sdcard/install-recovery-2.sh";
        rm_file1.start(cstring);
        rm_file1.waitForFinished(-1);
        command=rm_file1.readAll();

        if (!command.isEmpty())
             {
                QMessageBox::critical(this,"","Error: rm of /sdcard/install-recovery-2.sh failed");
              }

         QFile::remove(filename);

         QFile Fout(filename);

         if(Fout.exists())
         {
             QMessageBox::critical(this,"","Error: rm of PC: install-recovery-2.sh failed");
         }



        QProcess check_file;
        check_file.setProcessChannelMode(QProcess::MergedChannels);
        cstring = adb + " -s " + daddr + port +  " shell ls /system/etc/install-recovery-2.sh";
        check_file.start(cstring);
        check_file.waitForFinished(-1);
        command=check_file.readAll();

        if (!command.contains("/system/etc/install-recovery-2.sh"))
             {
                QMessageBox::critical(this,"","Error: /etc/install-recovery-2.sh not created. USB drive is not persistent");
              }

         else
              {
                QMessageBox::information(this,"","USB drive is persistent");
              }


    }



  else  /////  if(!checked)

    {


       mount_system("rw");

        QProcess check_file;
        check_file.setProcessChannelMode(QProcess::MergedChannels);
        cstring = adb + " -s " + daddr + port +  " shell su -c rm /system/etc/install-recovery-2.sh";
        check_file.start(cstring);
        check_file.waitForFinished(-1);
        command=check_file.readAll();

        QMessageBox::information(this,"","USB drive is not persistent.");
        mount_system("ro");
        return;

         }


    }



