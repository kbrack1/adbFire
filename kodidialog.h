#ifndef KODIDIALOG_H
#define KODIDIALOG_H

#include <QDialog>

namespace Ui {
class kodiDialog;
}

class kodiDialog : public QDialog
{
    Q_OBJECT

public:
    explicit kodiDialog(QWidget *parent = 0);
    ~kodiDialog();

private slots:
    void on_kodi_clicked();

private:
    Ui::kodiDialog *ui;
};

#endif // KODIDIALOG_H
