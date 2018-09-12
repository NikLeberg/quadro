/**
* @file    config.hpp
* @brief   Konfigurationsspeicher
*
* Liest und schreibt die Konfiguration aus einer Konfigurations-Datei.
* @author  Niklaus R. Leuenberger
* @date    22/07/2018
*/

#ifndef config_H
#define config_H

///
/// Defines
///
/** Speicherpfad der Konfigurations-Datei*/
#define CONF_FILE "/config.bin"
/** Anzahl an Variablen in vars-Array*/
#define CONF_NUM 38

///
/// Fehlercodes
///
/** Kein Fehler */
#define ERR_NO_ERR 0
/** Dateisystem konnte nicht formartiert werden */
#define ERR_CF_FORMAT 1
/** Dateisystem beschädigt */
#define ERR_CF_FS 2
/** Datei beschädigt */
#define ERR_CF_FILE 3

///
/// Includes
///
#include <Arduino.h>
#include <SPIFFS.h>

/// Klasse
class configClass {
  public:
    configClass() : vars {
      // Yaw-Regler
      {{'y','a','w','E','n'}, false, false},
      {{'y','a','w','K','p'}, false, 0.0f},
      {{'y','a','w','K','i'}, false, 0.0f},
      {{'y','a','w','K','d'}, false, 0.0f},
      {{'y','a','w','O','f','f'}, false, (int16_t)0},
      // Pitch-Regler
      {{'p','i','t','c','h','E','n'}, false, false},
      {{'p','i','t','c','h','K','p'}, false, 0.0f},
      {{'p','i','t','c','h','K','i'}, false, 0.0f},
      {{'p','i','t','c','h','K','d'}, false, 0.0f},
      {{'p','i','t','c','h','O','f','f'}, false, (int16_t)0},
      // Roll-Regler
      {{'r','o','l','l','E','n'}, false, false},
      {{'r','o','l','l','K','p'}, false, 0.0f},
      {{'r','o','l','l','K','i'}, false, 0.0f},
      {{'r','o','l','l','K','d'}, false, 0.0f},
      {{'r','o','l','l','O','f','f'}, false, (int16_t)0},
      // Kalibrationsprofil BNO055
      {{'c','a','l','A','c','c','X'}, true, (int16_t)10},
      {{'c','a','l','A','c','c','Y'}, true, (int16_t)-29},
      {{'c','a','l','A','c','c','Z'}, true, (int16_t)12},
      {{'c','a','l','M','a','g','X'}, true, (int16_t)-481},
      {{'c','a','l','M','a','g','Y'}, true, (int16_t)-45},
      {{'c','a','l','M','a','g','Z'}, true, (int16_t)-263},
      {{'c','a','l','G','y','r','X'}, true, (int16_t)0},
      {{'c','a','l','G','y','r','Y'}, true, (int16_t)0},
      {{'c','a','l','G','y','r','Z'}, true, (int16_t)0},
      {{'c','a','l','A','c','c','R','a','d'}, true, (int16_t)1000},
      {{'c','a','l','M','a','g','R','a','d'}, true, (int16_t)845},
      // Maximalwerte der Roll- und Pitch-Achsen
      {{'a','x','i','s','M','a','x'}, false, (int16_t)450},
      // Update Rate in Hz des schnellen Loops
      {{'f','a','s','t','L','o','o','p'}, false, (uint16_t)100},
      // Steuerintensität
      {{'i','n','t','Y','a','w'}, false, 1.0f},
      {{'i','n','t','R','o','l','l'}, false, 1.0f},
      {{'i','n','t','P','i','t','c','h'}, false, 1.0f},
      {{'i','n','t','T','h','r'}, false, 1.0f},
      // Throttle Modus. (0 = Relativ, 1 = Absolut)
      {{'a','b','s','T','h','r'}, false, true},
      // Aktivierte Flugmodi
      {{'a','l','l','o','w','H','o','v'}, false, false},
      {{'a','l','l','o','w','M','a','n'}, false, true},
      {{'a','l','l','o','w','S','u','p'}, false, false},
      {{'a','l','l','o','w','A','u','t','o'}, false, false},
      {{'a','l','l','o','w','F','u','l','l'}, false, false}
    } {};

    static const uint8_t num = CONF_NUM;
    struct configVariable {
      configVariable(std::array<char, 10> newName, bool newHide, bool newValue) :
        name(newName), type(0), hide(newHide), b(newValue) {};
      configVariable(std::array<char, 10> newName, bool newHide, uint8_t newValue) :
        name(newName), type(1), hide(newHide), ui8(newValue) {};
      configVariable(std::array<char, 10> newName, bool newHide, uint16_t newValue) :
        name(newName), type(2), hide(newHide), ui16(newValue) {};
      configVariable(std::array<char, 10> newName, bool newHide, uint32_t newValue) :
        name(newName), type(3), hide(newHide), ui32(newValue) {};
      configVariable(std::array<char, 10> newName, bool newHide, int8_t newValue) :
        name(newName), type(4), hide(newHide), i8(newValue) {};
      configVariable(std::array<char, 10> newName, bool newHide, int16_t newValue) :
        name(newName), type(5), hide(newHide), i16(newValue) {};
      configVariable(std::array<char, 10> newName, bool newHide, int32_t newValue) :
        name(newName), type(6), hide(newHide), i32(newValue) {};
      configVariable(std::array<char, 10> newName, bool newHide, float newValue) :
        name(newName), type(7), hide(newHide), f(newValue) {};

      std::array<char, 10> name; // Variablenname
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
    configVariable* get(String name, bool &null, uint8_t *index = NULL);
    bool            del();

    struct configVariable vars[CONF_NUM];

  private:
    // Keine
};

#endif /* end of include guard: config_H */
