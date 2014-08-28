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
      void setversioncheck(const bool &versioncheck);
      void setversionLabel(const QString &versiontext);


    ~preferencesDialog();

public:
   QString xbmcpackageName();

public:
   QString pulldir();

public:
   bool updatecheck();

public:
   bool versioncheck();


private slots:
   void on_pfolderButton_clicked();


   void on_getUpdate_clicked();


   void onRequestCompleted();


private:
    Ui::preferencesDialog *ui;



};

#endif // PREFERENCESDIALOG_H
