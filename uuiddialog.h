#ifndef UUIDDIALOG_H
#define UUIDDIALOG_H

#include <QDialog>

namespace Ui {
class uuidDialog;
}

class uuidDialog : public QDialog
{
    Q_OBJECT

public:
    explicit uuidDialog(QWidget *parent = 0);
    ~uuidDialog();

public:
void setData1(const QString &labelText);

public:
   QString uuidName();

public:
   void setTitle1(QString text);

public:
    QString buffer;

public:
    Ui::uuidDialog *ui;


    QString tmpdir2;
    QString tmpstr2;
    QString commstr2;
    QString adb22;
    QString fline2;
    QString cstring;
    QString cstr2;



};

#endif // UUIDDIALOG_H
