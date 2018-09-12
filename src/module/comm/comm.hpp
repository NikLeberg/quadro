/**
* @file    comm.hpp
* @brief   Kommunikations-Schnittstelle
*
* Schnittstelle für den gemeinsamen Zugriff auf die verschienen Bussysteme.
* @author  Niklaus R. Leuenberger
* @date    16/07/2018
*/

#ifndef comm_H
#define comm_H

///
/// Includes
///
#include <Arduino.h>
#include <Wire.h>
#include "module\pins.hpp"

namespace comm {
  namespace i2c {
    /**
    * @brief  I2C-Setup
    *
    * Initialisiert Wire-Library mit auf den definierten Pins, definiert Bustakt
    * und maximale Slave-Verzögerung.
    */
    void setup();

    /**
    * @brief  Lese Byte
    *
    * Liest ein Byte aus I2C-Gerät
    * @param  uint8_t dev Geräteadresse
    * @param  uint8_t reg Registeradresse
    * @return uint8_t Registerwert
    */
    uint8_t readByte(uint8_t dev, uint8_t reg);

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
    bool readBytes(uint8_t dev, uint8_t startReg, uint8_t data[], uint8_t numBytes);

    /**
    * @brief  Schreibe Byte
    *
    * Schreibt ein Byte in I2C-Gerät
    * @param  uint8_t dev Geräteadresse
    * @param  uint8_t reg Registeradresse
    * @param  uint8_t data zu schreibender Wert
    * @return bool true bei Fehler, sonst false
    */
    bool writeByte(uint8_t dev, uint8_t reg, uint8_t data);

    /**
    * @brief  Befreie Bus
    *
    * Befreit Bus manuell falls nach einem Neustart ein Slave die Datenleitung
    * low hält.
    * @note   wirkungslos wenn Slave Taktleitung low hält
    * @return bool false bei Erfolg, true bei Misserfolg
    */
    bool freeBus();
  }

  namespace spi {

  }
}

#endif /* end of include guard: comm_H */
