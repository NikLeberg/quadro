/*
  BNO055.cpp v0.6 - Bibliothek um den Bosch Sensortec BNO055 9-Achsen-Sensor anzusprechen.
  Erstellt von Niklaus R. Leuenberger, Juli 16, 2018.
*/

#include "BNO055.hpp"

// Initialisiert BNO055
//
// Returncodes:
//  0 - Ok
//  1 - BNO055 nicht verbunden oder antwortet nicht
//  2 - Busfehler beim Konfigurieren aufgetreten
//  3 - Sensor-Systemfehler: Kein Fehler (sollte nicht vorkommen)
//  4 - Sensor-Systemfehler: Sensorinitialisierung fehlgeschlagen
//  5 - Sensor-Systemfehler: Systeminitialisierung fehlgeschlagen
//  6 - Sensor-Systemfehler: Selbsttest hat Problem erkannt
//  7 - Sensor-Systemfehler: Registerwert ausserhalb des gültigen Bereichs
//  8 - Sensor-Systemfehler: Registeradresse ausserhalb des gültigen Bereichs
//  9 - Sensor-Systemfehler: Schreibfehler auf Register
//  10 - Sensor-Systemfehler: Low-Power-Modus nicht verfügbar
//  11 - Sensor-Systemfehler: Accelerometer Power-Modus nicht verfügbar
//  12 - Sensor-Systemfehler: Konfigurationsfehler des Fusionsalgorithmus
//  13 - Sensor-Systemfehler: Konfigurationsfehler der Sensoren
//
uint8_t BNO055::setup() {
  // Prüfe Verbindung zum BNO055
  if (BNO055::_readByte(_addr, BNO055_CHIP_ID_ADDR) != BNO055_CHIP_ID) {
    // warte kurz um Sensor bei Power-On etwas Zeit zu geben
    delay(1000);
    // Prüfe Verbindung erneut
    if (BNO055::_readByte(_addr, BNO055_CHIP_ID_ADDR) != BNO055_CHIP_ID) {
      // immernoch keine Verbindung
      return 1;
    }
  }

  // Wechsle in Konfigurationsmodus
  if (BNO055::setMode(BNO055_MODE_CONFIGMODE)) return 2;

  // Setze Sensor zurück
  if (BNO055::_writeByte(_addr, BNO055_SYS_TRIGGER_ADDR, 0b00100000)) return 2;
  // warte bis Sensor neugestartet hat und wieder erreichbar ist (gem. Datenblatt nach 650ms)
  delay(700);
  uint8_t timeout = 100;
  while (BNO055::_readByte(_addr, BNO055_CHIP_ID_ADDR) != BNO055_CHIP_ID && timeout) {
    delay(10);
    timeout--;
    if (!timeout) return 1;
  }
  delay(50);

  // Setze Power-Modus auf Normal
  if (BNO055::_writeByte(_addr, BNO055_PWR_MODE_ADDR, 0b00000000)) return 2;
  delay(10);

  // Sensor-Register auf Seite 0 schalten
  if (BNO055::_writeByte(_addr, BNO055_PAGE_ID_ADDR, BNO055_PAGE_ZERO)) return 2;

  // Externen Oszillator aktivieren
  if (BNO055::_writeByte(_addr, BNO055_SYS_TRIGGER_ADDR, 0b10000000)) return 2;
  delay(10);

  // Konfiguriere die Achsenorientation (ev. bei definitivem Einbau zu ändern)
  if (BNO055::_writeByte(_addr, BNO055_AXIS_MAP_CONFIG_ADDR, 0x21)) return 2;
  if (BNO055::_writeByte(_addr, BNO055_AXIS_MAP_SIGN_ADDR, 0x02)) return 2;

  // Ändere Einheiten: Grad -> Radiant, dps -> rps, m/s2 -> mg
  if (BNO055::_writeByte(_addr, BNO055_UNIT_SEL_ADDR, 0b00000111)) return 2;

  // Sensor in Fusions-Modus (NDOF) setzen
  if (BNO055::setMode(BNO055_MODE_NDOF)) return 2;

  // Prüfe auf Internen Sensor-Fehler
  if (BNO055::_readByte(_addr, BNO055_SYS_STAT_ADDR) & 0b00000001) {
    return 3 + BNO055::_readByte(_addr, BNO055_SYS_ERR_ADDR);
  }

  return 0;
}

// Liest aktuellen Sensor Betriebsmodus
uint8_t BNO055::getMode() {
  _mode = BNO055::_readByte(_addr, BNO055_OPR_MODE_ADDR);
  return _mode;
}

// Setzt Sensor in gewünschten Betriebsmodus
bool BNO055::setMode(uint8_t bno_mode) {
  // Prüfe ob gewünschter Betriebsmodus gültig ist
  if (bno_mode < BNO055_MODE_CONFIGMODE || bno_mode > BNO055_MODE_NDOF) return true;
  // Prüfe aktuellen Betriebsmodus
  if (_mode == bno_mode) return false;
  // Setze Betriebsmodus
  if (BNO055::_writeByte(_addr, BNO055_OPR_MODE_ADDR, bno_mode)) return true;
  _mode = bno_mode;
  // Warte 30ms, Sensor benötigt typ. 19ms um den Betriebsmodus zu wechseln
  delay(30);
  return false;
}

// Liest die Euler-Winkel als Radiant aus (um Faktor 900 grösser als tats. Wert)
bool BNO055::getEuler(int16_t &heading, int16_t &roll, int16_t &pitch) {
  //if (_mode != BNO055_MODE_NDOF) return true;
  uint8_t data[6];
  if (BNO055::_readBytes(_addr, BNO055_EULER_H_LSB_ADDR, data, 6)) return true;
  heading = (int16_t)((data[1]) << 8 | (data[0]));
  roll    = (int16_t)((data[3]) << 8 | (data[2]));
  pitch   = (int16_t)((data[5]) << 8 | (data[4]));
  return false;
}

// Liest die Euler-Winkel aus (float)
bool BNO055::getEuler(float &heading, float &roll, float &pitch) {
  int16_t h, r, p;
  if (BNO055::getEuler(h, r, p)) return true;
  heading = h / 900.0;
  roll    = r / 900.0;
  pitch   = p / 900.0;
  return false;
}

// Liest die lineare Beschleunigung in mg aus
bool BNO055::getLinAcc(int16_t &x, int16_t &y, int16_t &z) {
  //if (_mode != BNO055_MODE_NDOF) return true;
  uint8_t data[6];
  if (BNO055::_readBytes(_addr, BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR, data, 6)) return true;
  x = (data[1]) << 8 | (data[0]);
  y = (data[3]) << 8 | (data[2]);
  z = (data[5]) << 8 | (data[4]);
  return false;
}

// Prüft Kalibrierungsstatus
// 0 - System vollständig Kalibriert
// 1 - Gyroskop nK
// 2 - Accelerometer nK
// 4 - Magnetometer nK
uint8_t BNO055::getCalibStat() {
  uint8_t ret = 0;
  uint8_t cal = BNO055::_readByte(_addr, BNO055_CALIB_STAT_ADDR);
  // System vollständig kalibriert
  if (cal == 0xFF) return 0;
  // Gyroskop nicht kalibriert
  if ((cal & 0b00110000) != 48) ret += 1;
  // Accelerometer nicht kalibriert
  if ((cal & 0b00001100) != 12) ret += 2;
  // Magnetometer nicht kalibriert
  if ((cal & 0b00000011) != 3) ret += 4;
  return ret;
}

// Liest Kalibrierungswerte (int16_t) aus
bool BNO055::getCalibData(int16_t data[11]) {
  // Prüfe ob die Sensoren vollständig Kalibriert wurden
  if (BNO055::getCalibStat()) return true;
  // Wechsle in CONFIGMODE
  uint8_t mode_bevore = BNO055::getMode();
  if (BNO055::setMode(BNO055_MODE_CONFIGMODE)) return true;
  // Daten auslesen
  uint8_t buffer[22];
  bool ret = BNO055::_readBytes(_addr, BNO055_ACCEL_OFFSET_X_LSB_ADDR, buffer, 22);
  for (uint8_t i = 0; i < 11; i++) {
    data[i] = (int16_t)((buffer[i*2 + 1]) << 8 | (buffer[i*2]));
  }
  // Wechlse zurück in vorherigen Betriebsmodus
  if (BNO055::setMode(mode_bevore)) return true;
  return ret;
}

// Schreibt Kalibrierungswerte (int16_t) in Sensor
bool BNO055::setCalibData(int16_t data[11]) {
  // Wechsle in CONFIGMODE
  uint8_t mode_bevore = BNO055::getMode();
  if (BNO055::setMode(BNO055_MODE_CONFIGMODE)) return true;
  // Daten schreiben
  bool ret = 0;
  for (uint8_t i = 0; i < 11; i++) {
    ret += BNO055::_writeByte(_addr, BNO055_ACCEL_OFFSET_X_LSB_ADDR + i*2, (uint8_t)data[i]);
    ret += BNO055::_writeByte(_addr, BNO055_ACCEL_OFFSET_X_LSB_ADDR + i*2 + 1, (uint8_t)(data[i]) >> 8);
  }
  // Wechlse zurück in vorherigen Betriebsmodus
  if (BNO055::setMode(mode_bevore)) return true;
  return ret;
}
