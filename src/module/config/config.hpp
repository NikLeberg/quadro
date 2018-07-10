/**
* @file    config.hpp
* @brief   Konfigurationsspeicher
*
* Liest und schreibt die Konfiguration aus einer Konfigurations-Datei.
* @author  Niklaus R. Leuenberger
* @date    10/07/2018
*/

#ifndef config_H
#define config_H

///
/// Defines
///
/** Speicherpfad der Konfigurations-Datei*/
#define CONF_FILE "/config.bin"
/** Anzahl an Variablen in vars-Array*/
#define CONF_NUM 39

///
/// Fehlercodes
///
/** Kein Fehler */
#define ERR_NO_ERR 0
/** Dateisystem beschädigt */
#define ERR_CF_FS 1
/** Datei beschädigt */
#define ERR_CF_FILE 2

///
/// Includes
///
#include <Arduino.h>
#include <SPIFFS.h>

/// Klasse
class config {
  public:
    config();

    static const uint8_t num = CONF_NUM;
    struct configVariable {
      char name[10];   // Variablenname
      uint8_t     type;       // Datentyp
      bool        hide;       // Sichtbar auf Website?
      union {                 // Geteilter Datenspeicher
        bool      b;          // 0: True - False
        uint8_t   ui8;        // 1: 0 - 255
        uint16_t  ui16;       // 2: 0 - 65'535
        uint32_t  ui32;       // 3: 0 - 4'294'967'296
        int8_t    i8;         // 4: -127 - +128
        int16_t   i16;        // 5: -32'767 - +32'768
        int32_t   i32;        // 6: -2'147'483'647 - +2'147'483'648
        float     f;          // 7: -Infinity - +Infinity
      };

    };
    uint8_t         setup();

    uint8_t         read();
    uint8_t         write();
    void            format(uint8_t name, String var);
    configVariable* get(String name);
    bool            del();
    void            setDefault();

    struct configVariable vars[CONF_NUM] = {
      // Yaw-Regler
      {name : "yawEn", type : 0, hide : false, {.b = false}},
      {name : "yawKp", type : 7, hide : false, {.f = 0.0f}},
      {name : "yawKi", type : 7, hide : false, {.f = 0.0f}},
      {name : "yawKd", type : 7, hide : false, {.f = 0.0f}},
      {name : "yawOff", type : 7, hide : false, {.f = 0.0f}},
      // Pitch-Regler
      {name : "pitchEn", type : 0, hide : false, {.b = false}},
      {name : "pitchKp", type : 7, hide : false, {.f = 0.0f}},
      {name : "pitchKi", type : 7, hide : false, {.f = 0.0f}},
      {name : "pitchKd", type : 7, hide : false, {.f = 0.0f}},
      {name : "pitchOff", type : 7, hide : false, {.f = 0.0f}},
      // Roll-Regler
      {name : "rollEn", type : 0, hide : false, {.b = false}},
      {name : "rollKp", type : 7, hide : false, {.f = 0.0f}},
      {name : "rollKi", type : 7, hide : false, {.f = 0.0f}},
      {name : "rollKd", type : 7, hide : false, {.f = 0.0f}},
      {name : "rollOff", type : 7, hide : false, {.f = 0.0f}},
      // Kalibrationsprofil BNO055
      {name : "calAccX", type : 5, hide : true, {.i16 = 10}},
      {name : "calAccY", type : 5, hide : true, {.i16 = -29}},
      {name : "calAccZ", type : 5, hide : true, {.i16 = 12}},
      {name : "calMagX", type : 5, hide : true, {.i16 = -481}},
      {name : "calMagY", type : 5, hide : true, {.i16 = -45}},
      {name : "calMagZ", type : 5, hide : true, {.i16 = -263}},
      {name : "calGyrX", type : 5, hide : true, {.i16 = 0}},
      {name : "calGyrY", type : 5, hide : true, {.i16 = 0}},
      {name : "calGyrZ", type : 5, hide : true, {.i16 = 0}},
      {name : "calAccRad", type : 5, hide : true, {.i16 = 1000}},
      {name : "calMagRad", type : 5, hide : true, {.i16 = 845}},
      // Maximalwerte der Roll- und Pitch-Achsen
      {name : "pitchMax", type : 7, hide : false, {.f = 60.0f}},
      {name : "rollMax", type : 7, hide : false, {.f = 60.0f}},
      // System Clock resp. Update-Intervall in us
      {name : "sysUpdate", type : 3, hide : false, {.ui32 = 50000}},
      // Steuerintensität
      {name : "intYaw", type : 7, hide : false, {.f = 1.0f}},
      {name : "intRoll", type : 7, hide : false, {.f = 1.0f}},
      {name : "intPitch", type : 7, hide : false, {.f = 1.0f}},
      {name : "intThr", type : 7, hide : false, {.f = 1.0f}},
      // Throttle Modus. (0 = Relativ, 1 = Absolut)
      {name : "absThr", type : 0, hide : false, {.b = true}},
      // Aktivierte Flugmodi
      {name : "allowHov", type : 0, hide : false, {.b = false}},
      {name : "allowMan", type : 0, hide : false, {.b = true}},
      {name : "allowSup", type : 0, hide : false, {.b = false}},
      {name : "allowAuto", type : 0, hide : false, {.b = false}},
      {name : "allowFull", type : 0, hide : false, {.b = false}}
    };

  private:
    // Keine
};

#endif /* end of include guard: config_H */
