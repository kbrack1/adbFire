#ifndef CREDITSDIALOG_H
#define CREDITSDIALOG_H

#include <QDialog>

namespace Ui {
class creditsDialog;
}

class creditsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit creditsDialog(QWidget *parent = 0);
    ~creditsDialog();

private:
    Ui::creditsDialog *ui;
};

#endif // CREDITSDIALOG_H
