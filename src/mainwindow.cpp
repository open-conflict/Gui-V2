#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "conflict_core.h"
#include "qextserialport.h"
#include "qextserialenumerator.h"
#include "QSignalMapper"
#include <QColor>
#include "QSettings"
#include "QProgressBar"
#include "QComboBox"
#include "Qt"
#include "QColorDialog"
#include <QtWidgets>

conflict_core *core;
PortSettings portSettings;
QList<QextPortInfo> serialPorts;
QextSerialPort *SerialPort;

// Tab Info
QLabel *infoTempName[24];
QProgressBar *infoTempValue[24];

// Tab Temperatur
QLabel *temperaturNr[24];
QCheckBox *temperaturAktiv[24];
QLineEdit *temperaturName[24];
QLabel *temperaturOneWire[8];
QComboBox *temperaturAida[8];
QSpinBox *temperaturAidaErsatz[24];
QLabel *temperaturValue[24];

QLabel *luefterName[3][24];
QLabel *analogName[24];

QSpinBox *luefterMin[3][24];
QSpinBox *luefterMax[3][24];
QSpinBox *analogMax[24];
QSpinBox *analogMin[24];

QLabel *luefterValue[3][24];
QLabel *analogValue[24];

QCheckBox *luefterTempAktiv[3][24];
QCheckBox *analogTempAktiv[3][24];


// Stylesheets
QString redBar;
QString greenBar;
QString blueBar;

MainWindow::MainWindow(QWidget *parent) :

    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

        int i;
    ui->setupUi(this);

    core = new conflict_core();
    initDefault();
    core->setUi(ui);
    core->setMw(this);

    core->openAida();
    core->updateAida();


    redBar = "QProgressBar::chunk {background: red; }QProgressBar {border: 1px solid gray;height: 10px;margin-right: 30px;text-align: right;}";
    greenBar = "QProgressBar::chunk {background: green; }QProgressBar {border: 1px solid gray;height: 10px;margin-right: 30px;text-align: right;}";
    blueBar = "QProgressBar::chunk {background: blue; }QProgressBar {border: 1px solid gray;height: 10px;margin-right: 30px;text-align: right;}";
    initGui();

    serialPorts = QextSerialEnumerator::getPorts();
    QSignalMapper* signalMapper = new QSignalMapper (this);
    for(i = 0;i < serialPorts.size();i++){
        if((QString)serialPorts.at(i).portName != (QString)""){
            QAction *menueSerialPorts = new QAction((QString)serialPorts.at(i).portName.toLocal8Bit().constData(),this);
            ui->menuVerbinden->addAction(menueSerialPorts);
            QObject::connect (menueSerialPorts, SIGNAL(triggered()), signalMapper, SLOT(map())) ;
            signalMapper -> setMapping (menueSerialPorts, i);
        }
    }
    QObject::connect (signalMapper, SIGNAL(mapped(int)), this, SLOT(openSerial(int)));

    QObject::connect (ui->actionTrennen, SIGNAL(triggered()), this, SLOT(closeSerial()));
    QObject::connect (ui->actionBeenden, SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect (ui->actionNeustart, SIGNAL(triggered()), this, SLOT(conflictNeustart()));
    QObject::connect (ui->colorPicker, SIGNAL(clicked()), this, SLOT(colorPicker()));

    QObject::connect (ui->ledColorBlue, SIGNAL(sliderMoved(int)), this, SLOT(updateConflictColor(int)));
    QObject::connect (ui->ledColorRed, SIGNAL(sliderMoved(int)), this, SLOT(updateConflictColor(int)));
    QObject::connect (ui->ledColorGreen, SIGNAL(sliderMoved(int)), this, SLOT(updateConflictColor(int)));

    QObject::connect (ui->ledModusRadioManuell, SIGNAL(clicked()), this, SLOT(updateConflictLedModus()));
    QObject::connect (ui->ledModusRadioV1, SIGNAL(clicked()), this, SLOT(updateConflictLedModus()));
    QObject::connect (ui->ledModusRadioV2, SIGNAL(clicked()), this, SLOT(updateConflictLedModus()));
    QObject::connect (ui->ledModusRadioV3, SIGNAL(clicked()), this, SLOT(updateConflictLedModus()));

    QObject::connect (ui->optionenAlarmDurchfluss, SIGNAL(clicked()), this, SLOT(updateConflictAlarmWrap()));
    QObject::connect (ui->optionenAlarmLuefter, SIGNAL(clicked()), this, SLOT(updateConflictAlarmWrap()));
    QObject::connect (ui->optionenAlarmTemperatur, SIGNAL(clicked()), this, SLOT(updateConflictAlarmWrap()));
    QObject::connect (ui->optionenAlarmMinDurchfluss, SIGNAL(valueChanged(int)), this, SLOT(updateConflictAlarm(int)));
    QObject::connect (ui->optionenDfmImpulse, SIGNAL(valueChanged(int)), this, SLOT(updateConflictAlarm(int)));

    QObject::connect (ui->optionenAnzeigeBeleuchtung, SIGNAL(sliderMoved(int)), this, SLOT(updateConflictAnzeige(int)));
    QObject::connect (ui->optionenAnzeigeKontrast, SIGNAL(sliderMoved(int)), this, SLOT(updateConflictAnzeige(int)));
    QObject::connect (ui->optionenDfmShow, SIGNAL(stateChanged(int)), this, SLOT(updateConflictAnzeige(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::openSerial(int port){
    SerialPort = core->connectSerial(port);
    if(SerialPort){
        QObject::connect (SerialPort, SIGNAL(readyRead()),this,SLOT(getData()));
        ui->statusBar->showMessage("Serial Geoeffnet");
        core->sendInit();
        ui->menuVerbinden->setEnabled(false);
        ui->actionNeustart->setEnabled(true);
        ui->actionTrennen->setEnabled(true);
        ui->actionSpeichern->setEnabled(true);
    }
}

void MainWindow::getData(){
    core->getData();
}

void MainWindow::closeSerial(){
    core->sendExit();
    core->closeSerial();
    ui->statusBar->showMessage("Serial Geschlossen");
    ui->menuVerbinden->setEnabled(true);
    ui->actionNeustart->setEnabled(false);
    ui->actionTrennen->setEnabled(false);
    ui->actionSpeichern->setEnabled(false);
}

void MainWindow::conflictNeustart(){
    core->sendReset();
}

void MainWindow::initGui(){
    uint8_t i,j;

    // Temperatur Widgets in INFO Tab adden
    for(i=0;i<24;i++){
        infoTempName[i] = new QLabel();
        infoTempName[i]->setMinimumWidth(35);
        ui->infoTemperaturenBoxLayout->addWidget(infoTempName[i],(int)i,0);
        infoTempValue[i] = new QProgressBar();
        infoTempValue[i]->setFormat(QString::fromUtf8("%p c°"));
        ui->infoTemperaturenBoxLayout->addWidget(infoTempValue[i],(int)i,1);
    }

    QSignalMapper* signalMapperTemperaturAktiv = new QSignalMapper (this);
    QSignalMapper* signalMapperTemperaturNamen = new QSignalMapper (this);
    QSignalMapper* signalMapperAidaErsatz = new QSignalMapper (this);
    for(i=0;i<24;i++){
        temperaturNr[i]     = new QLabel();
        temperaturNr[i]->setText(QString::number(i));
        ui->temperaturBoxLayout->addWidget(temperaturNr[i],(int)i+1,0);

        temperaturAktiv[i]  = new QCheckBox();
        ui->temperaturBoxLayout->addWidget(temperaturAktiv[i],(int)i+1,1);
        QObject::connect (temperaturAktiv[i], SIGNAL(clicked()), signalMapperTemperaturAktiv, SLOT(map())) ;
        signalMapperTemperaturAktiv -> setMapping (temperaturAktiv[i], i);

        temperaturName[i]   = new QLineEdit();
        temperaturName[i]->setMaximumWidth(200);
        temperaturName[i]->setMinimumWidth(150);
        ui->temperaturBoxLayout->addWidget(temperaturName[i],(int)i+1,2);
        QObject::connect (temperaturName[i], SIGNAL(editingFinished()), signalMapperTemperaturNamen, SLOT(map())) ;
        signalMapperTemperaturNamen -> setMapping (temperaturName[i], i);

        temperaturValue[i]  = new QLabel();
        temperaturValue[i]->setAlignment(Qt::AlignCenter);
        ui->temperaturBoxLayout->addWidget(temperaturValue[i],(int)i+1,3);
        if(i>=8 && i <16){
            // Onewire
            temperaturOneWire[i-8]      = new QLabel();
            ui->temperaturBoxLayout->addWidget(temperaturOneWire[i-8],(int)i+1,4);
            temperaturOneWire[i-8]->setText(QString("00 00 00 00 00 00 00 00"));
        }
        if(i>=16 && i <24){
            // AIDA
            temperaturAidaErsatz[i-16]  = new QSpinBox();
            ui->temperaturBoxLayout->addWidget(temperaturAidaErsatz[i-16],(int)i+1,5);
            QObject::connect (temperaturAidaErsatz[i-16], SIGNAL(editingFinished ()), signalMapperAidaErsatz, SLOT(map())) ;
            signalMapperAidaErsatz -> setMapping (temperaturAidaErsatz[i-16], i);

            temperaturAida[i-16]        = new QComboBox();
            ui->temperaturBoxLayout->addWidget(temperaturAida[i-16],(int)i+1,4);
        }
    }
    QObject::connect (signalMapperTemperaturAktiv, SIGNAL(mapped(int)), this, SLOT(updateConflictTemperaturAktiv(int)));
    QObject::connect (signalMapperTemperaturNamen, SIGNAL(mapped(int)), this, SLOT(updateConflictTemperaturNamen(int)));
    //QObject::connect (signalMapperAidaErsatz,      SIGNAL(mapped(int)), this, SLOT(updateConflictAidaErsatz(int)));

    for(j=0;j<24;j++){
        for(i=0;i<3;i++){
            luefterName[i][j]  = new QLabel();
            luefterValue[i][j] = new QLabel();
            luefterMin[i][j] = new QSpinBox();
            luefterMax[i][j] = new QSpinBox();
            luefterName[i][j]->setText(QString::fromUtf8("t ")+QString::number(j));
            luefterValue[i][j]->setText(QString::fromUtf8("10 C°"));
            if(i==0){
                ui->kanal1TempBoxLayout->addWidget(luefterName[i][j],j+1,0);
                ui->kanal1TempBoxLayout->addWidget(luefterValue[i][j],j+1,1);
                ui->kanal1TempBoxLayout->addWidget(luefterMin[i][j],j+1,3);
                ui->kanal1TempBoxLayout->addWidget(luefterMax[i][j],j+1,4);
            }else if(i==1){
                ui->kanal2TempBoxLayout->addWidget(luefterName[i][j],j+1,0);
                ui->kanal2TempBoxLayout->addWidget(luefterValue[i][j],j+1,1);
                ui->kanal2TempBoxLayout->addWidget(luefterMin[i][j],j+1,3);
                ui->kanal2TempBoxLayout->addWidget(luefterMax[i][j],j+1,4);
            }else{
                ui->kanal3TempBoxLayout->addWidget(luefterName[i][j],j+1,0);
                ui->kanal3TempBoxLayout->addWidget(luefterValue[i][j],j+1,1);
                ui->kanal3TempBoxLayout->addWidget(luefterMin[i][j],j+1,3);
                ui->kanal3TempBoxLayout->addWidget(luefterMax[i][j],j+1,4);
            }
        }
        analogMax[j] = new QSpinBox();
        analogMin[j] = new QSpinBox();
        analogName[j]  = new QLabel();
        analogValue[j] = new QLabel();
        analogName[j]->setText(QString::fromUtf8("t ")+QString::number(j));
        analogValue[j]->setText(QString::fromUtf8("10 C°"));
        ui->analogTempBoxLayout->addWidget(analogName[j],j+1,0);
        ui->analogTempBoxLayout->addWidget(analogValue[j],j+1,1);
        ui->analogTempBoxLayout->addWidget(analogMin[j],j+1,3);
        ui->analogTempBoxLayout->addWidget(analogMax[j],j+1,4);
    }


    // Temperatur Widgets in INFO Tab adden
    updateLuefter();
    updateTemperaturValue();
    updateTemperaturNamen();
}

void MainWindow::initDefault(){
    uint8_t i;

    portSettings.BaudRate = BAUD57600;
    portSettings.DataBits = DATA_8;
    portSettings.Parity = PAR_NONE;
    portSettings.StopBits = STOP_1;
    portSettings.FlowControl = FLOW_OFF;
    portSettings.Timeout_Millisec = 0;
    core->setPortSettings(portSettings);

    luefter_s luef;
    for(i=0;i < 3;i++){
        luef.name = "L"+QString::number(i);
        luef.drehzahl = 0;
        luef.pwn = 0;
        luef.manuell = 0;
        luef.anlaufzeit = 0;
        luef.minDrehzahl = 0;
        luef.autoRegelung = 0;
        luef.off = 0;
        luef.anlaufSchwelle = 0;
        luef.alarm = 0;
        luef.minSpannung = 0;
        core->setLuefter(luef,i);
    }

    temperatur_s temp;
    for(i=0;i < 24;i++){
        temp.name = "T"+QString::number(i);
        temp.min = 0;
        temp.max = 70;
        temp.alarm = 0;
        temp.value = 10;
        core->setTemperature(temp,i);
    }

    dfm_s dfm;
    dfm.impulseProLiter = 50;
    dfm.durchfluss = 0;
    core->setDfm(dfm);

    analog_s analog;
    analog.name = "L"+QString::number(i);
    analog.drehzahl = 0;
    analog.pwn = 0;
    analog.manuell = 0;
    analog.anlaufzeit = 0;
    analog.minDrehzahl = 0;
    analog.autoRegelung = 0;
    analog.off = 0;
    analog.anlaufSchwelle = 0;
    analog.alarm = 0;
    analog.minSpannung = 0;
    core->setAnalog(analog);

    alarm_s alarm;
    alarm.dfm = 0;
    alarm.luefter = 0;
    alarm.status = 0;
    alarm.temperatur = 0;
    core->setAlarm(alarm);

    oneWireSensor_s oneWire;
    oneWire.adresse = (QString)"00000000";
    for(i=0;i < 8;i++){
                core->setOneWireSensor(oneWire,i);
    }
}

void MainWindow::initAll(){
    uint8_t i;
    ui->infoFirmware->setText(core->getFirmware().version);
    ui->optionenAlarmDurchfluss->setChecked((bool)core->getAlarm().dfm);
    ui->optionenAlarmMinDurchfluss->setValue((int)core->getAlarm().dfm);
    ui->optionenAlarmLuefter->setChecked((bool)core->getAlarm().luefter);
    ui->optionenAlarmTemperatur->setChecked((bool)core->getAlarm().temperatur);
    ui->optionenAnzeigeBeleuchtung->setValue((int)core->getAnzeige().backlight);
    ui->optionenAnzeigeKontrast->setValue((int)core->getAnzeige().kontrast);
    ui->optionenDfmShow->setChecked((bool)core->getAnzeige().dfm);
    ui->optionenDfmImpulse->setValue((int)core->getDfm().impulseProLiter);
    ui->optionenStartMinimiert->setChecked(false);

    ui->ledColorBlue->setValue((int)core->getLed(0).value);
    ui->ledColorRed->setValue((int)core->getLed(1).value);
    ui->ledColorGreen->setValue((int)core->getLed(2).value);
    ui->ledModusRadioManuell->setChecked(core->getLedModus().value == 0);
    ui->ledModusRadioV1->setChecked(core->getLedModus().value == 1);
    ui->ledModusRadioV2->setChecked(core->getLedModus().value == 2);
    ui->ledModusRadioV3->setChecked(core->getLedModus().value == 3);
    switch((int)core->getLedModus().value){
        case 0:
            ui->ledModusRadioManuell->setChecked(true);
            break;
        case 1:
            ui->ledModusRadioV1->setChecked(true);
            break;
        case 2:
            ui->ledModusRadioV2->setChecked(true);
            break;
        case 3:
            ui->ledModusRadioV3->setChecked(true);
            break;
    }

    for(i=0;i<24;i++){
         temperaturName[i]->setText(core->getTemperature(i).name);
         infoTempName[i]->setText(core->getTemperature(i).name);
         //infoTempName[i]->setVisible((bool)core->getTemperature(i).aktiv);
         //infoTempValue[i]->setVisible((bool)core->getTemperature(i).aktiv);
    }

    ui->kanal1Anlaufzeit->setValue((int)core->getLuefter(0).anlaufzeit);
    ui->kanal2Anlaufzeit->setValue((int)core->getLuefter(1).anlaufzeit);
    ui->kanal3Anlaufzeit->setValue((int)core->getLuefter(2).anlaufzeit);
    ui->kanal1Auto->setChecked(!(bool)core->getLuefter(0).manuell);
    ui->kanal2Auto->setChecked(!(bool)core->getLuefter(1).manuell);
    ui->kanal3Auto->setChecked(!(bool)core->getLuefter(2).manuell);
    ui->kanal1LuefterMin->setValue((int)core->getLuefter(0).minDrehzahl);
    ui->kanal2LuefterMin->setValue((int)core->getLuefter(1).minDrehzahl);
    ui->kanal3LuefterMin->setValue((int)core->getLuefter(2).minDrehzahl);
    ui->kanal1Manuell->setChecked((bool)core->getLuefter(0).manuell);
    ui->kanal2Manuell->setChecked((bool)core->getLuefter(1).manuell);
    ui->kanal3Manuell->setChecked((bool)core->getLuefter(2).manuell);
    ui->kanal1Wiedereinschaltschwelle->setValue((int)core->getLuefter(0).anlaufSchwelle);
    ui->kanal2Wiedereinschaltschwelle->setValue((int)core->getLuefter(1).anlaufSchwelle);
    ui->kanal3Wiedereinschaltschwelle->setValue((int)core->getLuefter(2).anlaufSchwelle);
}

void MainWindow::conflictUpdate(){

}

void MainWindow::updateAll(){
    updateLuefter();
    //updateTemperaturNamen();
    updateTemperaturValue();
    updateAlarm();
    updateDFM();
}

void MainWindow::updateLuefter(){
    luefter_s tempLuefter;
    tempLuefter = core->getLuefter(0);
    ui->infoKanal1RPM->setText(QString::number(tempLuefter.drehzahl) + " RPM");
    ui->infoKanal1Prozent->setText(QString::number(tempLuefter.pwn) + " %");
    tempLuefter = core->getLuefter(1);
    ui->infoKanal2RPM->setText(QString::number(tempLuefter.drehzahl) + " RPM");
    ui->infoKanal2Prozent->setText(QString::number(tempLuefter.pwn) + " %");
    tempLuefter = core->getLuefter(2);
    ui->infoKanal3RPM->setText(QString::number(tempLuefter.drehzahl) + " RPM");
    ui->infoKanal3Prozent->setText(QString::number(tempLuefter.pwn) + " %");
}

void MainWindow::updateTemperaturNamen(){
    uint8_t i,j;
    temperatur_s tempTemp;
    oneWireSensor_s tempOne;
    for(i=0;i<24;i++){
        tempTemp = core->getTemperature(i);
        for(j=0;j<3;j++){
            luefterName[j][i]->setText(tempTemp.name);
        }
        temperaturName[i]->setText(tempTemp.name);
        analogName[i]->setText(tempTemp.name);
        infoTempName[i]->setText(tempTemp.name);
    }
}

void MainWindow::updateTemperaturValue(){
    uint8_t i,j;
    temperatur_s tempTemp;
    oneWireSensor_s tempOne;
    for(i=0;i<24;i++){
        tempTemp = core->getTemperature(i);
        for(j=0;j<3;j++){
            luefterValue[j][i]->setText(QString::number(tempTemp.value/2));
        }
        temperaturValue[i]->setText(QString::number(tempTemp.value/2));
        analogValue[i]->setText(QString::number(tempTemp.value/2));
        infoTempValue[i]->setValue(tempTemp.value/2);
        infoTempValue[i]->setStyleSheet(blueBar);
    }
}

void MainWindow::updateAlarm(){
    // noch keine Funktion
}

void MainWindow::updateDFM(){
    dfm_s tempDfm = core->getDfm();
    ui->infoDurchfluss->setText(QString::number(tempDfm.durchfluss));
}


void MainWindow::colorPicker(){
    //QColor color = QColorDialog::getColor(QColor(ui->ledColorRed->value(),ui->ledColorGreen->value(),ui->ledColorBlue->value()), this);
    QColor color = QColorDialog::getColor(Qt::white, this);
    if (color.isValid())
    {
        ui->ledColorBlue->setValue(color.blue()*color.value()/255);
        ui->ledColorGreen->setValue(color.green()*color.value()/255);
        ui->ledColorRed->setValue(color.red()*color.value()/255);
    }
    this->updateConflictColor(0);
}
void MainWindow::updateConflictColor(int value){
    value = 0;
    led_s dummy;
    ledModus_s modus;
    modus.value = 0;
    core->setLedModus(modus);
    ui->ledModusRadioManuell->setChecked(true);
    ui->ledModusRadioV1->setChecked(false);
    ui->ledModusRadioV2->setChecked(false);
    ui->ledModusRadioV3->setChecked(false);
    dummy.value = (uint8_t)ui->ledColorRed->value();
    core->setLed(dummy,0);
    dummy.value = (uint8_t)ui->ledColorBlue->value();
    core->setLed(dummy,1);
    dummy.value = (uint8_t)ui->ledColorGreen->value();
    core->setLed(dummy,2);
    core->sendLedData();
}

void MainWindow::updateConflictLedModus(){
    ledModus_s modus;
    if(ui->ledModusRadioManuell->isChecked()){
        modus.value = 0;
    }else if(ui->ledModusRadioV1->isChecked()){
        modus.value = 1;
    }else if(ui->ledModusRadioV2->isChecked()){
        modus.value = 2;
    }else{
        modus.value = 3;
    }
    core->setLedModus(modus);
    core->sendLedData();
}

void MainWindow::updateConflictAlarmWrap(){
    updateConflictAlarm(0);
}

void MainWindow::updateConflictAlarm(int value){
    alarm_s dummy = core->getAlarm();
    dfm_s dummy2 = core->getDfm();
    if(core->getConflict().init){
        if(ui->optionenAlarmDurchfluss->isChecked()){
            dummy.dfm = (uint16_t)ui->optionenAlarmMinDurchfluss->value();
        }else{
            dummy.dfm = (uint16_t)ui->optionenAlarmDurchfluss->isChecked();
        }
        dummy.luefter = (uint8_t)ui->optionenAlarmLuefter->isChecked();
        dummy.temperatur = (uint8_t)ui->optionenAlarmTemperatur->isChecked();
        dummy2.impulseProLiter = (uint16_t)ui->optionenDfmImpulse->value();

        core->setAlarm(dummy);
        core->setDfm(dummy2);
        core->sendDfmData();
    }
}

void MainWindow::updateConflictAnzeige(int value){
    anzeige_s dummy;
    if(core->getConflict().init){
        dummy.backlight = (uint8_t)ui->optionenAnzeigeBeleuchtung->value();
        dummy.kontrast = (uint8_t)ui->optionenAnzeigeKontrast->value();
        dummy.dfm = (uint8_t)ui->optionenDfmShow->isChecked();
        core->setAnzeige(dummy);
        core->sendAnzeigeData();
    }
}

void MainWindow::updateConflictTemperaturAktiv(int value){
    temperatur_s dummy = core->getTemperature(value);
    if(core->getConflict().init){
        dummy.aktiv = temperaturAktiv[value]->isChecked();
        core->setTemperature(dummy,value);
        updateTemperaturValue();
        core->sendNamen();
    }
}

void MainWindow::updateConflictTemperaturNamen(int value){
    temperatur_s dummy = core->getTemperature(value);
    if(core->getConflict().init){
        dummy.name = temperaturName[value]->text();
        core->setTemperature(dummy,value);
        updateTemperaturNamen();
        core->sendNamen();
    }
}

void MainWindow::updateConflictAidaErsatz(int value){
    temperatur_s dummy = core->getTemperature(value);
    if(core->getConflict().init){
        dummy.ersatzTemperatur = temperaturAidaErsatz[value]->value();
        core->setTemperature(dummy,value);
        core->sendErsatzTemps();
        core->sendNamen();
    }
}
