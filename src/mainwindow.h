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

    void updateTemperaturNamen();
    void updateTemperaturValue();
    void updateLuefter();
    void updateDFM();
    void updateAlarm();
    void updateAll();
    void initDefault();
    void initGui();
    void initAll();
  //  void update

public slots:
    void conflictUpdate();
    void conflictNeustart();
    void openSerial(int port);
    void closeSerial();
    void getData();
    void colorPicker();
    void updateConflictColor(int value);
    void updateConflictLedModus();
    void updateConflictAlarm(int value);
    void updateConflictAlarmWrap();
    void updateConflictAnzeige(int value);
    void updateConflictTemperaturAktiv(int value);
    void updateConflictTemperaturNamen(int value);
    void updateConflictAidaErsatz(int value);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
