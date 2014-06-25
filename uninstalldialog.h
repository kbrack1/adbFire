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
    explicit uninstallDialog(QWidget *parent = 0);
    ~uninstallDialog();

public:
   QString packageName();


private:
    Ui::uninstallDialog *ui;
};

#endif // UNINSTALLDIALOG_H
