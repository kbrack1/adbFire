#ifndef VIDEODIALOG_H
#define VIDEODIALOG_H

#include <QDialog>

namespace Ui {
class videoDialog;
}

class videoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit videoDialog(QWidget *parent = 0);
    ~videoDialog();

private:
    Ui::videoDialog *ui;
};

#endif // VIDEODIALOG_H
