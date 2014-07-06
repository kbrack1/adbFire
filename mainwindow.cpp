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
QString port = ":5555";
QString xbmcpackage ="";

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
    query.exec("create table device(id int primary key, name varchar(20),sldir varchar(100),pushdir varchar(100),pulldir varchar(100), xbmcpackage varchar(50) )");
    query.exec("insert into device values(1, '','"+hdir+"','"+hdir+"','"+hdir+"' ,'org.xbmc.xbmc' )");

}


////////////////////////////////
void updateTables()
{
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


     if (sldir.isEmpty())
         sldir = hdir;

     if (pushdir.isEmpty())
      pushdir = hdir;

     if (pulldir.isEmpty())
      pulldir = hdir;

     if (xbmcpackage.isEmpty())
         xbmcpackage = "org.xbmc.xbmc";


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




    if(isConnected)
        ui->device_connected->setText(devstr1);
      else
       ui->device_connected->setText(devstr2);



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
     }
       else
        { ui->device_connected->setText("  Device not connected.");
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

    is_package(xbmcpackage);

   if (!is_packageInstalled)
      { QMessageBox::critical(
            this,
            "",
            "XBMC not installed");
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

       is_package(xbmcpackage);

      if (!is_packageInstalled)
         { QMessageBox::critical(
               this,
               "",
               "XBMC not installed");
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
    preferencesDialog dialog;
    dialog.setPackagename(xbmcpackage);
    dialog.setPulldir(pulldir);
    dialog.setModal(true);


    if(dialog.exec() == QDialog::Accepted)
    {

    xbmcpackage = dialog.xbmcpackageName();
    pulldir = dialog.pulldir();
    updateTables();

    }

   else return;

}

/////////////////////////////////////////////
void MainWindow::on_actionReboot_triggered()
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
     reply = QMessageBox::question(this, "Reboot Device", "Reboot Device?",
         QMessageBox::Yes|QMessageBox::No);
     if (reply == QMessageBox::Yes) {
         isConnected=false;
         ui->device_connected->setText(devstr2);
         QString cstring = adb + " reboot";
         QProcess::startDetached(cstring);
     }

}

///////////////////////////////////////////////
void MainWindow::on_actionRecovery_triggered()
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
     reply = QMessageBox::question(this, "Reboot Recovery", "Reboot to Recovery?",
         QMessageBox::Yes|QMessageBox::No);
     if (reply == QMessageBox::Yes) {

         isConnected=false;
         ui->device_connected->setText(devstr2);

         QString cstring = adb + " reboot recovery";
         QProcess::startDetached(cstring);
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


    QProcess screen_shot;
    screen_shot.setProcessChannelMode(QProcess::MergedChannels);
    QString cstring = adb +" -s " + daddr+port +" shell screencap -p /sdcard/screen.png";
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
        QString cstring = adb +" -s " + daddr+port +" pull /sdcard/screen.png "+pulldir;
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
                           "Screenshot copied to "+pulldir);


        QProcess del_shot;
        del_shot.setProcessChannelMode(QProcess::MergedChannels);
        cstring = adb +" -s " + daddr+port +" shell rm /sdcard/screen.png";
        del_shot.start(cstring);
        del_shot.waitForFinished(-1);
        command=del_shot.readAll();

    }



}



////////////////////////////////////////////////
void MainWindow::on_actionUpdates_on_triggered()
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


   if (amazon_updates("enable"))
    QMessageBox::information(
                   this,
                  "",
                   "com.amazon.dcp enabled");
    else

       QMessageBox::critical(
                      this,
                     "",
                      "com.amazon.dcp not enabled!");


}


/////////////////////////////////////////////////
void MainWindow::on_actionUpdates_off_triggered()
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





   if (amazon_updates("disable"))
    QMessageBox::information(
                   this,
                  "",
                   "com.amazon.dcp disabled");
    else

       QMessageBox::critical(
                      this,
                     "",
                      "com.amazon.dcp not disabled!");


}


/////////////////////////////////////////////////////
void MainWindow::on_actionMount_system_rw_triggered()
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
                   "/system mounted r/w");
    else

       QMessageBox::critical(
                      this,
                     "",
                      "/system not remounted!!");


}




/////////////////////////////////////////////////////
void MainWindow::on_actionMount_system_ro_triggered()
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
                   "/system mounted r/o");
    else

       QMessageBox::critical(
                      this,
                     "",
                      "/system not remounted!!");



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


        bool file1 = false;
        bool file2 = false;
        bool file3 = false;

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
              ui->progressBar->setHidden(true);
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

              ui->progressBar->setHidden(true);

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

               ui->progressBar->setHidden(true);

               if (!command.isEmpty())
                  { QMessageBox::information(
                               this,
                               "",
                               "busybox install failed");

                  }
               else
                   file3 = true;




 if (file1 && file2 && file3)

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

     QProcess chmod_script1;
     chmod_script1.setProcessChannelMode(QProcess::MergedChannels);
     cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 /system/xbin/binstall.sh";
     chmod_script1.start(cstring);
     chmod_script1.waitForFinished(-1);
     command=chmod_script1.readAll();
     chmod_script1.readAll();


     QProcess chmod_script2;
     chmod_script2.setProcessChannelMode(QProcess::MergedChannels);
     cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 /system/xbin/buninstall.sh";
     chmod_script2.start(cstring);
     chmod_script2.waitForFinished(-1);
     command=chmod_script2.readAll();
     chmod_script2.readAll();


     QProcess chmod_script3;
     chmod_script3.setProcessChannelMode(QProcess::MergedChannels);
     cstring = adb + " -s " + daddr+port + " shell su -c chmod 0755 /system/xbin/busybox";
     chmod_script3.start(cstring);
     chmod_script3.waitForFinished(-1);
     command=chmod_script3.readAll();
     chmod_script3.readAll();


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

        mount_system("rw");

         QProcess run_script1;
         run_script1.setProcessChannelMode(QProcess::MergedChannels);
         QString cstring = adb + " -s " + daddr+port + " shell su -c /system/xbin/buninstall.sh";
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

       is_package(xbmcpackage);

      if (!is_packageInstalled)
         { QMessageBox::critical(
               this,
               "",
               "XBMC not installed");
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
