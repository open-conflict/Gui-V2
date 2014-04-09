#include <conflict_core.h>
#include <qextserialport.h>
#include <qextserialenumerator.h>
#include <QSignalMapper>
#include <QObject>
#include <mainwindow.h>
#include <typedef.h>
#include <QSharedMemory>
#include <QBuffer>
#include <QString>

struct serial{
    QextSerialPort *port;
    QList<QextPortInfo> portlist;
    PortSettings settings;
    uint8_t portNr;
    uint8_t open;
}serial;

temperatur_s temperatur[24];
luefter_s luefter[3];
led_s Led[3];
ledModus_s ledModus;
dfm_s dfm;
anzeige_s anzeige;
analog_s analog;
alarm_s alarm;
computer_s computer;
oneWireSensor_s oneWireSensor[8];
uhrzeit_s uhrzeit;
luefter_temp_s luefterTemp[3][24];
luefter_temp_s analogTemp[24];
firmware_s firmware;
conflict_s conflict;

QString telegram;

Ui::MainWindow *ui;

MainWindow *mw;

QSharedMemory aida;

conflict_core::conflict_core()
{
    serial.portlist = QextSerialEnumerator::getPorts();
}

void conflict_core::openAida(){
    aida.setKey("AIDA64_SensorValues");
    if (aida.isAttached()){
          aida.detach();
    }
    if (!aida.create(10000)){
        conflict.aidaOpen = false;
    }else{
        conflict.aidaOpen = true;
    }
}

void conflict_core::updateAida(){
    QBuffer buffer;
    QDataStream in(&buffer);
    QString text;
    if(conflict.aidaOpen){
        aida.lock();
        buffer.setData((char*)aida.constData(), aida.size());
        buffer.open(QBuffer::ReadOnly);
        in >> text;
        ui->debugOutputOut->append(text);
        aida.unlock();
    }
}

QextSerialPort *conflict_core::connectSerial(int port){
    serial.port = new QextSerialPort(serial.portlist.at(port).portName,serial.settings);
    if(serial.port->open(QextSerialPort::ReadWrite)){
        serial.portNr = port;
        serial.open = true;
        conflict.serialOpen = true;
        return serial.port;
    }else{
        return NULL;
    }
}

void conflict_core::closeSerial(){
    serial.port->close();
    serial.portNr = 0;
    serial.open = false;
    conflict.init = false;
    conflict.serialOpen = false;
}

/*==========================*\
|         EMPFANGEN          |
\*==========================*/

void conflict_core::getData(){
    char data;
    char *dataPtr;
    dataPtr = &data;
    int16_t read;  // Anzahl gelesener Bytes
    while(serial.port->bytesAvailable()){
        read = serial.port->read(dataPtr,1);
        if(read > 0){
            telegram.append(QString(*dataPtr));
        }
        if(*dataPtr == '\n'){
            parseData();
            ui->debugOutputIn->append(telegram);
            telegram.clear();
        }
    }
}

void conflict_core::parseData(){
    int i,j;
    QList<QString> data = telegram.split('#');
    if(data.value(0).operator ==("STRE")){
        for(i=0;i<16;i++){
                temperatur[i].value = (uint8_t)data.value(i+2).toInt();
        }
        alarm.status        = (uint8_t)data.value(18).toInt();
        dfm.durchfluss      = (uint16_t)data.value(19).toInt();
        luefter[0].drehzahl = (uint32_t)data.value(20).toInt();
        luefter[1].drehzahl = (uint32_t)data.value(21).toInt();
        luefter[2].drehzahl = (uint32_t)data.value(22).toInt();
        luefter[0].pwn      = (uint8_t)(data.value(23).toInt()*100/255);
        luefter[1].pwn      = (uint8_t)(data.value(24).toInt()*100/255);
        luefter[2].pwn      = (uint8_t)(data.value(25).toInt()*100/255);
        mw->updateAll();
    }else if(data.value(0).operator ==("INIT")){
        for(i=0;i<16;i++){
                analogTemp[i].min = (uint8_t)data.value((48*j)+i+4).toInt();
        }
        for(i=0;i<16;i++){
                analogTemp[i].max = (uint8_t)data.value((48*j)+i+28).toInt();
        }
        for(j=0;j<3;j++){
            for(i=0;i<16;i++){
                    luefterTemp[j][i].min = (uint8_t)data.value((48*j)+i+52).toInt();
            }
            for(i=0;i<16;i++){
                    luefterTemp[j][i].max = (uint8_t)data.value((48*j)+i+76).toInt();
            }
        }
        for(j=0;j<3;j++){
            luefter[j].manuell = (uint8_t)data.value(201+(5*j)).toInt();
            luefter[j].anlaufzeit = (uint8_t)data.value(202+(5*j)).toInt();
            luefter[j].minDrehzahl = (uint8_t)data.value(203+(5*j)).toInt();
            luefter[j].autoRegelung = (uint8_t)data.value(204+(5*j)).toInt();
            luefter[j].off = (uint8_t)data.value(205+(5*j)).toInt();
        }
        luefter[0].anlaufSchwelle = (uint8_t)data.value(328).toInt();
        luefter[1].anlaufSchwelle = (uint8_t)data.value(329).toInt();
        luefter[2].anlaufSchwelle = (uint8_t)data.value(330).toInt();

        analog.manuell      = (uint8_t)data.value(196).toInt();
        analog.anlaufzeit   = (uint8_t)data.value(197).toInt();
        analog.minDrehzahl  = (uint8_t)data.value(198).toInt();
        analog.autoRegelung = (uint8_t)data.value(199).toInt();
        analog.off          = (uint8_t)data.value(200).toInt();
        analog.anlaufSchwelle = (uint8_t)data.value(327).toInt();

        alarm.luefter = (uint8_t)data.value(216).toInt();
        alarm.temperatur = (uint8_t)data.value(241).toInt();
        alarm.dfm = (uint16_t)data.value(3).toInt();
        for(i=0;i<24;i++){
            temperatur[i].name = data.value(217+i).toLatin1();
        }
        for(i=0;i<8;i++){
            temperatur[i].ersatzTemperatur = (uint8_t)data.value(242+i).toInt();
        }
        dfm.impulseProLiter = (uint8_t)data.value(2).toInt();
        anzeige.backlight = (uint8_t)data.value(253).toInt();
        anzeige.kontrast = (uint8_t)data.value(254).toInt();
        anzeige.dfm = (uint8_t)data.value(255).toInt();

        ledModus.value = (uint8_t)data.value(256).toInt();
        Led[0].value = (uint8_t)data.value(257).toInt();
        Led[1].value = (uint8_t)data.value(258).toInt();
        Led[2].value = (uint8_t)data.value(259).toInt();
        firmware.version = data.value(324)+(QString)"."+data.value(325)+(QString)"."+data.value(326);
        for(i=0;i<8;i++){
            oneWireSensor[i].adresse = data.value(260+(i*8))+(QString)" "+data.value(261+(i*8))+(QString)" "+data.value(262+(i*8))+(QString)" "+data.value(263+(i*8))+(QString)" "+data.value(264+(i*8))+(QString)" "+data.value(265+(i*8))+(QString)" "+data.value(266+(i*8))+(QString)" "+data.value(267+(i*8));
        }

        uint8_t byte;
        for(i=0;i<3;i++){
            byte = (uint8_t)data.value(250+i).toUInt();
            for(j=0;j<8;j++){
                temperatur[j+(i*8)].aktiv = byte & 0x01;
                byte = byte >> 1;
            }
        }
        mw->initAll();
        conflict.init = true;
    }
}

/*==========================*\
|           SENDEN           |
\*==========================*/


void conflict_core::sendInit(){
    sendString(QString("INST#3#1#\r\n"));
}

void conflict_core::sendExit(){
    sendString(QString("EXIT#2#\r\n"));
}

void conflict_core::sendReset(){
    sendString(QString("REST#3#DOIT#\r\n"));

}

void conflict_core::sendPcData(){
    sendString(  QString("PCRE#17#")
                +QString::number(temperatur[16].value)
                +QString("#")
                +QString::number(temperatur[17].value)
                +QString("#")
                +QString::number(temperatur[18].value)
                +QString("#")
                +QString::number(temperatur[19].value)
                +QString("#")
                +QString::number(temperatur[20].value)
                +QString("#")
                +QString::number(temperatur[21].value)
                +QString("#")
                +QString::number(temperatur[22].value)
                +QString("#")
                +QString::number(temperatur[23].value)
                +QString("#")
                +QString::number(uhrzeit.stunde)
                +QString("#")
                +QString::number(uhrzeit.minute)
                +QString("#")
                +QString::number(uhrzeit.sekunde)
                +QString("#")
                +QString::number(computer.takt)
                +QString("#")
                +QString::number(computer.cpu)
                +QString("#")
                +QString::number(computer.gpu)
                +QString("#")
                +QString::number(computer.ram)
                +QString("#\r\n")
              );
}

void conflict_core::sendDfmData(){
    sendString(  QString("DFM#6#")
                +QString::number(dfm.impulseProLiter)
                +QString("#")
                +QString::number(alarm.dfm)
                +QString("#")
                +QString::number(alarm.temperatur)
                +QString("#")
                +QString::number(alarm.luefter)
                +QString("#\r\n")
           );
}

void conflict_core::sendMinMax(uint8_t kanal){
    uint8_t i;
    QString str;
    str.append("MIA");
    if(kanal < 3 || kanal == 0){
        str.append(QString::number(kanal));
    }
    else{
        str.append("A");
    }
    str.append("#50#");
    for(i=0;i<24;i++){
        str.append(QString::number(temperatur[i].min));
        str.append("#");
    }
    for(i=0;i<24;i++){
        str.append(QString::number(temperatur[i].max));
        str.append("#");
    }
    str.append("#\r\n");
    sendString(str);
}

void conflict_core::sendKanalData(uint8_t kanal){
    QString str;
    str.append("KAN");
    if(kanal < 3 || kanal == 0){
        str.append(QString::number(kanal));
        str.append("#8#");
        str.append(QString::number(luefter[kanal].manuell));
        str.append("#");
        str.append(QString::number(luefter[kanal].anlaufzeit));
        str.append("#");
        str.append(QString::number(luefter[kanal].minDrehzahl));
        str.append("#");
        str.append(QString::number(luefter[kanal].autoRegelung));
        str.append("#");
        str.append(QString::number(luefter[kanal].off));
        str.append("#");
        str.append(QString::number(luefter[kanal].anlaufSchwelle));
        str.append("#\r\n");
    }
    else{
        str.append("A");
        str.append("#8#");
        str.append(QString::number(analog.manuell));
        str.append("#");
        str.append(QString::number(analog.anlaufzeit));
        str.append("#");
        str.append(QString::number(analog.minDrehzahl));
        str.append("#");
        str.append(QString::number(analog.autoRegelung));
        str.append("#");
        str.append(QString::number(analog.off));
        str.append("#");
        str.append(QString::number(analog.anlaufSchwelle));
        str.append("#\r\n");
    }
    sendString(str);
}

void conflict_core::sendNamen(){
    uint8_t i;
    QString str;
    str.append("NAME#29#");
    for(i=0;i<24;i++){
        str.append(temperatur[i].name);
        str.append("#");
    }
    str.append(0xff);
    str.append("#");
    str.append(0xff);
    str.append("#");
    str.append(0xff);
    str.append("#\r\n");
    sendString(str);
}

void conflict_core::sendErsatzTemps(){
    uint8_t i;
    QString str;
    str.append("ERSA#10#");
    for(i=0;i<8;i++){
        str.append(QString::number(temperatur[i+17].ersatzTemperatur));
        str.append("#");
    }
    str.append(0xff);
    str.append("#");
    str.append(0xff);
    str.append("#");
    str.append(0xff);
    str.append("#\r\n");
    sendString(str);
}

void conflict_core::sendAnzeigeData(){
    QString str;
    str.append("ANZE#5#");
    str.append(QString::number(anzeige.backlight));
    str.append("#");
    str.append(QString::number(anzeige.kontrast));
    str.append("#");
    str.append(QString::number(anzeige.dfm));
    str.append("#\r\n");
    sendString(str);
}

void conflict_core::sendLedData(){
    QString str;
    str.append("LED#6#");
    str.append(QString::number(ledModus.value));
    str.append("#");
    str.append(QString::number(Led[0].value));
    str.append("#");
    str.append(QString::number(Led[1].value));
    str.append("#");
    str.append(QString::number(Led[2].value));
    str.append("#\r\n");
    sendString(str);
}

void conflict_core::sendString(QString str){
    if(conflict.serialOpen){
        serial.port->write(str.toLatin1(),str.length());
        ui->debugOutputOut->append(str);
    }else{
        ui->debugOutputOut->append("Jung! Das geht ned. Erst verbinden.");
    }
}


/*==========================*\
|           SETTER           |
\*==========================*/


void conflict_core::setPortSettings(PortSettings settings){
    serial.settings = settings;
}

void conflict_core::setUi(Ui::MainWindow *uiuiuiui){
    ui = uiuiuiui;
}

void conflict_core::setLuefter(luefter_s data, uint8_t nr){
    luefter[nr] = data;
}

void conflict_core::setLed(led_s data, uint8_t nr){
    Led[nr] = data;
}

void conflict_core::setLedModus(ledModus_s modus){
    ledModus = modus;
}
void conflict_core::setAnzeige(anzeige_s data){
    anzeige = data;
}

void conflict_core::setTemperature(temperatur_s data, uint8_t nr){
    temperatur[nr] = data;
}

void conflict_core::setDfm(dfm_s data){
    dfm = data;
}

void conflict_core::setAnalog(analog_s data){
    analog = data;
}

void conflict_core::setAlarm(alarm_s data){
    alarm = data;
}

void conflict_core::setComputer(computer_s data){
    computer = data;
}

void conflict_core::setUhrzeit(uhrzeit_s data){
    uhrzeit = data;
}

void conflict_core::setOneWireSensor(oneWireSensor_s data, uint8_t nr){
    oneWireSensor[nr] = data;
}

void conflict_core::setMw(MainWindow *mainwindows){
    mw = mainwindows;
}

void conflict_core::setLuefterTemp(luefter_temp_s data, uint8_t luefter, uint8_t temperatur){
    luefterTemp[luefter][temperatur] = data;
}

void conflict_core::setAnalogTemp(luefter_temp_s data, uint8_t temperatur){
    analogTemp[temperatur] = data;
}

void conflict_core::setFirmware(firmware_s data){
    firmware = data;
}

void conflict_core::setConflict(conflict_s data){
    conflict = data;
}

/*==========================*\
|           GETTER           |
\*==========================*/


luefter_s conflict_core::getLuefter(uint8_t nr){
    return luefter[nr];
}

led_s conflict_core::getLed(uint8_t nr){
    return Led[nr];
}

ledModus_s conflict_core::getLedModus(){
    return ledModus;
}

anzeige_s conflict_core::getAnzeige(){
    return anzeige;
}

temperatur_s conflict_core::getTemperature(uint8_t nr){
    return temperatur[nr];
}

dfm_s conflict_core::getDfm(){
    return dfm;
}

analog_s conflict_core::getAnalog(){
    return analog;
}

alarm_s conflict_core::getAlarm(){
    return alarm;
}

computer_s conflict_core::getComputer(){
    return computer;
}

uhrzeit_s conflict_core::getUhrzeit(){
    return uhrzeit;
}

oneWireSensor_s conflict_core::getOneWireSensor(uint8_t nr){
    return oneWireSensor[nr];
}

luefter_temp_s conflict_core::getLuefterTemp(uint8_t luefter, uint8_t temperatur){
    return luefterTemp[luefter][temperatur];
}

luefter_temp_s conflict_core::getAnalogTemp(uint8_t temperatur){
    return analogTemp[temperatur];
}

firmware_s conflict_core::getFirmware(){
    return firmware;
}

conflict_s conflict_core::getConflict(){
    return conflict;
}




