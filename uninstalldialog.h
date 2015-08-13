#ifndef UNINSTALLDIALOG_H
#define UNINSTALLDIALOG_H

#include <QDialog>

namespace Ui {
class uninstallDialog;
}

class uninstallDialog : public QDialog
{
    Q_OBJECT


public:
    explicit uninstallDialog(const QString &port, const QString &daddr,QWidget *parent = 0);
    ~uninstallDialog();


private:
        QString m_daddr;
        QString m_port;

        //   explicit uninstallDialog(const QString &port, QString &daddr, QWidget *parent = 0);
  //  ~uninstallDialog();



public:
   QString packageName();

   // void setport(const QString &port);
   void setdescription(const QString &description);

   //   void setdaddr(const QString &daddr);

public:
   bool keepBox();



private:
    Ui::uninstallDialog *ui;
};

#endif // UNINSTALLDIALOG_H
