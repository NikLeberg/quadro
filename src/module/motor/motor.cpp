/**
* @file    motor.cpp
* @brief   Motoransteuerung
*
* Steuert einen einzelnen Motor an.
* @author  Niklaus R. Leuenberger
* @date    15/07/2018
*/

#include "motor.hpp"

/**
* @brief  Setup
*
* Verknüpft Motor mit Pin in Servo-Library und initialisiert ihn.
* @return bool true bei Fehler, sonst false
*/
bool motor::setup() {
  if (_m.attach(_pin)) return true;
  _m.writeMicroseconds(_mOff);
  return false;
}

/**
* @brief  Motoransteuerung
*
* Drosselt Motor auf gewünschten prozentualen Wert.
* @param  uint16_t percent Prozentuale Ansteuerung 0 - 0.0% bis 1000 - 100.0%
* @return bool true bei Fehler
*/
bool motor::throttle(uint16_t percent) {
  if (!_armed) return true;
  percent = constrain(percent, 0, 1000);
  _m.writeMicroseconds(map(percent, 0, 1000, _mMin, _mMax));
  return false;
}

/**
* @brief  Schärfe/Entschärfe Motor
*
* Stellt den Motor scharf oder entschärft ihn.
* @param  bool armed gewünschter Zustand
*/
void motor::setArmed(bool armed) {
  _armed = armed;
  if (_armed) {
    _m.writeMicroseconds(_mMin);
  } else {
    _m.writeMicroseconds(_mOff);
  }
}

/**
* @brief  Abfrage des Zustandes
*
* Gibt zurück ob der Motor scharfgestellt ist oder nicht.
* @return bool Zustand
*/
bool motor::isArmed() {
  return _armed;
}
