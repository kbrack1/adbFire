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
 QProcess *kill_adb=new QProcess;
 kill_adb->setProcessChannelMode(QProcess::MergedChannels);
 QString cstring = adb + " kill-server";
 kill_adb->start(cstring);
command=kill_adb->readAll();
 kill_adb->waitForFinished(-1);
 serverRunning = false;
 delete kill_adb;
}


 //////////////////////////////////////
void start_server()
{
 QProcess *start_adb=new QProcess;
 start_adb->setProcessChannelMode(QProcess::MergedChannels);
 QString cstring = adb + " start-server";
 start_adb->start(cstring);
 start_adb->waitForFinished(-1);
 command=start_adb->readAll();
 delete start_adb;


     if (command.contains("daemon started successfully"))
         serverRunning = true;
         else
         serverRunning = false;

}


/////////////////////////////////////////////////////
bool is_package(QString package)
{

    QProcess *ispackage_adb=new QProcess;
    ispackage_adb->setProcessChannelMode(QProcess::MergedChannels);
    QString cstring = adb + " -s " + daddr + port + " shell pm list packages ";
    ispackage_adb->start(cstring);
    ispackage_adb->waitForFinished(-1);
    command=ispackage_adb->readAll();
    delete ispackage_adb;



        if (command.contains(package))
            is_packageInstalled = true;
            else
            is_packageInstalled = false;

        return  is_packageInstalled;
}


////////////////////////////////
void createTables()
{

    QSqlQuery query;
    query.exec("create table device(id int primary key, name varchar(20),sldir varchar(100),pushdir varchar(100),xbmcpackage varchar(50) )");
    query.exec("insert into device values(1, '','"+hdir+"','"+hdir+"','.xbmc')");



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


            query.exec("SELECT xbmcpackage FROM device");
            while (query.next()) {
                  xbmcpackage = query.value(0).toString();
            }


     if (sldir.isEmpty())
         sldir = hdir;

     if (sldir.isEmpty())
      pushdir = hdir;

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
         QProcess *adb_chmod=new QProcess;
         adb_chmod->setProcessChannelMode(QProcess::MergedChannels);
         adb_chmod->start(cstring);
         adb_chmod->waitForFinished(-1);
         command=adb_chmod->readAll();
         delete adb_chmod;
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


                      QProcess *uninstall_package=new QProcess;
                      uninstall_package->setProcessChannelMode(QProcess::MergedChannels);

                      if (!keepbox)
                      cstring = adb + " -s " +daddr+port+ " shell pm uninstall " + package;
                      else
                      cstring = adb + " -s " +daddr+port+ " shell pm uninstall -k " + package;

                      uninstall_package->start(cstring);

                      while(uninstall_package->state() != QProcess::NotRunning)
                          qApp->processEvents();

                      command=uninstall_package->readAll();
                      delete uninstall_package;
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
    QProcess *connect_device=new QProcess;
    connect_device->setProcessChannelMode(QProcess::MergedChannels);
    connect_device->start(cstring);
    connect_device->waitForFinished(-1);
    command=connect_device->readAll();
    delete connect_device;


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


          QProcess *install_apk=new QProcess;
          install_apk->setProcessChannelMode(QProcess::MergedChannels);
          QString cstring = adb + " install -r " + '"'+ fileName+'"';
          install_apk->start(cstring);

           while(install_apk->state() != QProcess::NotRunning)
               qApp->processEvents();

           command=install_apk->readAll();
           delete install_apk;
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
        QProcess *disconnect_device=new QProcess;
        disconnect_device->setProcessChannelMode(QProcess::MergedChannels);
        disconnect_device->start(cstring);
        disconnect_device->waitForFinished(-1);
        command=disconnect_device->readAll();
        delete disconnect_device;

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
    QProcess *disconnect_device=new QProcess;
    disconnect_device->setProcessChannelMode(QProcess::MergedChannels);
    disconnect_device->start(cstring);
    disconnect_device->waitForFinished(-1);
    command=disconnect_device->readAll();
    delete disconnect_device;

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


       QProcess *pull_adb=new QProcess;
       pull_adb->setProcessChannelMode(QProcess::MergedChannels);

       QString cstring = adb + " -s " + daddr + port + " pull "+xpath+" "+'"'+dir+'"';

       pull_adb->start(cstring);

        while(pull_adb->state() != QProcess::NotRunning)
            qApp->processEvents();

        command = pull_adb->readAll();
        delete pull_adb;

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



                              QProcess *install_apk1=new QProcess;
                              QString cstring = adb + " -s " + daddr + port + " install -r "+rootfile1;
                               install_apk1->start(cstring);

                               while(install_apk1->state() != QProcess::NotRunning)
                                   qApp->processEvents();

                               command=install_apk1->readAll();
                               delete install_apk1;

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



                                    QProcess *install_apk2=new QProcess;
                                     cstring = adb + " -s " + daddr + port + " install -r "+rootfile2;
                                     install_apk2->start(cstring);

                                     while(install_apk2->state() != QProcess::NotRunning)
                                         qApp->processEvents();

                                     command=install_apk2->readAll();
                                     delete install_apk2;
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


     QProcess *check_dir=new QProcess;
      check_dir->setProcessChannelMode(QProcess::MergedChannels);
      QString cstring = adb + " -s " + daddr + port +  " shell ls "+xpath;
      check_dir->start(cstring);
      check_dir->waitForFinished(-1);
      command=check_dir->readAll();
      delete check_dir;

      if (command.contains("No such file or directory"))
       { QMessageBox::critical(
                      this,
                     "",
                      "Destination path missing");
                      return;
      }


     QMessageBox::StandardButton reply;
       reply = QMessageBox::question(this, "Push", "Push "+fileName+" to "+cname+"?",
           QMessageBox::Yes|QMessageBox::No);
       if (reply == QMessageBox::Yes) {

           QProcess *push_adb=new QProcess;
           push_adb->setProcessChannelMode(QProcess::MergedChannels);

           cstring = adb + " -s " + daddr + port +  " push "+'"'+fileName+'"'+ " "+xpath;

           push_adb->start(cstring);

           // push_adb->waitForFinished(-1);

            while(push_adb->state() != QProcess::NotRunning)
                qApp->processEvents();

            command = push_adb->readAll();
            delete push_adb;

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

     QString pulldir = QDir::homePath();
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

     default:
     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/.xbmc/addons/";
     break;
     }



      QString pullfile = QInputDialog::getText(this,"" ,
        "Filename:", QLineEdit::Normal,
        "", &ok);

             if (!ok)
               return;

                         QProcess *check_dir=new QProcess;
                          check_dir->setProcessChannelMode(QProcess::MergedChannels);
                          QString cstring = adb + " -s " + daddr + port +  " shell ls "+xpath+pullfile;
                          check_dir->start(cstring);
                          check_dir->waitForFinished(-1);
                          command=check_dir->readAll();
                          delete check_dir;

                          if (command.contains("No such file or directory"))
                           { QMessageBox::critical(
                                          this,
                                         "",
                                          "No such file or directory");
                                          return;
                          }


             QProcess *pull_file=new QProcess;
             pull_file->setProcessChannelMode(QProcess::MergedChannels);
             cstring = adb + " -s " + daddr + port +  " pull "+xpath+pullfile+" "+pulldir;
             pull_file->start(cstring);
             pull_file->waitForFinished(-1);
             command=pull_file->readAll();
             delete pull_file;

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


       QProcess *rm_dir=new QProcess;
       rm_dir->setProcessChannelMode(QProcess::MergedChannels);
       cstring = adb + " -s " + daddr + port + " shell rm -r "+xpath +xbmcpackage;
       rm_dir->start(cstring);

       while(rm_dir->state() != QProcess::NotRunning)
           qApp->processEvents();

       command = rm_dir->readAll();
       delete rm_dir;

       QProcess *restore_xbmc=new QProcess;
       restore_xbmc->setProcessChannelMode(QProcess::MergedChannels);

       cstring = adb + " -s " + daddr + port +  " push "+'"'+dir+'"'+ " "+xpath+xbmcpackage;

       restore_xbmc->start(cstring);

        while(restore_xbmc->state() != QProcess::NotRunning)
            qApp->processEvents();

        command = restore_xbmc->readAll();
        delete restore_xbmc;
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

     QProcess *check_dir=new QProcess;
      check_dir->setProcessChannelMode(QProcess::MergedChannels);
      QString cstring = adb + " -s " + daddr + port +  " shell ls "+xpath;
      check_dir->start(cstring);
      check_dir->waitForFinished(-1);
      command=check_dir->readAll();
      delete check_dir;

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

           QProcess *push_adb=new QProcess;
           push_adb->setProcessChannelMode(QProcess::MergedChannels);

           cstring = adb + " -s " + daddr + port +  " push "+'"'+fileName+'"'+ " "+xpath+"/keyboard.xml";

           push_adb->start(cstring);

           // push_adb->waitForFinished(-1);

            while(push_adb->state() != QProcess::NotRunning)
                qApp->processEvents();

            command = push_adb->readAll();
            delete push_adb;

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
        cstring =  "open "+adbdir+"startadb.app";
         QProcess::startDetached(cstring);
       }


}

//////////////////////////////////////////////////
void MainWindow::on_actionPreferences_triggered()
{
    preferencesDialog dialog;
    dialog.setPackagename(xbmcpackage);
    dialog.setModal(true);


    if(dialog.exec() == QDialog::Accepted)
    {

    xbmcpackage = dialog.xbmcpackageName();

    QSqlQuery query;
    QString sqlstatement = "UPDATE device SET xbmcpackage='"+xbmcpackage+"' WHERE Id=1";
    query.exec(sqlstatement);

    }

   else return;

}
