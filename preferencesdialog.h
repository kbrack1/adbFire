#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

namespace Ui {
class preferencesDialog;
}

class preferencesDialog : public QDialog
{
    Q_OBJECT





public:
    explicit preferencesDialog(QWidget *parent = 0 );

      void setPackagename(const QString &packagename);

    ~preferencesDialog();

public:
   QString xbmcpackageName();


private:
    Ui::preferencesDialog *ui;



};

#endif // PREFERENCESDIALOG_H
