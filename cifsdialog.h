#ifndef CIFSDIALOG_H
#define CIFSDIALOG_H

#include <QDialog>

namespace Ui {
class cifsDialog;
}

class cifsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit cifsDialog(QWidget *parent = 0);

public:
    void setipaddress1(const QString &ipaddress1);
    void setshare1(const QString &share1);
    void setmount1(const QString &mount1);
    void setselect1(const bool &select1);
    void setnfs1(const bool &nfs1);

    void setipaddress2(const QString &ipaddress2);
    void setshare2(const QString &share2);
    void setmount2(const QString &mount2);
    void setselect2(const bool &select2);
    void setnfs2(const bool &nfs2);

    void setipaddress3(const QString &ipaddress3);
    void setshare3(const QString &share3);
    void setmount3(const QString &mount3);
    void setselect3(const bool &select3);
    void setnfs3(const bool &nfs3);

    void setipaddress4(const QString &ipaddress4);
    void setshare4(const QString &share4);
    void setmount4(const QString &mount4);
    void setselect4(const bool &select4);
    void setnfs4(const bool &nfs4);


    ~cifsDialog();

public:
   QString ipaddress1();
   QString share1();
   QString mount1();
   bool nfs1();
   bool select1();

   QString ipaddress2();
   QString share2();
   QString mount2();
   bool nfs2();
   bool select2();

   QString ipaddress3();
   QString share3();
   QString mount3();
   bool nfs3();
   bool select3();

   QString ipaddress4();
   QString share4();
   QString mount4();
   bool nfs4();
   bool select4();

   int returnval();

private slots:
   void on_mountButton_clicked();

   void on_umountButton_clicked();

   void on_saveButton_clicked();

   void on_scriptButton_clicked();

   void on_cancelButton_clicked();

   void on_unscriptButton_clicked();

private:
    Ui::cifsDialog *ui;
};

#endif // CIFSDIALOG_H
