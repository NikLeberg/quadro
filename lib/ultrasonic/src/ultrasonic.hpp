/*
  ultrasonic.hpp v0.1 - Bibliothek um ein Ultraschallsensor auzuwerten.
  Erstellt von Niklaus R. Leuenberger, August 20, 2018.
*/

#ifndef ULTRASONIC_h
#define ULTRASONIC_h

#include "Arduino.h"

class ultrasonic
{
  public:
    ultrasonic(uint8_t echo, uint8_t trigger);

    bool setup();
    bool update();
    bool getDistance(uint16_t &distance);
    static ultrasonic* instance() { return _instance; }

  private:
    static void _interrupt();
    uint8_t _echoPin, _triggerPin;
    volatile uint32_t _echoStart, _echoEnd, _timeout;
    volatile bool _finished, _timeoutError;
    static ultrasonic* _instance;
};

#endif
