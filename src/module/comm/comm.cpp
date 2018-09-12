/**
* @file    comm.cpp
* @brief   Kommunikations-Schnittstelle
*
* Schnittstelle für den gemeinsamen Zugriff auf die verschienen Bussysteme.
* @author  Niklaus R. Leuenberger
* @date    16/07/2018
*/

#include "comm.hpp"

/**
* @brief  I2C-Setup
*
* Initialisiert Wire-Library mit auf den definierten Pins, definiert Bustakt
* und maximale Slave-Verzögerung.
*/
void comm::i2c::setup() {
  Wire.begin(pins::SDA, pins::SCL, 400000L); // 400kHz
  Wire.setTimeOut(1000);
}

/**
* @brief  Lese Byte
*
* Liest ein Byte aus I2C-Gerät
* @param  uint8_t dev Geräteadresse
* @param  uint8_t reg Registeradresse
* @return uint8_t Registerwert
*/
uint8_t comm::i2c::readByte(uint8_t dev, uint8_t reg) {
  Wire.beginTransmission(dev);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(dev, (uint8_t)1);
  return Wire.read();
}

/**
* @brief  Lese Bytes
*
* Lese mehrere Bytes aus I2C-Gerät
* @param  uint8_t dev Geräteadresse
* @param  uint8_t startReg Startregister
* @param  uint8_t data[] Array für Registerwerte
* @param  uint8_t numBytes Anzahl Bytes
* @return bool true bei Fehler, sonst false
*/
bool comm::i2c::readBytes(uint8_t dev, uint8_t startReg, uint8_t data[], uint8_t numBytes) {
  Wire.beginTransmission(dev);
  Wire.write(startReg);
  if (Wire.endTransmission()) return true;
  if (Wire.requestFrom(dev, numBytes) < numBytes) return true;
  for (uint8_t i = 0; i < numBytes; i++) {
    data[i] = Wire.read();
  }
  return false;
}

/**
* @brief  Schreibe Byte
*
* Schreibt ein Byte in I2C-Gerät
* @param  uint8_t dev Geräteadresse
* @param  uint8_t reg Registeradresse
* @param  uint8_t data zu schreibender Wert
* @return bool true bei Fehler, sonst false
*/
bool comm::i2c::writeByte(uint8_t dev, uint8_t reg, uint8_t data) {
  Wire.beginTransmission(dev);
  Wire.write(reg);
  Wire.write(data);
  return Wire.endTransmission();
}

/**
* @brief  Befreie Bus
*
* Befreit Bus manuell falls nach einem Neustart ein Slave die Datenleitung
* low hält.
* @note   wirkungslos wenn Slave Taktleitung low hält
* @return bool false bei Erfolg, true bei Misserfolg
*/
bool comm::i2c::freeBus() {
  pinMode(pins::SCL, INPUT_PULLUP); // setze SCL und SDA Pins als Input mit Pullup
  pinMode(pins::SDA, INPUT_PULLUP);
  delayMicroseconds(10); // warte 10us
  if (digitalRead(pins::SCL) == LOW) { // prüfe ob Clock-Pin vom Slave auf LOW gezogen wird
    return true;
  }
  if (digitalRead(pins::SDA) == LOW) { // prüfe ob DatenPin vom Slave auf Low gezogen wird
    uint8_t clockCount = 20; // versuche im Maximum 20 Mal
    while ((digitalRead(pins::SDA) == LOW) && (clockCount > 0)) { // toggle ClockPin wenn SDA vom Slave auf LOW gezogen wird
      clockCount--;
      pinMode(pins::SCL, INPUT); // deaktiviere Pullup
      pinMode(pins::SCL, OUTPUT); // aktiviere Pulldown
      delayMicroseconds(10); // warte 10us
      pinMode(pins::SCL, INPUT); // gebe SCL wieder frei resp. deaktiviere Pulldown
      pinMode(pins::SCL, INPUT_PULLUP); //aktiviere Pullup
      delayMicroseconds(10); // warte 10us
      uint8_t counter = 20;
      while ((digitalRead(pins::SCL) == LOW) && (counter > 0)) { // warte maximal 0.2s bis der Slave SCL freigibt
        counter--;
        delay(10);
      }
      if ((digitalRead(pins::SCL) == LOW)) { // Slave hält SCL immernoch auf LOW
        return true;
      }
    }
    if (digitalRead(pins::SDA) == LOW) { // wenn nach 20 Clock-Signalen SDA immer noch nicht freigegeben ist
      return true;
    }
    pinMode(pins::SDA, INPUT); // deaktiviere Pullup
    pinMode(pins::SDA, OUTPUT); // aktiviere Pulldown (Start-Signal)
    delayMicroseconds(10); // warte 10us
    pinMode(pins::SDA, INPUT); // deaktiviere Pulldown
    pinMode(pins::SDA, INPUT_PULLUP); // aktiviere Pullup (Stop-Signal)
    delayMicroseconds(10); // warte 10us
  }
  pinMode(pins::SDA, INPUT); // setze SDA und SCL zurück auf normal
  pinMode(pins::SCL, INPUT);
  return false; // beende ohne Fehler
}
