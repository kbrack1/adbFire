#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

namespace Ui {
class preferencesDialog;
}

class preferencesDialog : public QDialog
{
    Q_OBJECT





public:
    explicit preferencesDialog(QWidget *parent = 0 );

      void setPackagename(const QString &packagename);
      void setPulldir(const QString &pulldir);
      void setftvUpdate(const bool &updatecheck);
      void setsshBoot(const bool &sshcheck);
      void setmountBoot(const bool &mountcheck);

      void setversioncheck(const bool &versioncheck);
      void setisusb(const bool &isusb);

      void setversionLabel(const QString &versiontext);
      void setdaddr(const QString &daddr);
      void setdescription(const QString &description);
      void setfilepath(const QString &filepath);
      void setport(const QString &port);
      void setbuffermode(const int &buffermode);
      void setbuffersize(const QString &buffersize);
      void setbufferfactor(const QString &bufferfactor);
      void setrecnum(const QString &recnum);

    ~preferencesDialog();

public:
   QString xbmcpackageName();
   int buffermode();
   QString buffersize();
   QString bufferfactor();
   QString pulldir();
   QString daddr();
   QString description();
   QString filepath();
   QString port();

  bool updatecheck();
  bool sshcheck();
  bool mountcheck();
   bool versioncheck();  
   bool isusb();

int returnval1();

private slots:
   void on_pfolderButton_clicked();


   void on_getUpdate_clicked();


   void onRequestCompleted();


   void on_writexml_clicked();

   void on_resetxml_clicked();

   void on_pushButton_clicked();

   void on_preset1_clicked();

   void on_preset2_clicked();

   void on_preset3_clicked();

   void on_kodiButton_clicked();

   void on_xbmcButton_clicked();

   void on_spmcButton_clicked();

   void on_otherButton_clicked();

   void on_tvmcButton_clicked();

   void on_isusb_clicked(bool checked);

private:
    Ui::preferencesDialog *ui;



};

#endif // PREFERENCESDIALOG_H
