/*
  BME280.cpp v0.2 - Bibliothek um den Bosch Sensortec BME280 Umgebungssensor anzusprechen.
  Erstellt von Niklaus R. Leuenberger, Juni 17, 2018.
*/

#ifndef BME280_h
#define BME280_h

#include "Arduino.h"
#include <Wire.h>
#include <functional>

/* REGISTER ADDRESS DEFINITION */
#define BME280_TEMP_PRESS_CALIB_DATA_ADDR	  (0x88) // bis 0xA1
#define BME280_CHIP_ID_ADDR					        (0xD0)
#define BME280_RESET_ADDR					          (0xE0)
#define BME280_HUMIDITY_CALIB_DATA_ADDR		  (0xE1) // bis 0xE7
#define BME280_CTRL_HUM_ADDR				        (0xF2)
#define BME280_STATUS_ADDR              	  (0xF3)
#define BME280_CTRL_MEAS_ADDR				        (0xF4)
#define BME280_CONFIG_ADDR					        (0xF5)
#define BME280_DATA_ADDR					          (0xF7)
#define BME280_DATA_PRESS_ADDR	            (0xF7)
#define BME280_DATA_TEMP_ADDR 	            (0xFA)
#define BME280_DATA_HUM_ADDR 	              (0xFD)

/* Sensor Betriebsmodi */
#define	BME280_SLEEP_MODE		                (B00)
#define	BME280_FORCED_MODE		              (B01)
#define	BME280_NORMAL_MODE		              (B11)

/* Sensor Oversampling-Rate */
#define	BME280_SAMPLING_NONE                (B000)
#define	BME280_SAMPLING_X1                  (B001)
#define	BME280_SAMPLING_X2                  (B010)
#define	BME280_SAMPLING_X4                  (B011)
#define	BME280_SAMPLING_X8                  (B100)
#define	BME280_SAMPLING_X16                 (B101)

/* Sensor Filer-Rate */
#define BME280_FILTER_OFF                   (B000)
#define BME280_FILTER_X2                    (B001)
#define BME280_FILTER_X4                    (B010)
#define BME280_FILTER_X8                    (B011)
#define BME280_FILTER_X16                   (B100)

/* Sensor Standby-Länge */
#define BME280_STANDBY_MS_0_5               (B000)
#define BME280_STANDBY_MS_10                (B110)
#define BME280_STANDBY_MS_20                (B111)
#define BME280_STANDBY_MS_62_5              (B001)
#define BME280_STANDBY_MS_125               (B010)
#define BME280_STANDBY_MS_250               (B011)
#define BME280_STANDBY_MS_500               (B100)
#define BME280_STANDBY_MS_1000              (B101)

/* Sensor-ID */
#define BME280_CHIP_ID                      (0x60)

class BME280
{
  public:
    BME280(uint8_t addr,
      std::function<uint8_t(uint8_t, uint8_t)> readByte,
      std::function<bool(uint8_t, uint8_t, uint8_t*, uint8_t)> readBytes,
      std::function<bool(uint8_t, uint8_t, uint8_t)> writeByte) :
        _addr(addr),
        _readByte(readByte),
        _readBytes(readBytes),
        _writeByte(writeByte) {};

    uint8_t setup();
    bool readCalibration();
    bool setConfig(uint8_t mode, uint8_t tempSampling, uint8_t pressSampling, uint8_t humSampling, uint8_t filter, uint8_t tSleep);
    bool getTemperature(int32_t &temp); // Funktionen mit integer sind schneller (20-50%)
    bool getTemperature(float &temp);
    bool getPressure(uint32_t &press);
    bool getPressure(float &press);
    bool getHumidity(uint32_t &hum);
    bool getHumidity(float &hum);
    bool getAltitude(uint32_t &alt);
    bool getAltitude(float &alt);
  private:
    uint8_t _addr;
    std::function<uint8_t(uint8_t, uint8_t)> _readByte;
    std::function<bool(uint8_t, uint8_t, uint8_t*, uint8_t)> _readBytes;
    std::function<bool(uint8_t, uint8_t, uint8_t)> _writeByte;
    int32_t _compensateTemperature(int32_t &rawTemp);
    uint32_t _compensatePressure(int32_t &rawPress);
    uint32_t _compensateHumidity(int32_t &rawPress);
    struct {
      uint16_t dig_T1;
      int16_t  dig_T2;
      int16_t  dig_T3;

      uint16_t dig_P1;
      int16_t  dig_P2;
      int16_t  dig_P3;
      int16_t  dig_P4;
      int16_t  dig_P5;
      int16_t  dig_P6;
      int16_t  dig_P7;
      int16_t  dig_P8;
      int16_t  dig_P9;

      uint8_t  dig_H1;
      int16_t  dig_H2;
      uint8_t  dig_H3;
      int16_t  dig_H4;
      int16_t  dig_H5;
      int8_t   dig_H6;
    } _calibrationData;
    uint32_t _tFine;
};

#endif
