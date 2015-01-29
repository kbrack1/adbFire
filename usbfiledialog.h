#ifndef USBFILEDIALOG_H
#define USBFILEDIALOG_H

#include <QDialog>

namespace Ui {
class usbfileDialog;
}

class usbfileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit usbfileDialog(QWidget *parent = 0);
    ~usbfileDialog();

public:
void setData(const QString &labelText);

public:
   QString binfileName();

public:
   void setTitle(QString text);

public:
    QString buffer;

public:
    Ui::usbfileDialog *ui;


    QString tmpdir1;
    QString tmpstr1;
    QString commstr1;
    QString adb21;
    QString fline1;
    QString cstring;
    QString cstr1;



private slots:

};

#endif // USBFILEDIALOG_H
