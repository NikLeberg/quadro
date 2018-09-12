/*
  ultrasonic.cpp v0.1 - Bibliothek um ein Ultraschallsensor interruptgesteurt auzuwerten.
  Erstellt von Niklaus R. Leuenberger, September 12, 2018.
*/

#include "ultrasonic.hpp"

ultrasonic *ultrasonic::_instance(NULL);

// Konstruktor
ultrasonic::ultrasonic(uint8_t echo, uint8_t trigger) {
  _instance = this;
  _echoPin = echo;
  _triggerPin = trigger;
  _finished = true;
  //static ultrasonic* _instance;
}

// Initialisiere Sensor
bool ultrasonic::setup() {
  // Pins
  pinMode(_triggerPin, OUTPUT);
  pinMode(_echoPin, INPUT);

  // Teste auf Echo
  digitalWrite(_triggerPin, HIGH);
  delayMicroseconds(50);
  digitalWrite(_triggerPin, LOW);
  if (pulseIn(_echoPin, HIGH, 2000) == 0) {
    Serial.println("Sensor-Timeout!");
    return true;
  }
  digitalWrite(_triggerPin, HIGH);
  return false;
}

// Starte Aktualisierung
bool ultrasonic::update() {
  if (_finished) {
    attachInterrupt(digitalPinToInterrupt(_echoPin), _interrupt, CHANGE);
    digitalWrite(_triggerPin, LOW);
    _finished = false;
    _timeout = micros();
  } else {
    if (micros() - _timeout > 20000) {
      digitalWrite(_triggerPin, HIGH);
      detachInterrupt(_echoPin);
      _finished = true;
      _timeoutError = true;
      return _timeoutError;
    }
  }
  return _timeoutError;
}

// Lese Abstand aus Ultraschallsensor (in cm)
bool ultrasonic::getDistance(uint16_t &distance) {
  if (update()) return true;
  if (_echoStart < _echoEnd) {
    distance = (_echoEnd - _echoStart) / 58.0;
  }
  return false;
}

// Interrupt-Routine
void ultrasonic::_interrupt() {
  ultrasonic* _this=ultrasonic::instance();
  switch (digitalRead(_this->_echoPin)) {
    case HIGH:
      _this->_echoStart = micros();
      _this->_timeoutError = false;
      break;
    case LOW:
      _this->_echoEnd = micros();
      _this->_finished = true;
      digitalWrite(_this->_triggerPin, HIGH);
      detachInterrupt(_this->_echoPin);
      break;
  }
}
