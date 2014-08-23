#ifndef LLAMADIALOG_H
#define LLAMADIALOG_H

#include <QDialog>

namespace Ui {
class llamaDialog;
}

class llamaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit llamaDialog(QWidget *parent = 0);
    ~llamaDialog();

public:
   bool llamacheck();

public:
   bool llamaradio1();

public:
   bool llamaradio2();

public:
   bool llamaradio3();

public:
   bool ctvradio1();

public:
   bool ctvradio2();

public:
   bool ctvradio3();


private:
    Ui::llamaDialog *ui;
};

#endif // LLAMADIALOG_H