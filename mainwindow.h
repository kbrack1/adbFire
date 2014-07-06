#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_actionAbout_triggered();

    void on_connButton_clicked();

    void on_astart_Button_clicked();

    void on_akill_Button_clicked();

    void TimerEvent();

    void on_sideload_Button_clicked();

    void on_disButton_clicked();

    void on_actionQuit_triggered();

    void on_uninstall_Button_clicked();

    void on_actionHelp_triggered();

    void on_backupButton_clicked();

    void on_rootButton_clicked();

    void on_fpushButton_clicked();

    void on_fpullButton_clicked();

    void on_restoreButton_clicked();

    void on_pushRemote_clicked();

    void on_consoleButton_clicked();

    void on_actionPreferences_triggered();

    void on_actionReboot_triggered();

    void on_actionRecovery_triggered();

    void on_screenshotButton_clicked();

    void on_actionUpdates_on_triggered();

    void on_actionUpdates_off_triggered();

    void on_actionMount_system_rw_triggered();

    void on_actionMount_system_ro_triggered();

    void on_actionInstall_busybox_triggered();


    void on_actionUninstall_Busybox_triggered();

    void on_fdellButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
