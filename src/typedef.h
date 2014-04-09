#ifndef TYPEDEF_H
#define TYPEDEF_H

struct conflict_s{
    bool init;
    bool serialOpen;
    bool aidaOpen;
};


struct luefter_s{
    QString name;
    uint32_t drehzahl;
    uint8_t pwn;
    uint8_t manuell;        // Manuelle Geschwindigkeit - Wert von 0-255, 0 ist aus. Für Analog Spannung von 0 - 120 (entspricht 0 - 12,0V)
    uint8_t anlaufzeit;     // Die Anlaufzeit in 0,5 Sekunden.
    uint8_t minDrehzahl;    // Wert von 0-255, hier ist der minimale PWM-Wert einzustellen bei dem sich der Lüfter noch dreht.
    uint8_t autoRegelung;   // 1 wenn Automatisch geregelt werden soll, 0 wenn manuell.
    uint8_t off;            // 1, wenn der Lüfter ausgeschaltet werden darf, 0 wenn er immer mindestens mit der minimalen Drehzahl laufen soll.
    uint8_t anlaufSchwelle; // Schwelle des Kühlbedarfs, bei der der Lüfter wieder losdreht wenn er ausgeschaltet war (nur bei Auto) 0-50 (%)
    uint8_t alarm;
    uint8_t minSpannung;
};

struct luefter_temp_s{
    uint8_t min;
    uint8_t max;
    uint8_t aktiv;
};

struct led_s{
    uint8_t value;          // Helligkeit des Led Kanals (0-255)
};

struct ledModus_s{
    uint8_t value;          // 0 = Manuell; 1, 2, 3 sind automatische Farbwechselmodi
};

struct anzeige_s{
    uint8_t backlight;      // Backlight Helligkeit 0-255
    uint8_t kontrast;       // Kontrast - 0-63
    uint8_t dfm;            // Gibt an ob der Bildschirm für Pumpe/DFM angezeigt werden soll
};

struct temperatur_s{
    uint8_t min;
    uint8_t max;
    uint8_t value;
    QString name;
    uint8_t alarm;
    uint8_t ersatzTemperatur;
    bool aktiv;
};

struct dfm_s{
    uint8_t impulseProLiter;
    uint16_t durchfluss;
};

struct analog_s{
    QString name;
    uint32_t drehzahl;
    uint8_t pwn;
    uint8_t manuell;        // Manuelle Geschwindigkeit - Wert von 0-255, 0 ist aus. Für Analog Spannung von 0 - 120 (entspricht 0 - 12,0V)
    uint8_t anlaufzeit;     // Die Anlaufzeit in 0,5 Sekunden.
    uint8_t minDrehzahl;    // Wert von 0-255, hier ist der minimale PWM-Wert einzustellen bei dem sich der Lüfter noch dreht.
    uint8_t autoRegelung;   // 1 wenn Automatisch geregelt werden soll, 0 wenn manuell.
    uint8_t off;            // 1, wenn der Lüfter ausgeschaltet werden darf, 0 wenn er immer mindestens mit der minimalen Drehzahl laufen soll.
    uint8_t anlaufSchwelle; // Schwelle des Kühlbedarfs, bei der der Lüfter wieder losdreht wenn er ausgeschaltet war (nur bei Auto) 0-50 (%)
    uint8_t alarm;
    uint8_t minSpannung;
};

struct alarm_s{
    uint16_t dfm;        // minimal Liter oder 0 = aus
    uint8_t temperatur; // 1 = an, 0 = aus
    uint8_t luefter;    // 1 = an, 0 = aus
    uint8_t status;     // 1 = an, 0 = aus
};

struct computer_s{
    uint32_t takt;  // CPU Takt in MHz (4-Stellig ASCII)
    uint16_t cpu;   // CPU Auslastung in % (3-Stellig ASCII)
    uint16_t ram;   // RAM (Hauptspeicher) Auslastung in % (3-Stellig ASCII)
    uint16_t gpu;   // GPU (Grafikkarte) Auslastung in % (3-Stellig ASCII)
};

struct uhrzeit_s{
    uint8_t stunde;
    uint8_t minute;
    uint8_t sekunde;
};

struct oneWireSensor_s{
    QString adresse;
};

struct firmware_s{
    QString version;
};

#endif
