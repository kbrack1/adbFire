#ifndef BOOTMENUDIALOG_H
#define BOOTMENUDIALOG_H

#include <QDialog>

namespace Ui {
class bootmenuDialog;
}

class bootmenuDialog : public QDialog
{
    Q_OBJECT

public:
    explicit bootmenuDialog(QWidget *parent = 0);
    ~bootmenuDialog();

    bool bagreeBox();

private:
    Ui::bootmenuDialog *ui;
};

#endif // BOOTMENUDIALOG_H
