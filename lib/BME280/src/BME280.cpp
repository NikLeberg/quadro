/*
  BME280.cpp v0.2 - Bibliothek um den Bosch Sensortec BME280 Umgebungssensor anzusprechen.
  Erstellt von Niklaus R. Leuenberger, Juni 17, 2018.
*/

#include "BME280.hpp"

// Initialisiert BME280
//
// Returncodes:
//  0 - Ok
//  1 - BME280 nicht verbunden oder antwortet nicht
//  2 - Busfehler beim Konfigurieren aufgetreten
//
uint8_t BME280::setup() {
  // Prüfe Verbindung zum BME280
  if (BME280::_readByte(_addr, BME280_CHIP_ID_ADDR) != BME280_CHIP_ID) {
    // warte kurz um Sensor bei Power-On etwas Zeit zu geben
    delay(1000);
    // Prüfe Verbindung erneut
    if (BME280::_readByte(_addr, BME280_CHIP_ID_ADDR) != BME280_CHIP_ID) {
      // immernoch keine Verbindung
      return 1;
    }
  }

  // Setze Sensor zurück
  if (BME280::_writeByte(_addr, BME280_RESET_ADDR, 0xB6)) return 2;
  // warte bis Sensor neugestartet hat und wieder erreichbar ist
  delay(10);
  uint8_t timeout = 100;
  while (BME280::_readByte(_addr, BME280_CHIP_ID_ADDR) != BME280_CHIP_ID && timeout) {
    delay(10);
    timeout--;
    if (!timeout) return 1;
  }

  // Warte bis Sensor Kalibrierungsdaten eingelesen hat
  while ((BME280::_readByte(_addr, BME280_STATUS_ADDR) & 0x01) != 0) {
    delay(10);
  }

  // Speichere Kalibrierungsdaten für Rohdatenkompensation
  if(BME280::readCalibration()) return 2;

  // Konfiguriere Sensor mit empfohlenen Einstellungen für "Gaming"
  // ! Feuchtigkeitsmessung ist deaktiviert !
  if (BME280::setConfig(BME280_NORMAL_MODE, BME280_SAMPLING_X1, BME280_SAMPLING_X4, BME280_SAMPLING_NONE, BME280_FILTER_X16, BME280_STANDBY_MS_0_5)) return 2;

  return 0;
}

// Lese Kalibrierungsdaten aus
bool BME280::readCalibration() {
  uint8_t data[26];
  if (BME280::_readBytes(_addr, BME280_TEMP_PRESS_CALIB_DATA_ADDR, data, 26)) return true;
  _calibrationData.dig_T1 = (uint16_t)(data[1] << 8 | data[0]);
  _calibrationData.dig_T2 = (int16_t)(data[3] << 8 | data[2]);
  _calibrationData.dig_T3 = (int16_t)(data[5] << 8 | data[4]);

  _calibrationData.dig_P1 = (uint16_t)(data[7] << 8 | data[6]);
  _calibrationData.dig_P2 = (int16_t)(data[9] << 8 | data[8]);
  _calibrationData.dig_P3 = (int16_t)(data[11] << 8 | data[10]);
  _calibrationData.dig_P4 = (int16_t)(data[13] << 8 | data[12]);
  _calibrationData.dig_P5 = (int16_t)(data[15] << 8 | data[14]);
  _calibrationData.dig_P6 = (int16_t)(data[17] << 8 | data[16]);
  _calibrationData.dig_P7 = (int16_t)(data[19] << 8 | data[18]);
  _calibrationData.dig_P8 = (int16_t)(data[21] << 8 | data[20]);
  _calibrationData.dig_P9 = (int16_t)(data[23] << 8 | data[22]);

  _calibrationData.dig_H1 = data[25];
  if (BME280::_readBytes(_addr, BME280_HUMIDITY_CALIB_DATA_ADDR, data, 7)) return true;
  _calibrationData.dig_H2 = (int16_t)(data[1] << 8 | data[0]);
  _calibrationData.dig_H3 = data[2];
  _calibrationData.dig_H4 = (int16_t)(data[3] << 4 | data[4] & 0x0F);
  _calibrationData.dig_H5 = (int16_t)(data[5] << 4 | data[4] >> 4);
  _calibrationData.dig_H6 = data[6];
  return false;
}

// Konfiguriere Sensor
bool BME280::setConfig(uint8_t mode, uint8_t tempSampling, uint8_t pressSampling, uint8_t humSampling, uint8_t filter, uint8_t tSleep) {
  if (BME280::_writeByte(_addr, BME280_CTRL_HUM_ADDR, humSampling)) return true;
  if (BME280::_writeByte(_addr, BME280_CTRL_MEAS_ADDR, tempSampling << 5 | pressSampling << 2 | mode)) return true;
  if (BME280::_writeByte(_addr, BME280_CONFIG_ADDR, tSleep << 5 | filter << 2 | 0)) return true;
  return false;
}

// Lese Temperatur
bool BME280::getTemperature(int32_t &temp) {
  uint8_t data[3];
  if (BME280::_readBytes(_addr, BME280_DATA_TEMP_ADDR, data, 3)) return true;
  int32_t rawTemp = (int32_t)(data[0] << 12 | data[1] << 4 | data[2] >> 4);
  if (rawTemp == 0x800000) return true; // Temperaturmessung deaktiviert
  temp = BME280::_compensateTemperature(rawTemp);
  return false;
}

// Lese Temperatur
bool BME280::getTemperature(float &temp) {
  int32_t t;
  if (BME280::getTemperature(t)) return true;
  temp = t / 100.0;
  return false;
}

// Lese Druck
bool BME280::getPressure(uint32_t &press) {
  uint8_t data[6];
  if (BME280::_readBytes(_addr, BME280_DATA_PRESS_ADDR, data, 6)) return true;
  // berechne zuerst die Temperatur
  int32_t rawTemp = (int32_t)(data[3] << 12 | data[4] << 4 | data[5] >> 4);
  if (rawTemp == 0x800000) return true; // Temperaturmessung deaktiviert
  BME280::_compensateTemperature(rawTemp);
  // berechne Druck
  int32_t rawPress = (int32_t)(data[0] << 12 | data[1] << 4 | data[2] >> 4);
  if (rawTemp == 0x800000) return true; // Druckmessung deaktiviert
  press = BME280::_compensatePressure(rawPress);
  return false;
}

// Lese Druck
bool BME280::getPressure(float &press) {
  uint32_t p;
  if (BME280::getPressure(p)) return true;
  press = p / 256.0;
  return false;
}

// Lese rel.Feuchtigkeit
bool BME280::getHumidity(uint32_t &hum) {
  uint8_t data[5];
  if (BME280::_readBytes(_addr, BME280_DATA_TEMP_ADDR, data, 5)) return true;
  // berechne zuerst die Temperatur
  int32_t rawTemp = (int32_t)(data[0] << 12 | data[1] << 4 | data[2] >> 4);
  if (rawTemp == 0x800000) return true; // Temperaturmessung deaktiviert
  BME280::_compensateTemperature(rawTemp);
  // berechne rel.Feuchtigkeit
  int32_t rawHum = (int32_t)(data[0] << 8 | data[1] << 8);
  if (rawHum == 0x8000) return true; // Druckmessung deaktiviert
  hum = BME280::_compensateHumidity(rawHum);
  return false;
}

// Lese rel.Feuchtigkeit
bool BME280::getHumidity(float &temp) {
  uint32_t h;
  if (BME280::getHumidity(h)) return true;
  temp = h / 1024.0;
  return false;
}

// Höhe über Meer (in cm)
bool BME280::getAltitude(uint32_t &alt) {
  uint32_t pressure;
  float rel;
  if (BME280::getPressure(pressure)) return true;
  rel = pressure / 25939200.0;
  alt = 4433000 * (1.0 - pow(rel, 0.1903));
  return false;
}

// Höhe über Meer (in cm)
bool BME280::getAltitude(float &alt) {
  float pressure;
  if (BME280::getPressure(pressure)) return true;
  alt = 4433000.0 * (1.0 - pow(pressure / 101325.0, 0.1903));
  return false;
}

// Kompensiere Temperatur
int32_t BME280::_compensateTemperature(int32_t &rawTemp) {
  int32_t var1, var2, temperature;
  var1 = (rawTemp >> 3) - ((int32_t)_calibrationData.dig_T1 << 1);
  var1 = (var1 * ((int32_t)_calibrationData.dig_T2 >> 11));
  var2 = (rawTemp >> 4) - ((int32_t)_calibrationData.dig_T1);
  var2 = (((var2 * var2) >> 12) * ((int32_t)_calibrationData.dig_T3)) >> 14;
  _tFine = var1 + var2;
  temperature = (_tFine * 5 + 128) >> 8;
  return temperature; // in °C, 2251 entspricht 22.51°C
}

// Kompensiere Druck
uint32_t BME280::_compensatePressure(int32_t &rawPress) {
  int64_t var1, var2, pressure;
  var1 = ((int64_t)_tFine) - 128000;
  var2 = var1 * var1 * (int64_t)_calibrationData.dig_P6;
  var2 = var2 + ((var1 * (int64_t)_calibrationData.dig_P5) << 17);
  var2 = var2 + (((int64_t)_calibrationData.dig_P4) << 35);
  var1 = ((var1 * var1 * (int64_t)_calibrationData.dig_P3) >> 8) + ((var1 * (int64_t)_calibrationData.dig_P2) << 12);
  var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)_calibrationData.dig_P1) >> 33;
  if (var1 == 0) return 0; // verhindere Division durch 0
  pressure = 1048576 - rawPress;
  pressure = (((pressure << 31) - var2) * 3125) / var1;
  var1 = (((int64_t)_calibrationData.dig_P9) * (pressure >> 13) * (pressure >> 13)) >> 25;
  var2 = (((int64_t)_calibrationData.dig_P8) * pressure) >> 19;
  pressure = ((pressure + var1 + var2) >> 8) + (((int64_t)_calibrationData.dig_P7) << 4);
  return (int32_t)pressure; // in Pa, 24674867 entspricht 24674867/256 = 96386.2 Pa = 963.862 hPa
}

// Kompensiere rel.Feuchtigkeit
uint32_t BME280::_compensateHumidity(int32_t &rawPress) {
  int32_t var1, var2, var3, var4, var5;
	uint32_t humidity;
	var1 = _tFine - ((int32_t)76800);
	var2 = (int32_t)(rawPress << 14);
	var3 = (int32_t)(((int32_t)_calibrationData.dig_H4) << 20);
	var4 = ((int32_t)_calibrationData.dig_H5) * var1;
	var5 = (((var2 - var3) - var4) + (int32_t)16384) >> 15;
	var2 = (var1 * ((int32_t)_calibrationData.dig_H6)) >> 10;
	var3 = (var1 * ((int32_t)_calibrationData.dig_H3)) >> 11;
	var4 = ((var2 * (var3 + (int32_t)32768)) >> 10) + (int32_t)2097152;
	var2 = ((var4 * ((int32_t)_calibrationData.dig_H2)) + 8192) >> 14;
	var3 = var5 * var2;
	var4 = ((var3 >> 15) * (var3 >> 15)) >> 7;
	var5 = var3 - ((var4 * ((int32_t)_calibrationData.dig_H1)) >> 4);
	var5 = (var5 < 0 ? 0 : var5);
	var5 = (var5 > 419430400 ? 419430400 : var5);
	humidity = var5 >> 12;
	return humidity; // in %rF, 47445 entspricht 47445/1024 = 46.333 %rF
}
