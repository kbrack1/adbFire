#ifndef MEDIACENTERDIALOG_H
#define MEDIACENTERDIALOG_H

#include <QDialog>

namespace Ui {
class mediacenterDialog;
}

class mediacenterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit mediacenterDialog(QWidget *parent = 0);
    ~mediacenterDialog();

public:

    QString xbmcpackageName();
    QString filepath();

private slots:
    void on_kodiButton_clicked();

    void on_xbmcButton_clicked();

    void on_spmcButton_clicked();

    void on_tvmcButton_clicked();

    void on_otherButton_clicked();

private:
    Ui::mediacenterDialog *ui;
};

#endif // MEDIACENTERDIALOG_H
