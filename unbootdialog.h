#ifndef UNBOOTDIALOG_H
#define UNBOOTDIALOG_H

#include <QDialog>

namespace Ui {
class unbootDialog;
}

class unbootDialog : public QDialog
{
    Q_OBJECT

public:
    explicit unbootDialog(QWidget *parent = 0);
    ~unbootDialog();

     bool uagreeBox();

private:
    Ui::unbootDialog *ui;
};

#endif // UNBOOTDIALOG_H
