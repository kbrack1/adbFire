#ifndef DATADIALOG_H
#define DATADIALOG_H

#include <QDialog>

namespace Ui {
class dataDialog;
}

class dataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit dataDialog(QWidget *parent = 0);
    ~dataDialog();

public:
    int returnval2();

private slots:
    void on_dataButton1_clicked();

    void on_dataButton2_clicked();

    void on_dataButton3_clicked();

    void on_dataButton4_clicked();

private:
    Ui::dataDialog *ui;
};

#endif // DATADIALOG_H
