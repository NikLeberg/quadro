/**
* @file    motor.hpp
* @brief   Motoransteuerung
*
* Steuert einen einzelnen Motor an.
* @author  Niklaus R. Leuenberger
* @date    15/07/2018
*/

#ifndef motor_H
#define motor_H

///
/// Includes
///
#include <Arduino.h>
#include <Servo.h>

///
/// Klasse
///
class motor {
  public:
    motor(uint8_t pin) : _pin(pin) {};

    bool setup();

    bool throttle(uint16_t percent);
    void setArmed(bool armed);
    bool isArmed();

  private:
    enum {
      _mMax = 2000,
      _mMin = 1000,
      _mOff = 0
    };
    uint8_t _pin;
    Servo   _m;
    bool    _armed = false;
};

#endif /* end of include guard: motor_H */
