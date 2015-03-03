#ifndef UNLOCKDIALOG_H
#define UNLOCKDIALOG_H

#include <QDialog>

namespace Ui {
class unlockDialog;
}

class unlockDialog : public QDialog
{
    Q_OBJECT

public:
    explicit unlockDialog(QWidget *parent = 0);
    ~unlockDialog();

  bool agreeBox();

private:
    Ui::unlockDialog *ui;
};

#endif // UNLOCKDIALOG_H
