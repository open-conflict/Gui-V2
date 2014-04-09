#ifndef CONFLICT_CORE_H
#define CONFLICT_CORE_H

#include "stdint.h"
#include "qextserialport.h"
#include "qextserialenumerator.h"
//#include <QtCore/QCoreApplication>
#include "ui_mainwindow.h"
#include "typedef.h"
#include "mainwindow.h"

class conflict_core{
private:
    void sendString(QString str);

public:
    conflict_core();
   // void loadIni();

    QextSerialPort *connectSerial(int port);
    void closeSerial();
    uint8_t isSerialOpen();
    void setPortSettings(PortSettings settings);
    PortSettings getPortSettings();
    void parseData();
    void getData();

    void openAida();
    void updateAida();

    // UART Senden/Empfangen
    void sendInit();
    void sendReset();
    void sendExit();
    void sendPcData();
    void sendDfmData();
    void sendMinMax(uint8_t kanal);
    void sendKanalData(uint8_t kanal);
    void sendNamen();
    void sendErsatzTemps();
    void sendAnzeigeData();
    void sendLedData();
    void telegramInterpreter();

    // *GETTER*
    luefter_s getLuefter(uint8_t nr);
    led_s getLed(uint8_t nr);
    ledModus_s getLedModus();
    anzeige_s getAnzeige();
    temperatur_s getTemperature(uint8_t nr);
    dfm_s getDfm();
    analog_s getAnalog();
    alarm_s getAlarm();
    computer_s getComputer();
    uhrzeit_s getUhrzeit();
    oneWireSensor_s getOneWireSensor(uint8_t nr);
    luefter_temp_s getLuefterTemp(uint8_t luefter, uint8_t temperatur);
    firmware_s getFirmware();
    luefter_temp_s getAnalogTemp(uint8_t temperatur);
    conflict_s getConflict();


    // *SETTER*

    void setLuefter(luefter_s data, uint8_t nr);
    void setLed(led_s data, uint8_t nr);
    void setLedModus(ledModus_s modus);
    void setAnzeige(anzeige_s data);
    void setTemperature(temperatur_s data, uint8_t nr);
    void setDfm(dfm_s data);
    void setAnalog(analog_s data);
    void setAlarm(alarm_s data);
    void setComputer(computer_s data);
    void setUhrzeit(uhrzeit_s data);
    void setOneWireSensor(oneWireSensor_s data, uint8_t nr);
    void setUi(Ui::MainWindow *uiuiuiui);
    void setMw(MainWindow *mainwindows);
    void setAnalogTemp(luefter_temp_s data, uint8_t temperatur);
    void setFirmware(firmware_s data);
    void setLuefterTemp(luefter_temp_s data, uint8_t luefter, uint8_t temperatur);
    void setConflict(conflict_s data);
};

#endif // CONFLICT_CORE_H
