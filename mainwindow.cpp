#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog2.h"
#include "helpdialog.h"
#include "uninstalldialog.h"
#include "llamadialog.h"
#include "usbfiledialog.h"
#include "uuiddialog.h"
#include "kodidialog.h"
#include "creditsdialog.h"
#include "cifsdialog.h"
#include "videodialog.h"
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

#ifdef Q_OS_LINUX
 int os=0;
#elif defined(Q_OS_WIN)
  int os=1;
#elif defined(Q_OS_MAC)
int os=2;
#endif

// QString num="123";
//  int n = num.toInt();


const QString version = "1.17";

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
bool usbdev;

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
int buffermode = 1;
QString buffersize = "";
QString bufferfactor = "";
QString dbstring = "";
QString description = "";

QString ipaddress1;
QString share1;
QString mount1;
QString nfs1;
bool    select1;

QString ipaddress2;
QString share2;
QString mount2;
QString nfs2;
bool    select2;

QString ipaddress3;
QString share3;
QString mount3;
QString nfs3;
bool    select3;

QString ipaddress4;
QString share4;
QString mount4;
QString nfs4;
bool    select4;


QString adbstr1 = "ADB running. ";
QString adbstr2 = "ADB not running. ";
QString devstr1 = "  Device connected";
QString devstr2 = "  Device not connected.";
QString devstr3 = "  USB Mode";

QString amazon_update1 = "Amazon updates on.      ";
QString amazon_update0 = "Amazon updates off.     ";
QString amazon_update2 = "                        ";

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

QString cstring = adb + " " + reboot;

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

bool isroot;
QString cstring = adb + " shell ls /system/xbin/su";
QString command=RunProcess(cstring);

if (command.contains("/system/xbin/su"))
   isroot = true;
    else
    isroot = false;

logfile("is_su");
logfile(command);

return isroot;


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

    QString cstring = adb + " shell pm list packages ";
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
       QString cstring = adb + " shell su -c mount -o remount,"+mnt+ " /system";
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
       QString cstring = adb + " shell su -c mount -o remount,"+mnt+ " /";
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


       QString cstring = adb + " shell su -c pm "+ onoff + " com.amazon.dcp";
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

    QString sqlstatement = "create table device(id int primary key, name varchar(20) ,description varchar(20),sldir varchar(100),pushdir varchar(100),pulldir varchar(100), xbmcpackage varchar(50) , sshpassword varchar(10) , versioncheck int, buffermode int, buffersize varchar(10), bufferfactor varchar(10))";

    QSqlQuery query;
    query.exec(sqlstatement);

    if (query.lastError().isValid())
     {
        logfile(sqlstatement);
        logfile("SqLite error:" + query.lastError().text());
        logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
       }

       QString s;


    for( int n = 0; n < 10; n = n + 1 )
     {

       s  = QString::number(n);
       QString r  = "IP address "+ QString::number(n+1);

       QString d = "Description";
       sqlstatement="insert into device values('"+s+"','"+r+"', '"+d+"'  ,'"+hdir+"','"+hdir+"','"+hdir+"' ,'org.xbmc.kodi','password', 1 ,1,'20971520','1')";

        query.exec(sqlstatement);

        if (query.lastError().isValid())
         {
          logfile(sqlstatement);
          logfile("SqLite error:" + query.lastError().text());
          logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
        }
     }



   sqlstatement = "create table netshare(id int primary key, ipaddress varchar(20), share varchar(60), mount varchar(60), nfs varchar(1) )";

    query.exec(sqlstatement);

    if (query.lastError().isValid())
     {
        logfile(sqlstatement);
        logfile("SqLite error:" + query.lastError().text());
        logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
       }




    for( int n2 = 1; n2 < 5; n2 = n2 + 1 )
     {

       s  = QString::number(n2);

       QString r  = "ipaddress"+s;

        sqlstatement="insert into netshare values('"+s+"', '','','','0' )";
        query.exec(sqlstatement);

        if (query.lastError().isValid())
         {
          logfile(sqlstatement);
          logfile("SqLite error:" + query.lastError().text());
          logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
        }
     }



}


////////////////////////////////
void updateTables(int i)
{


 // QMessageBox::critical(0, "",daddr,QMessageBox::Cancel);


    logfile("updating database adbfire.db");

    QString str1;
    QString str2;
    QString str3;
    QString str4;


    str1.setNum(usbcheck);
    str2.setNum(ftvupdate);
    str3.setNum(checkversion);
    str4.setNum(buffermode);


    QString idstring = QString::number(i);

    QSqlQuery query;

    QString sqlstatement = "UPDATE device SET name='"+daddr+"'  WHERE Id="+ idstring;
     query.exec(sqlstatement);

     if (query.lastError().isValid())
      {
        logfile(sqlstatement);
        logfile("SqLite error:" + query.lastError().text());
        logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
       }


    sqlstatement = "UPDATE device SET sldir='"+sldir+"'  WHERE Id="+ idstring;
     query.exec(sqlstatement);


     if (query.lastError().isValid())
      {
        logfile(sqlstatement);
        logfile("SqLite error:" + query.lastError().text());
        logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
       }

    sqlstatement = "UPDATE device SET pushdir='"+pushdir+"'  WHERE Id="+ idstring;
     query.exec(sqlstatement);

     if (query.lastError().isValid())
      {
        logfile(sqlstatement);
        logfile("SqLite error:" + query.lastError().text());
        logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
       }

    sqlstatement = "UPDATE device SET pulldir='"+pulldir+"'  WHERE Id="+ idstring;
     query.exec(sqlstatement);


     if (query.lastError().isValid())
      {
        logfile(sqlstatement);
        logfile("SqLite error:" + query.lastError().text());
        logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
       }

    sqlstatement = "UPDATE device SET xbmcpackage='"+xbmcpackage+"'  WHERE Id="+ idstring;
     query.exec(sqlstatement);

     if (query.lastError().isValid())
      {
        logfile(sqlstatement);
        logfile("SqLite error:" + query.lastError().text());
        logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
       }


     sqlstatement = "UPDATE device SET sshpassword='"+sshpassword+"'  WHERE Id="+ idstring;
      query.exec(sqlstatement);

      if (query.lastError().isValid())
       {
         logfile(sqlstatement);
         logfile("SqLite error:" + query.lastError().text());
         logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
        }

     sqlstatement = "UPDATE device SET versioncheck='"+str3+"'  WHERE Id="+ idstring;
      query.exec(sqlstatement);

      if (query.lastError().isValid())
       {
         logfile(sqlstatement);
         logfile("SqLite error:" + query.lastError().text());
         logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
        }


      sqlstatement = "UPDATE device SET buffermode='"+str4+"'  WHERE Id="+ idstring;
       query.exec(sqlstatement);

       if (query.lastError().isValid())
        {
          logfile(sqlstatement);
          logfile("SqLite error:" + query.lastError().text());
          logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
         }

       sqlstatement = "UPDATE device SET buffersize='"+buffersize+"'  WHERE Id="+ idstring;
        query.exec(sqlstatement);

        if (query.lastError().isValid())
         {
           logfile(sqlstatement);
           logfile("SqLite error:" + query.lastError().text());
           logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
          }

        sqlstatement = "UPDATE device SET bufferfactor='"+bufferfactor+"'  WHERE Id="+ idstring;
         query.exec(sqlstatement);

         if (query.lastError().isValid())
          {
            logfile(sqlstatement);
            logfile("SqLite error:" + query.lastError().text());
            logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
           }

         sqlstatement = "UPDATE device SET description='"+description+"'  WHERE Id="+ idstring;
          query.exec(sqlstatement);

          if (query.lastError().isValid())
           {
             logfile(sqlstatement);
             logfile("SqLite error:" + query.lastError().text());
             logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
            }



}



////////////////////////////////
void updateShares()
{


 // QMessageBox::critical(0, "",daddr,QMessageBox::Cancel);


    logfile("updating shares table adbfire.db");



 // sqlstatement = "create table netshare(id int primary key, ipaddress varchar(20), share varchar(60), mount varchar(60), nfs varchar(1) )";


    QSqlQuery query;
    QString idstring = "1";


    QString sqlstatement = "UPDATE netshare SET ipaddress='"+ipaddress1+"'  WHERE Id="+ idstring;
     query.exec(sqlstatement);

     if (query.lastError().isValid())
      {
        logfile(sqlstatement);
        logfile("SqLite error:" + query.lastError().text());
        logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
       }


      sqlstatement = "UPDATE netshare SET share='"+share1+"'  WHERE Id="+ idstring;
      query.exec(sqlstatement);

      if (query.lastError().isValid())
       {
         logfile(sqlstatement);
         logfile("SqLite error:" + query.lastError().text());
         logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
        }

      sqlstatement = "UPDATE netshare SET mount='"+mount1+"'  WHERE Id="+ idstring;
       query.exec(sqlstatement);

       if (query.lastError().isValid())
        {
          logfile(sqlstatement);
          logfile("SqLite error:" + query.lastError().text());
          logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
         }


       sqlstatement = "UPDATE netshare SET nfs='"+nfs1+"'  WHERE Id="+ idstring;
        query.exec(sqlstatement);

        if (query.lastError().isValid())
         {
           logfile(sqlstatement);
           logfile("SqLite error:" + query.lastError().text());
           logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
          }


        idstring = "2";


        sqlstatement = "UPDATE netshare SET ipaddress='"+ipaddress2+"'  WHERE Id="+ idstring;
         query.exec(sqlstatement);

         if (query.lastError().isValid())
          {
            logfile(sqlstatement);
            logfile("SqLite error:" + query.lastError().text());
            logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
           }


          sqlstatement = "UPDATE netshare SET share='"+share2+"'  WHERE Id="+ idstring;
          query.exec(sqlstatement);

          if (query.lastError().isValid())
           {
             logfile(sqlstatement);
             logfile("SqLite error:" + query.lastError().text());
             logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
            }

          sqlstatement = "UPDATE netshare SET mount='"+mount2+"'  WHERE Id="+ idstring;
           query.exec(sqlstatement);

           if (query.lastError().isValid())
            {
              logfile(sqlstatement);
              logfile("SqLite error:" + query.lastError().text());
              logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
             }


           sqlstatement = "UPDATE netshare SET nfs='"+nfs2+"'  WHERE Id="+ idstring;
            query.exec(sqlstatement);

            if (query.lastError().isValid())
             {
               logfile(sqlstatement);
               logfile("SqLite error:" + query.lastError().text());
               logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
              }


            idstring = "3";


             sqlstatement = "UPDATE netshare SET ipaddress='"+ipaddress3+"'  WHERE Id="+ idstring;
             query.exec(sqlstatement);

             if (query.lastError().isValid())
              {
                logfile(sqlstatement);
                logfile("SqLite error:" + query.lastError().text());
                logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
               }


              sqlstatement = "UPDATE netshare SET share='"+share3+"'  WHERE Id="+ idstring;
              query.exec(sqlstatement);

              if (query.lastError().isValid())
               {
                 logfile(sqlstatement);
                 logfile("SqLite error:" + query.lastError().text());
                 logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
                }

              sqlstatement = "UPDATE netshare SET mount='"+mount3+"'  WHERE Id="+ idstring;
               query.exec(sqlstatement);

               if (query.lastError().isValid())
                {
                  logfile(sqlstatement);
                  logfile("SqLite error:" + query.lastError().text());
                  logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
                 }


               sqlstatement = "UPDATE netshare SET nfs='"+nfs3+"'  WHERE Id="+ idstring;
                query.exec(sqlstatement);

                if (query.lastError().isValid())
                 {
                   logfile(sqlstatement);
                   logfile("SqLite error:" + query.lastError().text());
                   logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
                  }



                idstring = "4";


                 sqlstatement = "UPDATE netshare SET ipaddress='"+ipaddress4+"'  WHERE Id="+ idstring;
                 query.exec(sqlstatement);

                 if (query.lastError().isValid())
                  {
                    logfile(sqlstatement);
                    logfile("SqLite error:" + query.lastError().text());
                    logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
                   }


                  sqlstatement = "UPDATE netshare SET share='"+share4+"'  WHERE Id="+ idstring;
                  query.exec(sqlstatement);

                  if (query.lastError().isValid())
                   {
                     logfile(sqlstatement);
                     logfile("SqLite error:" + query.lastError().text());
                     logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
                    }

                  sqlstatement = "UPDATE netshare SET mount='"+mount4+"'  WHERE Id="+ idstring;
                   query.exec(sqlstatement);

                   if (query.lastError().isValid())
                    {
                      logfile(sqlstatement);
                      logfile("SqLite error:" + query.lastError().text());
                      logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
                     }


                   sqlstatement = "UPDATE netshare SET nfs='"+nfs4+"'  WHERE Id="+ idstring;
                    query.exec(sqlstatement);

                    if (query.lastError().isValid())
                     {
                       logfile(sqlstatement);
                       logfile("SqLite error:" + query.lastError().text());
                       logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
                      }

}





////////////////////////////////
QString loadDevices(int i)
{

  QString sqlstatement;
  QString device_address;

    logfile("reading database");

     QSqlQuery query;


     QString idstring = QString::number(i);

     sqlstatement= "SELECT name FROM device WHERE Id="+ idstring;
     query.exec(sqlstatement);
         while (query.next()) {
            device_address = query.value(0).toString();
         }


        logfile(sqlstatement);

         if (query.lastError().isValid())
          {
            logfile(sqlstatement);
            logfile("SqLite error:" + query.lastError().text());
            logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
           }


     return device_address;

}


////////////////////////////////
void readTables(int i)

{


  QString sqlstatement;


    logfile("reading database");

     QSqlQuery query;



    QString id = QString::number(i);


     sqlstatement= "SELECT name FROM device WHERE id=" + id;
     query.exec(sqlstatement);
         while (query.next()) {
              daddr = query.value(0).toString();
         }


        logfile(sqlstatement);
        logfile(daddr);

         // QMessageBox::critical(0, "",daddr,QMessageBox::Cancel);


         if (query.lastError().isValid())
          {
            logfile(sqlstatement);
            logfile("SqLite error:" + query.lastError().text());
            logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
           }


         sqlstatement="SELECT sldir FROM device  WHERE id=" + id;
         query.exec(sqlstatement);
           while (query.next()) {
                sldir = query.value(0).toString();
          }

            logfile(sqlstatement);
            logfile(sldir);

           if (query.lastError().isValid())
            {
              logfile(sqlstatement);
              logfile("SqLite error:" + query.lastError().text());
              logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
             }


           sqlstatement="SELECT pushdir FROM device  WHERE id=" + id;
           query.exec(sqlstatement);
            while (query.next()) {
                 pushdir = query.value(0).toString();
           }

            logfile(sqlstatement);
            logfile(pushdir);

            if (query.lastError().isValid())
             {
               logfile(sqlstatement);
               logfile("SqLite error:" + query.lastError().text());
               logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
              }


           sqlstatement="SELECT pulldir FROM device  WHERE id=" + id;
            query.exec(sqlstatement);
            while (query.next()) {
                  pulldir = query.value(0).toString();
            }

            logfile(sqlstatement);
            logfile(pulldir);

            if (query.lastError().isValid())
             {
               logfile(sqlstatement);
               logfile("SqLite error:" + query.lastError().text());
               logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
              }



            sqlstatement="SELECT xbmcpackage FROM device  WHERE id=" + id;
            query.exec(sqlstatement);
            while (query.next()) {
                  xbmcpackage = query.value(0).toString();
            }

            logfile(sqlstatement);
            logfile(xbmcpackage);

            if (query.lastError().isValid())
             {
               logfile(sqlstatement);
               logfile("SqLite error:" + query.lastError().text());
               logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
              }


            sqlstatement="SELECT sshpassword FROM device  WHERE id=" + id;
            query.exec(sqlstatement);
            while (query.next()) {
                  sshpassword = query.value(0).toString();
            }

            logfile(sqlstatement);
            logfile(sshpassword);

            if (query.lastError().isValid())
             {
               logfile(sqlstatement);
               logfile("SqLite error:" + query.lastError().text());
               logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
              }


            sqlstatement="SELECT versioncheck FROM device  WHERE id=" + id;
            query.exec(sqlstatement);
            while (query.next()) {
                  checkversion = query.value(0).toInt();
            }

            logfile(sqlstatement);

            if (query.lastError().isValid())
             {
               logfile(sqlstatement);
               logfile("SqLite error:" + query.lastError().text());
               logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
              }

            logfile(sqlstatement);

            sqlstatement="SELECT buffermode FROM device  WHERE id=" + id;
            query.exec(sqlstatement);
            while (query.next()) {
                  buffermode = query.value(0).toInt();
            }

            logfile(sqlstatement);

            if (query.lastError().isValid())
             {
               logfile(sqlstatement);
               logfile("SqLite error:" + query.lastError().text());
               logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
              }



            sqlstatement="SELECT buffersize FROM device  WHERE id=" + id;
            query.exec(sqlstatement);
            while (query.next()) {
                  buffersize = query.value(0).toString();
            }


              logfile(sqlstatement);
              logfile(buffersize);

            if (query.lastError().isValid())
             {
               logfile(sqlstatement);
               logfile("SqLite error:" + query.lastError().text());
               logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
              }


            sqlstatement="SELECT bufferfactor FROM device  WHERE id=" + id;
            query.exec(sqlstatement);
            while (query.next()) {
                  bufferfactor = query.value(0).toString();
            }

            logfile(sqlstatement);
            logfile(bufferfactor);

            if (query.lastError().isValid())
             {
               logfile(sqlstatement);
               logfile("SqLite error:" + query.lastError().text());
               logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
              }


            sqlstatement="SELECT description FROM device  WHERE id=" + id;
            query.exec(sqlstatement);
            while (query.next()) {
                  description = query.value(0).toString();
            }

            logfile(sqlstatement);
            logfile(description);

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
         xbmcpackage = "org.xbmc.kodi";

     if (checkversion==0)
         versioncheck=false;
     else
         versioncheck=true;


}


////////////////////////////////
void readShares()

{


    QString sqlstatement;


    logfile("reading share table");

     QSqlQuery query;

     QString idstring = "1";

     sqlstatement= "SELECT ipaddress FROM netshare WHERE Id="+ idstring;
     query.exec(sqlstatement);
         while (query.next()) {
             ipaddress1 = query.value(0).toString();
         }


        logfile(sqlstatement);

         if (query.lastError().isValid())
          {
            logfile(sqlstatement);
            logfile("SqLite error:" + query.lastError().text());
            logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
           }




         sqlstatement="SELECT share FROM netshare  WHERE Id="+ idstring;
         query.exec(sqlstatement);
           while (query.next()) {
                share1 = query.value(0).toString();
          }



           if (query.lastError().isValid())
            {
              logfile(sqlstatement);
              logfile("SqLite error:" + query.lastError().text());
              logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
             }




           sqlstatement="SELECT mount FROM netshare WHERE Id="+ idstring;
           query.exec(sqlstatement);
            while (query.next()) {
                 mount1 = query.value(0).toString();
           }



            if (query.lastError().isValid())
             {
               logfile(sqlstatement);
               logfile("SqLite error:" + query.lastError().text());
               logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
              }


           sqlstatement="SELECT nfs FROM netshare  WHERE Id="+ idstring;
            query.exec(sqlstatement);
            while (query.next()) {
                  nfs1 = query.value(0).toString();
            }


            if (query.lastError().isValid())
             {
               logfile(sqlstatement);
               logfile("SqLite error:" + query.lastError().text());
               logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
              }

        idstring = "2";

        sqlstatement= "SELECT ipaddress FROM netshare WHERE Id="+ idstring;
        query.exec(sqlstatement);
            while (query.next()) {
                ipaddress2 = query.value(0).toString();
            }


           logfile(sqlstatement);

            if (query.lastError().isValid())
             {
               logfile(sqlstatement);
               logfile("SqLite error:" + query.lastError().text());
               logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
              }

            sqlstatement="SELECT share FROM netshare  WHERE Id="+ idstring;
            query.exec(sqlstatement);
              while (query.next()) {
                   share2 = query.value(0).toString();
             }



              if (query.lastError().isValid())
               {
                 logfile(sqlstatement);
                 logfile("SqLite error:" + query.lastError().text());
                 logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
                }

              sqlstatement="SELECT mount FROM netshare WHERE Id="+ idstring;
              query.exec(sqlstatement);
               while (query.next()) {
                    mount2 = query.value(0).toString();
              }



               if (query.lastError().isValid())
                {
                  logfile(sqlstatement);
                  logfile("SqLite error:" + query.lastError().text());
                  logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
                 }


              sqlstatement="SELECT nfs FROM netshare  WHERE Id="+ idstring;
               query.exec(sqlstatement);
               while (query.next()) {
                     nfs2 = query.value(0).toString();
               }


               if (query.lastError().isValid())
                {
                  logfile(sqlstatement);
                  logfile("SqLite error:" + query.lastError().text());
                  logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
                 }

               idstring = "3";

               sqlstatement= "SELECT ipaddress FROM netshare WHERE Id="+ idstring;
               query.exec(sqlstatement);
                   while (query.next()) {
                       ipaddress3 = query.value(0).toString();
                   }


                  logfile(sqlstatement);

                   if (query.lastError().isValid())
                    {
                      logfile(sqlstatement);
                      logfile("SqLite error:" + query.lastError().text());
                      logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
                     }




                   sqlstatement="SELECT share FROM netshare  WHERE Id="+ idstring;
                   query.exec(sqlstatement);
                     while (query.next()) {
                          share3 = query.value(0).toString();
                    }



                     if (query.lastError().isValid())
                      {
                        logfile(sqlstatement);
                        logfile("SqLite error:" + query.lastError().text());
                        logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
                       }




                     sqlstatement="SELECT mount FROM netshare WHERE Id="+ idstring;
                     query.exec(sqlstatement);
                      while (query.next()) {
                           mount3 = query.value(0).toString();
                     }



                      if (query.lastError().isValid())
                       {
                         logfile(sqlstatement);
                         logfile("SqLite error:" + query.lastError().text());
                         logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
                        }


                     sqlstatement="SELECT nfs FROM netshare  WHERE Id="+ idstring;
                      query.exec(sqlstatement);
                      while (query.next()) {
                            nfs3 = query.value(0).toString();
                      }


                      if (query.lastError().isValid())
                       {
                         logfile(sqlstatement);
                         logfile("SqLite error:" + query.lastError().text());
                         logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
                        }

                      idstring = "4";

                      sqlstatement= "SELECT ipaddress FROM netshare WHERE Id="+ idstring;
                      query.exec(sqlstatement);
                          while (query.next()) {
                              ipaddress4 = query.value(0).toString();
                          }


                         logfile(sqlstatement);

                          if (query.lastError().isValid())
                           {
                             logfile(sqlstatement);
                             logfile("SqLite error:" + query.lastError().text());
                             logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
                            }




                          sqlstatement="SELECT share FROM netshare  WHERE Id="+ idstring;
                          query.exec(sqlstatement);
                            while (query.next()) {
                                 share4 = query.value(0).toString();
                           }



                            if (query.lastError().isValid())
                             {
                               logfile(sqlstatement);
                               logfile("SqLite error:" + query.lastError().text());
                               logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
                              }




                            sqlstatement="SELECT mount FROM netshare WHERE Id="+ idstring;
                            query.exec(sqlstatement);
                             while (query.next()) {
                                  mount4 = query.value(0).toString();
                            }


                             if (query.lastError().isValid())
                              {
                                logfile(sqlstatement);
                                logfile("SqLite error:" + query.lastError().text());
                                logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
                               }


                            sqlstatement="SELECT nfs FROM netshare  WHERE Id="+ idstring;
                             query.exec(sqlstatement);
                             while (query.next()) {
                                   nfs4 = query.value(0).toString();
                             }


                             if (query.lastError().isValid())
                              {
                                logfile(sqlstatement);
                                logfile("SqLite error:" + query.lastError().text());
                                logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
                               }


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
          // adbdir = QDir::currentPath();
         adb = adbdir+"adb.exe";
        }

   if (os == 2)

        {
           adbdir = QCoreApplication::applicationDirPath();
           adbdir = adbdir+"/adbfiles/";


           if (QFile::exists(adbdir+"adb"))
           {
               adb = adbdir+"adb";
               QProcess::execute ("chmod 0755 "+ adb);

           }

           else

           {
           QFile::copy(adbdir+"adb.osx", adbdir+"adb");
           adb = adbdir+"adb";
           QProcess::execute ("chmod 0755 "+ adb);

           }



   }

   if (os == 0)

        {
           adbdir = QCoreApplication::applicationDirPath();
           adbdir = adbdir+"/adbfiles/";
           QProcess::execute ("chmod 0755 "+ adbdir+"adb.linux");

           if (QFile::exists(adbdir+"adb"))
           {
               adb = adbdir+"adb";
               QProcess::execute ("chmod 0755 "+ adb);

           }

           else

           {
           QFile::copy(adbdir+"adb.linux", adbdir+"adb");
           adb = adbdir+"adb";
           QProcess::execute ("chmod 0755 "+ adb);
           }

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

  //  this->setFixedSize(this->size().width(), this->size().height());

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


   for( int i = 0; i < 10; i = i + 1 )
   {
    ui->deviceBox->addItem(loadDevices(i));
   }



   ui->deviceBox->setCurrentIndex(0);
   daddr = ui->deviceBox->currentText();


 //  readTables(ui->deviceBox->currentIndex()+1);

   connect(ui->deviceBox, SIGNAL(currentIndexChanged(int)),
       SLOT(readInc(int)));


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
        }

    else
    { ui->server_running->setText(adbstr2);
      ui->device_connected->setText(devstr2);
      ui->update_status->setText("");
      ui->usbBox->setDisabled(true);
    }

    // open_pref_database();
   db.open();
    readTables(ui->deviceBox->currentIndex());
    db.close();


    if (checkversion==1)
         get_data();



}


MainWindow::~MainWindow()
{



    logfile("closing program");
    kill_server();
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

/////////////////////////////////
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


}


/////////////////////////////////////////////////////
void MainWindow::readInc(int idx)
{
    daddr = ui->deviceBox->currentText();
    // readTables( idx + 1 );
    on_disButton_clicked();
//    on_connButton_clicked();

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
                      cstring = adb + " shell pm uninstall " + package;
                      else
                      cstring = adb + " shell pm uninstall -k " + package;

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


  if (ui->usbmode->isChecked() )
    {

        QMessageBox::critical(
             this,
             tr("adbFire"),
             tr("Inactive in USB Mode"));
          return;
    }

     if (ui->deviceBox->currentText().isEmpty())
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

    kill_server();
    start_server();

    port = ":5555";

    daddr = ui->deviceBox->currentText();
    QString cstring = adb + " connect " +daddr+port;

    QString command=RunProcess(cstring);

    if (command.contains("connected to"))
           isConnected=true;
       else
        isConnected=false;

     if(isConnected)
       {
         cstring = adb + " shell pm list packages -d";
         command=RunProcess(cstring);

         logfile("check amazon update status");
         logfile(cstring);
         logfile(command);

         if (command.contains("package:com.amazon.dcp"))
         ui->update_status->setText(amazon_update0);
         else
          ui->update_status->setText(amazon_update1);


         cstring = adb + " shell ls /etc/init.d/01mntdrives";
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

    if (ui->usbmode->isChecked() )
      {

          QMessageBox::critical(
               this,
               tr("adbFire"),
               tr("Inactive in USB Mode"));
            return;
      }


    if (ui->deviceBox->currentText().isEmpty())
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

        ui->usbBox->setDisabled(true);
        daddr = ui->deviceBox->currentText();
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
     { ui->server_running->setText(adbstr1);

        if (ui->usbmode->isChecked() )
          { isConnected=true;
            ui->device_connected->setText(devstr3);
          }
        else
           { isConnected=false;
             ui->device_connected->setText(devstr2);
           }

     }

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

       QString cstring = adb + " pull "+xpath+" "+'"'+dir+'"';
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

                              QString cstring = adb + " install -r "+rootfile1;

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

                              cstring = adb + " install -r "+rootfile2;

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


    QString cstring;
    QString command;

    QString xpath = "";
    QString hidden;

    QElapsedTimer rtimer;
    int nMilliseconds;
    rtimer.start();


    if (xbmcpackage.isEmpty())
       xbmcpackage="org.xbmc.kodi";

      cstring = adb + " shell ls /sdcard/Android/data/"+xbmcpackage+"/files/.xbmc";
      command=RunProcess(cstring);
     if (command.contains("No such file or directory"))
      {
         hidden=".kodi";
       }

      cstring = adb + " shell ls /sdcard/Android/data/"+xbmcpackage+"/files/.kodi";
      command=RunProcess(cstring);
      if (command.contains("No such file or directory"))
       {
          hidden=".xbmc";
        }


 if (xbmcpackage.isEmpty())
    xbmcpackage="org.xbmc.kodi";


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
       logfile("xbmc not installed. cant push to it.");
        return;
     }



 QString fileName = QFileDialog::getOpenFileName(this,
 "Push file to "+cname, pushdir, tr("Files (*)"));


 if (!fileName.isEmpty() )
 {

     QFileInfo finfo(fileName);
     pushdir = finfo.absolutePath();

     QString cstring = adb + " shell ls "+xpath;

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

           cstring = adb + " push "+'"'+fileName+'"'+ " "+xpath;

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

       cstring = adb + " shell rm -r "+xpath +xbmcpackage;

       command=RunProcess(cstring);

       cstring = adb + " push "+'"'+dir+'"'+ " "+xpath+xbmcpackage;

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


      QString cstring = adb + " shell ls "+xpath;

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


           cstring = adb + " push "+'"'+fileName+'"'+ " "+xpath+"/keyboard.xml";

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
       cstring = "cmd /k  adb shell";
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


logfile("opening preferences dialog");

    QString cstring;
    QString command;
    QString oldpass;
    QString olddaddr;

    QString recnum  = QString::number(ui->deviceBox->currentIndex());

    // open_pref_database();
   db.open();
    readTables(ui->deviceBox->currentIndex());
    db.close();

    oldpass = sshpassword;
    olddaddr = daddr;
    sshcheck = false;
    updatecheck = true;


  if (is_su() && isConnected)
   {

        logfile("checking ssh boot status");
        cstring = adb + " shell ls /system/etc/init.d/02sshd";
        command=RunProcess(cstring);


     logfile(cstring);
     logfile(command);

     if (command.contains("No such file or directory"))
         sshcheck = false;
      else
      sshcheck = true;
  }

  else sshcheck = false;



 if (is_su() && isConnected)
   {
       logfile("checking amazon updates status");

       cstring = adb + " shell pm list packages -d";
       command=RunProcess(cstring);

        logfile(cstring);
        logfile(command);

        if (command.contains("package:com.amazon.dcp"))
           {
            ui->update_status->setText(amazon_update0);
            updatecheck = false;
           }

       }

      else
        {
             ui->update_status->setText(amazon_update1);
             updatecheck = true;
        }




    bool currentupdate = updatecheck;

    preferencesDialog dialog;
    dialog.setPackagename(xbmcpackage);
    dialog.setSSHpassword(sshpassword);
    dialog.setPulldir(pulldir);
    dialog.setftvUpdate(updatecheck);
    dialog.setsshBoot(sshcheck);
    dialog.setversioncheck(versioncheck);
    dialog.setversionLabel(version);
    dialog.setdaddr(daddr);
    dialog.setbuffermode(buffermode);
    dialog.setbuffersize(buffersize);
    dialog.setbufferfactor(bufferfactor);
    dialog.setdescription(description);
    dialog.setrecnum(recnum);  //hidden on form
    dialog.setModal(true);


    if(dialog.exec() == QDialog::Accepted)
    {



    int x = dialog.returnval1();

//    QString s  = QString::number(x);
//    QMessageBox::critical(this,"",s);

    xbmcpackage = dialog.xbmcpackageName();
    sshpassword = dialog.sshpassword();
    pulldir = dialog.pulldir();
    updatecheck = dialog.updatecheck();
    sshcheck = dialog.sshcheck();
    versioncheck = dialog.versioncheck();
    buffermode = dialog.buffermode();
    buffersize = dialog.buffersize();
    bufferfactor = dialog.bufferfactor();
    description = dialog.description();
    daddr = dialog.daddr();

     // QMessageBox::critical(this,"",daddr);

    if (versioncheck)
        checkversion = 1;
    else
        checkversion = 0;

    if (x == 1)
           {

                // open_pref_database();
                db.open();
                updateTables(ui->deviceBox->currentIndex());
                db.close();

                // QMessageBox::critical(this,"",daddr);

                 if (daddr != olddaddr)
                {
                    isConnected=false;
                    ui->device_connected->setText(devstr2);
                    ui->update_status->setText(amazon_update2);
                    ui->deviceBox->setItemText(ui->deviceBox->currentIndex(),daddr);
                }
        }  // end x1


    if (x == 2)
          {

          if (!isConnected)
              return;

           logfile("write advancedsettings.xml");

              QString str1;
              str1.setNum(buffermode-1);

                     QString xpath = "";
                     QString hidden;


                     if (xbmcpackage.contains(".kodi"))
                        hidden=".kodi";
                      else
                          hidden=".xbmc";

                     xpath = "/sdcard/Android/data/"+xbmcpackage+"/files/"+hidden+"/userdata/";
                     QString filename1 = "advancedsettings.xml";
                     QString filename2 = adbdir+filename1;

                     cstring = adb + " shell ls "+xpath+filename1;
                     command=RunProcess(cstring);

                     logfile(cstring);
                     logfile(command);

                      if (!command.contains("No such file or directory"))
                         {
                          logfile("advancedsettings.xml exists");

                          QMessageBox::StandardButton reply;
                           reply = QMessageBox::question(this, "XML", "advancedsettings.xml exists. Overwrite?\n(will backup original)",
                                                         QMessageBox::Yes|QMessageBox::No);
                           if (reply == QMessageBox::No) {
                               logfile("abort xml write");
                               return;
                              } else {
                              logfile("continue xml write");
                              logfile("backup advancedsettings.xml");
                              cstring = adb + " shell cp "+xpath+filename1+" "+xpath+filename1+".old";
                              command=RunProcess(cstring);
                              }
                          }  // end if exists



          QString line1 ="<advancedsettings>";
          QString line2 ="  <network>";
          QString line3 ="    <buffermode>"+str1+"</buffermode>";
          QString line4 ="    <cachemembuffersize>"+buffersize+"</cachemembuffersize>";
          QString line5 ="    <readbufferfactor>"+bufferfactor+"</readbufferfactor>";
          QString line6 ="  </network>";
          QString line7 ="</advancedsettings>";

          QFile file(filename2);


          if(!file.open(QFile::WriteOnly))
                {
                  logfile("error creating advancedsettings.xml.");
                  QMessageBox::critical(this,"","Unknown error creating xml file!");
                   return;
                 }

                  QTextStream out(&file);

                  out  << line1 << endl;
                  out  << line2 << endl;
                  out  << line3 << endl;
                  out  << line4 << endl;
                  out  << line5 << endl;
                  out  << line6 << endl;
                  out  << line7 << endl;

                  file.flush();
                  file.close();

                  cstring = adb + " push "+filename2+ " "+xpath+filename1;
                  command=RunProcess(cstring);

                   if (!command.contains("bytes"))
                      {
                        logfile(cstring);
                        logfile(command);
                        logfile("error pushing xml script to device!");
                        QMessageBox::critical(this,"","Error pushing xml from PC to device!");
                        return;
                      }




           }  //end x2



    if (is_su() && isConnected)  // superuser block
    {

        if (updatecheck)  // turn updates on
                    {

                      if (amazon_updates("enable"))
                        {
                          logfile("Amazon updates on, com.amazon.dcp enabled");
                          cstring = adb + " shell pm list packages -d";
                          command=RunProcess(cstring);

                          logfile(cstring);
                          logfile(command);


                          if (!command.contains("package:com.amazon.dcp"))
                            {
                              ui->update_status->setText(amazon_update1);
                            }

                          else { QMessageBox::critical(this,"","Problem: com.amazon.dcp not enabled!");
                                  logfile("Problem: com.amazon.dcp not enabled!");
                                  ui->update_status->setText(amazon_update0);
                               }

                        }  //  if (amazon_updates("enable"))

                  }  // if (updatecheck)


         if (!updatecheck)  // turn updates off
              {

                if (amazon_updates("disable"))
                              {
                               logfile("Amazon updates off, com.amazon.dcp disabled");
                               cstring = adb + " shell pm list packages -d";
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


         if (!sshcheck )    // rm ssh at boot
                 {

                       mount_system("rw");
                       cstring = adb + " shell su -c rm /system/etc/init.d/02sshd";
                       command=RunProcess(cstring);

                      logfile(cstring);
                      logfile(command);

                      if (command.isEmpty())
                          logfile("/system/etc/init.d/02sshd deleted");
                      else
                          logfile("/system/etc/init.d/02sshd not deleted");

                 }  // end rm ssh at boot


         if (sshcheck )  // start ssh at boot
               {

                  logfile("write 02sshd");
                  QString hashbang = "#!/system/bin/sh";
                  QString filename = adbdir+"02sshd";
                  QString makepst = "/system/xbin/sshstart " + sshpassword;


                  QFile file(filename);

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

                       cstring = adb + " push " +filename+ " /sdcard/";
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



                      cstring = adb + " shell su -c cp " + " /sdcard/02sshd /system/etc/init.d";
                      QString command=RunProcess(cstring);

                      if (!command.isEmpty())
                           {
                             logfile(cstring);
                             logfile(command);
                             logfile("file copy error, sdcard to system/etc");
                             QMessageBox::critical(this,"","Error: cp /sdcard/02sshd /system/etc/init.d failed");
                              mount_system("ro");
                              return;
                            }


                      logfile(cstring);
                      logfile(command);

                      cstring = adb + " shell su -c chmod 0755 " + " /system/etc/init.d/02sshd";
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

                      cstring = adb + " shell rm " + " /sdcard/02sshd";
                     command=RunProcess(cstring);

                      if (!command.isEmpty())
                           {
                          logfile(cstring);
                          logfile(command);
                          logfile("error: rm /sdcard/02sshd");
                            }


                      cstring = adb + " shell ls /system/etc/init.d/02sshd";
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
                             }

                     mount_system("ro");
                     QFile::remove(filename);

            } // ssh at boot



    }  //  end superuser block



  }  //end accepted



} // end preferences



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

        QString cstring = adb + " shell screencap -p /sdcard/"+dtstr+".png";


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

        QString cstring = adb + " pull /sdcard/"+dtstr+".png " +pulldir;

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

        cstring = adb + " shell rm /sdcard/"+dtstr+".png " ;
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
         reply = QMessageBox::question(this, "Busybox", "Install System Tools?",
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






      QString cstring = adb + " push "+busybox1+ " /sdcard/";

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

              cstring = adb + " push "+busybox2+ " /sdcard/";
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


               cstring = adb + " push "+busybox3+ " /sdcard/";
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


                cstring = adb + " push "+busybox4+ " /sdcard/";
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

                 cstring = adb + " push "+busybox5+ " /sdcard/";

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


     cstring = adb + " shell su -c cp " + " /sdcard/busybox /system/xbin";
     command=RunProcess(cstring);
     
     logfile(cstring);
     logfile(command);

     cstring = adb + " shell su -c chmod 0755 /system/xbin/busybox";
     command=RunProcess(cstring);

     logfile(cstring);
     logfile(command);



     cstring = adb + " shell su -c /system/xbin/busybox tar xf /sdcard/xbin.tar -C /system";
     command=RunProcess(cstring);

     logfile(cstring);
     logfile(command);

     cstring = adb + " shell su -c chmod 0755 /system/xbin/*.sh";
     command=RunProcess(cstring);

     logfile(cstring);
     logfile(command);

     cstring = adb + " shell su -c cp /system/xbin/mntdrives1.sh /system/xbin/mntdrives.sh";
     command=RunProcess(cstring);

     logfile(cstring);
     logfile(command);

     cstring = adb + " shell su -c chmod 0755 /system/xbin/01mntdrives";
     command=RunProcess(cstring);
     logfile(cstring);
     logfile(command);

     cstring = adb + " shell su -c chmod 0755 /system/xbin/02sshd";
     command=RunProcess(cstring);
     logfile(cstring);
     logfile(command);

     cstring = adb + " shell su -c chmod 0755 /system/xbin/mount.exfat-fuse";
     command=RunProcess(cstring);
     logfile(cstring);
     logfile(command);

     cstring = adb + " shell su -c chmod 0755 /system/xbin/ntfs-3g";
     command=RunProcess(cstring);
     logfile(cstring);
     logfile(command);


     cstring = adb + " shell su -c /system/xbin/binstall.sh";
     command=RunProcess(cstring);

     logfile(cstring);
     logfile(command);
     
      cstring = adb + " shell ls /system/xbin/which";
      command=RunProcess(cstring);
      
      logfile(cstring);
      logfile(command);

        if (command.contains("No such file or directory"))
          { logfile("busybox install failed!");
            QMessageBox::critical( this,"","System Tools not installed!");
        }

        else
        {

            cstring = adb + " shell su -c rm /sdcard/xbin.tar";
            command=RunProcess(cstring);

            logfile(cstring);
            logfile(command);

            cstring = adb + " shell su -c tar xf /sdcard/samba.tar -C /data/data";
            command=RunProcess(cstring);

            logfile(cstring);
            logfile(command);

            cstring = adb + " shell su -c ln -s /data/data/com.funkyfresh.samba/files/samba-rc /system/xbin/samba";
            command=RunProcess(cstring);

            logfile(cstring);
            logfile(command);

         cstring = adb + " shell su -c rm /sdcard/samba.tar";
         command=RunProcess(cstring);

         logfile(cstring);
         logfile(command);


         cstring = adb + " shell su -c mkdir /system/etc/init.d";
         command=RunProcess(cstring);

         logfile(cstring);
         logfile(command);

         cstring = adb + " shell su -c chmod 777 /system/etc/init.d";
         command=RunProcess(cstring);

         logfile(cstring);
         logfile(command);



         cstring = adb + " shell su -c cp /sdcard/install-recovery*  /system/etc/";
         command=RunProcess(cstring);

         logfile(cstring);
         logfile(command);

         cstring = adb + " shell su -c chmod 0755 /system/etc/install-recovery*";
         command=RunProcess(cstring);

         logfile(cstring);
         logfile(command);


         cstring = adb + " shell su -c rm /sdcard/install-recovery*";
         command=RunProcess(cstring);

         logfile(cstring);
         logfile(command);


         cstring = adb + " shell su -c cp /system/etc/init.bueller.sh  /system/etc/init.bueller.sh.old";
         command=RunProcess(cstring);

         logfile(cstring);
         logfile(command);


         cstring = adb + " shell su -c cp /system/xbin/init.bueller.sh  /system/etc/";
         command=RunProcess(cstring);

         logfile(cstring);
         logfile(command);

         logfile(cstring);
         logfile(command);
         QMessageBox::information( this,"","System Tools installed!");
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


   cstring = adb + " shell ls /system/xbin/busybox";
   command=RunProcess(cstring);

   logfile(cstring);
   logfile(command);

     if (command.contains("No such file or directory"))
        {
         QMessageBox::critical( this,"","System Tools not installed!");
         return;
        }

   QMessageBox::StandardButton reply;
         reply = QMessageBox::question(this, "System Tools", "Uninstall System Tools?",
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

         cstring = adb + " shell su -c ls /storage/usb/drive*/";
         command=RunProcess(cstring);

       if (!command.contains("No such file or directory"))

        {

             logfile(cstring);
             logfile(command);

             mount_root("rw");



             cstring = adb + " shell su -c /data/data/com.funkyfresh.samba/files/samba-rc stop";
             command=RunProcess(cstring);;

             logfile(cstring);
             logfile(command);

             cstring = adb + " shell su -c " + rmsam;
             command=RunProcess(cstring);;

             logfile(cstring);
             logfile(command);

         cstring = adb + " shell su -c " + rmsh;
         command=RunProcess(cstring);;

         logfile(cstring);
         logfile(command);



          cstring = adb + " shell su -c " + umntstring;
          command=RunProcess(cstring);;

          logfile(cstring);
          logfile(command);

          cstring = adb + " shell su -c " + rmsd;
          command=RunProcess(cstring);

          logfile(cstring);
          logfile(command);

          mount_root("ro");

      }



        mount_system("rw");


        cstring = adb + " shell ls /system/xbin/sshstop";
        command=RunProcess(cstring);

        logfile(cstring);
        logfile(command);


         cstring = adb + " shell su -c /system/xbin/buninstall.sh";
         command=RunProcess(cstring);

         logfile(cstring);
         logfile(command);

         cstring = adb + " shell ls /system/xbin/busybox";
         command=RunProcess(cstring);
         
         logfile(cstring);
         logfile(command);

           if (command.contains("No such file or directory"))
            {
               logfile("busybox uninstalled");
               QMessageBox::information( this,"","System Tools uninstalled"); }
           else
           { logfile("busybox uninstall failed");
               QMessageBox::critical( this,"","System Tools not uninstalled!");}






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



     QString pullfile;
     QString cstring;
     QString command;
     QString xpath = "";
     QString cname = ui->comboBox->currentText();


     QString hidden;

     if (xbmcpackage.isEmpty())
        xbmcpackage="org.xbmc.kodi";

     cstring = adb + " shell ls /sdcard/Android/data/"+xbmcpackage+"/files/.xbmc";
     command=RunProcess(cstring);
    if (command.contains("No such file or directory"))
     {
        hidden=".kodi";
      }

     cstring = adb + " shell ls /sdcard/Android/data/"+xbmcpackage+"/files/.kodi";
     command=RunProcess(cstring);
     if (command.contains("No such file or directory"))
      {
         hidden=".xbmc";
       }




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





  //cstring = adb + " shell su -c find " +xpath+ " -type f ";
  //QString command=RunProcess(cstring);

  cstring = adb + " shell ls " +xpath;
  command=RunProcess(cstring);



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



            // cstring = adb + " shell rm "+pullfile;
           //  command=RunProcess(cstring);
             
             cstring = adb + " shell rm "+xpath+pullfile;
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


        cstring = adb + " shell su -c chmod 777 /cache";
        command=RunProcess(cstring);

        cstring = adb + " shell su -c chmod 777 /cache/recovery";
        command=RunProcess(cstring);


        if (command.contains("No such file or directory"))
         {

            logfile(cstring);
            logfile(command);
            logfile("/cache/recovery/ not found");

            cstring = adb + " shell su -c mkdir -p /cache/recovery/";
            command=RunProcess(cstring);

            logfile(cstring);
            logfile(command);

            if (!command.isEmpty())
             {

                QMessageBox::critical(this,"","Firmware installation aborted. See log.");
                logfile("Firmware installation aborted.");
                return;
             }



            cstring = adb + " shell su -c chmod 777 /cache/recovery";
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



    cstring = adb + " push "+commstr+ " /sdcard/";
    command=RunProcess(cstring);
    
    if (!command.contains("bytes"))
     { QMessageBox::information( this,"","Error: "+command);

        logfile(cstring);
        logfile(command);
        logfile("error pushing command file. cancelling firmware installation.");

        return; }

    cstring = adb + " shell su -c cp " + " /sdcard/command /cache/recovery/";
    command=RunProcess(cstring);

    if (!command.isEmpty())
     { QMessageBox::information( this,"","Error: "+command);

        logfile(cstring);
        logfile(command);
        logfile("error cp command file. cancelling firmware installation.");

        return; }

    logfile(cstring);
    logfile(command);

    cstring = adb + " shell su -c chmod 777 /cache/recovery/command";
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
          cstring = adb + " shell ls "+fileName;
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
        cstring = adb + " push "  + '"' +   fileName + '"'   + " /sdcard/update.zip";
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
    cstring = adb + " shell su -c cp " + " /sdcard/update.zip /cache/";
    else
    cstring = adb + " shell su -c cp " + fileName + " /cache/update.zip";

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
    cstring = adb + " shell rm /sdcard/update.zip";
    command=RunProcess(cstring);
    logfile(cstring);
    logfile(command);

   }

   logfile(cstring);
   logfile(command);



    cstring = adb + " shell rm /sdcard/command";
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

          cstring = adb + " shell su -c rm /cache/update.zip";
          command=RunProcess(cstring);

          logfile(cstring);
          logfile(command);

          cstring = adb + " shell su -c rm /cache/recovery/command";
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

   cstring = adb + " shell ls /system/xbin/mount";
   QString command=RunProcess(cstring);


     if (command.contains("No such file or directory"))
      { QMessageBox::critical( this,"","System Tools required for USB drive. Install them from the menu.");

         logfile(cstring);
         logfile(command);
         logfile("Busybox required for USB drive. Install it from the menu.");
         return;
     }



       cstring = adb + " shell su -c /system/xbin/mntdrives.sh";
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


           cstring = adb + " shell su -c /data/data/com.funkyfresh.samba/files/samba-rc start";
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




   QString cstring = adb + " shell ls /system/xbin/umount";
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
       cstring = adb + " shell su -c /data/data/com.funkyfresh.samba/files/samba-rc stop";
       command=RunProcess(cstring);

       logfile(cstring);
       logfile(command);

       cstring = adb + " shell su -c " + umntstring;
       command=RunProcess(cstring);

        ui->progressBar->setHidden(true);

       if (command.contains("failed"))
         { logfile(cstring);
           logfile(command);
           logfile("error unmounting usb drive");
           QMessageBox::critical( this,"","USB drive not found!");}
       else
        {           
           cstring = adb + " shell su -c " + rmsd;
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
     QString command;
     QString hidden;

     if (xbmcpackage.isEmpty())
        xbmcpackage="org.xbmc.kodi";

     if (xbmcpackage.isEmpty())
        xbmcpackage="org.xbmc.kodi";

       cstring = adb + " shell ls /sdcard/Android/data/"+xbmcpackage+"/files/.xbmc";
       command=RunProcess(cstring);
      if (command.contains("No such file or directory"))
       {
          hidden=".kodi";
        }

       cstring = adb + " shell ls /sdcard/Android/data/"+xbmcpackage+"/files/.kodi";
       command=RunProcess(cstring);
       if (command.contains("No such file or directory"))
        {
           hidden=".xbmc";
         }



       // QMessageBox::critical(this,"",xbmcpackage);

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
     command=RunProcess(cstring);


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


             cstring = adb + " pull "+xpath+fileName+" "+pulldir;
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


      QString cstring = adb + " shell ls "+xpath;

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


           cstring = adb + " push "+'"'+fileName+'"'+ " "+xpath+"/splash.png";

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

           cstring = adb + " install -r "+llama;
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

         cstring = adb + " shell rm -r /sdcard/Llama";
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


   cstring = adb + " shell su -c cat /system/build.prop | grep 51.1.1.0";
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
        cstring = adb + " push "  + '"' +   fileName + '"'   + " /sdcard/recovery.img";
        command=RunProcess(cstring);

        logfile(cstring);
        logfile(command);

        if (!command.contains("bytes"))
        {
          logfile("pushing "+fileName+" to /sdcard/ failed. Cancelling firmware installation");
           return;
        }



    cstring=adb + " shell su -c  dd if=/sdcard/recovery.img of=/dev/block/platform/msm_sdcc.1/by-name/recovery";
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


     cstring = adb + " shell rm /sdcard/recovery.img";
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
QString makepst = "/system/xbin/mntdrives1.sh samba";

cstring = adb + " shell su -c ls /system/xbin/mount";
QString command=RunProcess(cstring);

if (command.contains("No such file or directory"))
{

    logfile(cstring);
    logfile(command);
    logfile("busybox required, not found");

    QMessageBox::critical(this,"","System Tools required for USB drive. Install them from the menu.");
    ui->usbBox->setChecked(false);

    return;
}


    if (checked)
    {


        mount_system("rw");


        cstring = adb + " shell su -c cp " + " /system/xbin/01mntdrives /system/etc/init.d";
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

        cstring = adb + " shell su -c chmod 0755 " + " /system/etc/init.d/01mntdrives";
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



        cstring = adb + " shell ls /system/etc/init.d/01mntdrives";
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

        cstring = adb + " shell su -c rm /system/etc/init.d/01mountusb";
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


    QString cstring;
    QString command;





   bool isroot;
   cstring = adb + " shell ls /system/xbin/su";
   command=RunProcess(cstring);

   if (command.contains("No such file or directory"))
      isroot = false;
       else
       isroot = true;


   if (!isroot)
      { QMessageBox::critical(
            this,
            "",
            "Root required!");
         return;
   }





   cstring = adb + " shell ls /system/xbin/busybox";
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




   cstring = adb + " push "+adbdir+ "/ssh.tar /sdcard/";
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

   cstring = adb + " shell su -c tar xf /sdcard/ssh.tar -C /data";
   command=RunProcess(cstring);

   logfile(cstring);
   logfile(command);

cstring = adb + " shell su -c rm /sdcard/ssh.tar";
command=RunProcess(cstring);

logfile(cstring);
logfile(command);


cstring = adb + " shell su -c /data/jocala/ssh/setupssh";
command=RunProcess(cstring);

logfile(cstring);
logfile(command);


cstring = adb + " shell sshstatus";
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
     cstring = adb + " shell stopssh";
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


   cstring = adb + " shell /system/xbin/sshstatus";
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

       cstring = adb + " shell su -c /data/jocala/sshstop";
       command=RunProcess(cstring);
       logfile("ssh check");
       logfile(cstring);
       logfile(command);


      }


       logfile("uninstall ssh");

       cstring = adb + " shell su -c /data/jocala/rmssh";
       command=RunProcess(cstring);

       logfile("rm /system/xbin/ssh");
       logfile(cstring);
       logfile(command);

       cstring = adb + " shell su -c rm -r /data/jocala/";
       command=RunProcess(cstring);

       logfile("rm /system/xbin/ssh");
       logfile(cstring);
       logfile(command);


       cstring = adb + " shell su -c rm -r /system/etc/init.d/02sshd";
       command=RunProcess(cstring);

       logfile("rm /system/etc/init.d/02sshd");
       logfile(cstring);
       logfile(command);


       cstring = adb + " shell sshstatus";
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

    cstring = adb + " shell sshstatus";
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

    cstring = adb + " shell sshstatus";
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


   QString cstring = adb + " shell ls /system/xbin/aftv-unlock unlock";
   QString command=RunProcess(cstring);

    if (command.contains("No such file or directory"))
    {
     QMessageBox::information(this,"","aftv-unlock not found!");
     return;
     }


        cstring = adb + " shell su -c /system/xbin/aftv-unlock unlock";
        command=RunProcess(cstring);

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

   QString cstring = adb + " shell ls /system/xbin/aftv-unlock unlock";
   QString command=RunProcess(cstring);

    if (command.contains("No such file or directory"))
    {
     QMessageBox::information(this,"","aftv-unlock not found!");
     return;
     }


     cstring = adb + " shell su -c /system/xbin/aftv-unlock lock";
     command=RunProcess(cstring);

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

    if (!is_su())
       { QMessageBox::critical(
             this,
             "",
             "Root required!");
          return;
    }

    QMessageBox::information(this,"","Not implemented");
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


          QString mount = QInputDialog::getText(this, "Mount point",
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



///////////////////////////////////////////////////
void MainWindow::on_actionConsole_triggered()
{


    // set PATH=%PATH%;C:\xampp\php

    logfile("detaching console process");

     QString cstring = "";


     if (os == 1)

        {

        QString commstr = adbdir+"cpath.bat";
        QFile file(commstr);

            if(!file.open(QFile::WriteOnly |
                          QFile::Text))
            {
                logfile("error creating cpath.bat!");
                QMessageBox::critical(this,"","Error creating bat file!");
                return;
            }



            QTextStream out(&file);
            out  << "set PATH=%PATH%;"+QDir::currentPath()+";"<< endl;



            file.flush();
            file.close();

            cstring = "cmd /k " +QDir::currentPath()+"/cpath.bat";
            QProcess::startDetached(cstring);



     }

     QString pathdir = QCoreApplication::applicationDirPath() +"/adbfiles";

       if (os == 2 || os == 0)
         {

           QString commstr = adbdir+"cpath";
           QFile file(commstr);

               if(!file.open(QFile::WriteOnly |
                             QFile::Text))
               {
                   logfile("error creating cpath!");
                   QMessageBox::critical(this,"","Error creating sh file!");
                   return;
               }



               QTextStream out(&file);
                out  << "#!/bin/sh" << endl;
                out  << "export PATH="+pathdir+":$PATH" << endl;


               file.flush();
               file.close();

               cstring = "chmod 0755 " + commstr ;
               QString command=RunProcess(cstring);


             // QString cstring = "open -a Terminal.app "+adbdir+"cpath.sh";

              QString cstring = "open -a Terminal.app "+adbdir;

         QProcess::startDetached(cstring);



       }

       if (os == 0)
        {
           // cstring = "x-terminal-emulator --working-directory="+adbdir+"/adbfiles";
            cstring = "gnome-terminal --working-directory="+adbdir;
           QProcess::startDetached(cstring);

         }



}

///////////////////////////////////////////////////
void MainWindow::on_actionMount_CIFS_triggered()

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



   QString nfsString ="/system/xbin/busybox mount -o nolock,ro,hard,intr,vers=3 -t nfs ";
   QString cifsString = "/system/xbin/busybox mount -t cifs //";

    QString cstring;
    QString command;

    db.open();
    readShares();
    db.close();

    logfile("opening shares dialog");

    cifsDialog dialog;

    dialog.setipaddress1(ipaddress1);
    dialog.setshare1(share1);
    dialog.setmount1(mount1);
    if (nfs1=="1")
     dialog.setnfs1(true);

    dialog.setipaddress2(ipaddress2);
    dialog.setshare2(share2);
    dialog.setmount2(mount2);
    if (nfs2=="1")
     dialog.setnfs2(true);

    dialog.setipaddress3(ipaddress3);
    dialog.setshare3(share3);
    dialog.setmount3(mount3);
    if (nfs3=="1")
     dialog.setnfs3(true);

    dialog.setipaddress4(ipaddress4);
    dialog.setshare4(share4);
    dialog.setmount4(mount4);
    if (nfs4 =="1")
     dialog.setnfs4(true);





    dialog.setModal(true);


    if(dialog.exec() == QDialog::Accepted)
    {


        int x = dialog.returnval();

        ipaddress1=dialog.ipaddress1();
        share1=dialog.share1();
        mount1=dialog.mount1();
        select1=dialog.select1();

        if (dialog.nfs1())
            nfs1="1";
         else
           nfs1="0";


        ipaddress2=dialog.ipaddress2();
        share2=dialog.share2();
        mount2=dialog.mount2();
        select2=dialog.select2();

        if (dialog.nfs2())
            nfs2="1";
         else
           nfs2="0";

        ipaddress3=dialog.ipaddress3();
        share3=dialog.share3();
        mount3=dialog.mount3();
        select3=dialog.select3();

        if (dialog.nfs3())
            nfs3="1";
         else
           nfs3="0";

        ipaddress4=dialog.ipaddress4();
        share4=dialog.share4();
        mount4=dialog.mount4();
        select4=dialog.select4();

        if (dialog.nfs4())
            nfs4="1";
         else
           nfs4="0";



        switch( x )
        {
            case 1 :
                // mount selected shares

            if (select1)
                {


                 if (nfs1=="1")
                 {
                     logfile("mounting nfs share");
                     cstring = adb + " shell su -c "+nfsString+ipaddress1+":"+share1+" "+mount1;
                     command=RunProcess(cstring);

                      logfile(cstring);
                      logfile(command);
                  }

                 else

                 {
                     logfile("mounting cifs share");
                     cstring = adb + " shell su -c "+cifsString+ipaddress1+share1+" "+mount1+ " -o username=guest";
                     command=RunProcess(cstring);

                      logfile(cstring);
                      logfile(command);
                 }



                }   // end select

            if (select2)
                {


                 if (nfs2=="1")
                 {
                     logfile("mounting nfs share");
                     cstring = adb + " shell su -c "+nfsString+ipaddress2+":"+share2+" "+mount2;
                     command=RunProcess(cstring);

                      logfile(cstring);
                      logfile(command);
                  }

                 else

                 {
                     logfile("mounting cifs share");
                     cstring = adb + " shell su -c "+cifsString+ipaddress2+share2+" "+mount2+ " -o username=guest";
                     command=RunProcess(cstring);

                      logfile(cstring);
                      logfile(command);
                 }

                }   // end select


            if (select3)
                {


                 if (nfs3=="1")
                 {
                     logfile("mounting nfs share");
                     cstring = adb + " shell su -c "+nfsString+ipaddress3+":"+share3+" "+mount3;
                     command=RunProcess(cstring);

                      logfile(cstring);
                      logfile(command);
                  }

                 else

                 {
                     logfile("mounting cifs share");
                     cstring = adb + " shell su -c "+cifsString+ipaddress3+share3+" "+mount3+ " -o username=guest";
                     command=RunProcess(cstring);

                      logfile(cstring);
                      logfile(command);
                 }

                }   // end select

            if (select4)
                {


                 if (nfs4=="1")
                 {
                     logfile("mounting nfs share");
                     cstring = adb + " shell su -c "+nfsString+ipaddress4+":"+share4+" "+mount4;
                     command=RunProcess(cstring);

                      logfile(cstring);
                      logfile(command);
                  }

                 else

                 {
                     logfile("mounting cifs share");
                     cstring = adb + " shell su -c "+cifsString+ipaddress4+share4+" "+mount4+ " -o username=guest";
                     command=RunProcess(cstring);

                      logfile(cstring);
                      logfile(command);
                 }

                }   // end select 4

            break;

       case 2 :
                // unmount

            if (select1)
                {

                     cstring = adb + " shell su -c busybox umount "+mount1;
                     command=RunProcess(cstring);

                      logfile(cstring);
                      logfile(command);
                  }

            if (select2)
                {

                     cstring = adb + " shell su -c busybox umount "+mount2;
                     command=RunProcess(cstring);

                      logfile(cstring);
                      logfile(command);
                  }

            if (select3)
                {

                     cstring = adb + " shell su -c busybox umount "+mount3;
                     command=RunProcess(cstring);

                      logfile(cstring);
                      logfile(command);
                  }


            if (select4)
                {

                     cstring = adb + " shell su -c busybox umount "+mount4;
                     command=RunProcess(cstring);

                      logfile(cstring);
                      logfile(command);
                  }




            break;

            case 3 :
                // save

                db.open();
                updateShares();
                db.close();

                break;

             case 4 :
                // script

            QString commstr = adbdir+"03mntshares";
            QFile file(commstr);

                if(!file.open(QFile::WriteOnly |
                              QFile::Text))
                {
                    logfile("error creating file!");
                    QMessageBox::critical(this,"","Error creating sh file!");
                    return;
                }



                QTextStream out(&file);
                 out  << "#!/system/bin/sh" << endl;

                 if (select1)
                     {


                      if (nfs1=="1")
                      {

                          cstring = nfsString+ipaddress1+":"+share1+" "+mount1;
                          out  << cstring << endl;
                          logfile(cstring);
                       }

                      else

                      {

                          cstring = cifsString+ipaddress1+share1+" "+mount1+ " -o username=guest";
                          out  << cstring << endl;
                          logfile(cstring);
                      }



                     }   // end select


                 if (select2)
                     {


                      if (nfs2=="1")
                      {

                          cstring = nfsString+ipaddress2+":"+share2+" "+mount2;
                          out  << cstring << endl;
                          logfile(cstring);
                       }

                      else

                      {

                          cstring = cifsString+ipaddress2+share2+" "+mount2+ " -o username=guest";
                          out  << cstring << endl;
                          logfile(cstring);
                      }



                     }   // end select


                 if (select3)
                     {


                      if (nfs3=="1")
                      {

                          cstring = nfsString+ipaddress3+":"+share3+" "+mount3;
                          out  << cstring << endl;
                          logfile(cstring);
                       }

                      else

                      {

                          cstring = cifsString+ipaddress3+share3+" "+mount3+ " -o username=guest";
                          out  << cstring << endl;
                          logfile(cstring);
                      }



                     }   // end select

                 if (select4)
                     {


                      if (nfs4=="1")
                      {

                          cstring = nfsString+ipaddress4+":"+share4+" "+mount4;
                          out  << cstring << endl;
                          logfile(cstring);
                       }

                      else

                      {

                          cstring = cifsString+ipaddress4+share4+" "+mount4+ " -o username=guest";
                          out  << cstring << endl;
                          logfile(cstring);
                      }



                     }   // end select




                 file.flush();
                 file.close();

                 mount_system("rw");

                      cstring = adb + " push "+adbdir+"03mntshares /sdcard/";
                      command=RunProcess(cstring);

                      logfile(cstring);
                      logfile(command);

                         if (!command.contains("bytes"))
                           {
                             logfile("03mntshares install failed ");

                             }






                     cstring = adb + " shell su -c cp " + " /sdcard/03mntshares /system/etc/init.d/";
                     command=RunProcess(cstring);

                     logfile(cstring);
                     logfile(command);


                         cstring = adb + " shell su -c chmod 0755 /system/etc/init.d/03mntshares";
                         command=RunProcess(cstring);

                         logfile(cstring);
                         logfile(command);

                         cstring = adb + " shell rm /sdcard/03mntshares";
                         command=RunProcess(cstring);

                         logfile(cstring);
                         logfile(command);

                         mount_system("ro");

            break;

        }



  }

  // QMessageBox::information(this,"","USB Mode On");
}



void MainWindow::on_usbmode_toggled(bool checked)
{

    QString msg;

    if (ui->usbmode->isChecked() )
      {
        daddr = ui->deviceBox->currentText();
        QString cstring = adb + " disconnect "+daddr+port ;
        QString command=RunProcess(cstring);

        isConnected=true;
        ui->device_connected->setText(devstr3);
        ui->update_status->setText(amazon_update2);
      }
    else
       { isConnected=false;
         ui->device_connected->setText(devstr2);
         ui->update_status->setText(amazon_update2);
       }




}

void MainWindow::on_actionVideo_Help_triggered()
{
    logfile("opening video help");
    videoDialog videodialog;
    videodialog.setModal(true);
    videodialog.exec();
}

void MainWindow::on_actionDownload_Kodi_triggered()
{
    logfile("opening video help");
    kodiDialog kodidialog;
    kodidialog.setModal(true);
    kodidialog.exec();
}

void MainWindow::on_pushButton_clicked()
{
    on_actionPreferences_triggered();
}
