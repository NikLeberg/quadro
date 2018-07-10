/**
* @file    config.cpp
* @brief   Konfigurationsspeicher
*
* Liest und schreibt die Konfiguration aus einer Konfigurations-Datei.
* @author  Niklaus R. Leuenberger
* @date    10/07/2018
*/

#include "config.hpp"

/**
* @brief Konstruktor
*/
config::config() {
  // Nichts
}

/**
* @brief  Setup
*
* Setzt alles auf. Initialisiert Dateisystem im SPIFFS und liest mit
* config::read() die Konfiguration ein.
* @return uint8_t Fehlercode
*/
uint8_t config::setup() {
  if (!SPIFFS.begin()) {
    return ERR_CF_FS;
  }
  return config::read();
}

/**
* @brief  Lade Konfiguration aus Dateisystem
*
* Lese CONF_FILE in vars ein. Wenn Datei nicht existiert oder fehlerhaft ist,
* dann erstelle neue Standart-Datei mittels conf::write().
* @note   vars    wird als Array von ints behandelt
* @return uint8_t Fehlercode
*/
uint8_t config::read() {
  File file = SPIFFS.open(CONF_FILE, "r");
  if (!file || file.size() != sizeof(vars)) {
    file.close();
    return config::write();
  }
  file.read((uint8_t *)&vars, sizeof(vars));
  file.close();
  return ERR_NO_ERR;
}

/**
* @brief  Speichere Konfiguration in Datei
*
* Schreibe vars in CONF_FILE. Wenn Datei vorher nicht existierte wird diese
* erstellt.
* @note   vars    wird als Array von ints behandelt
* @return uint8_t Fehlercode
*/
uint8_t config::write() {
  File file = SPIFFS.open(CONF_FILE, "w");
  if (!file) {
    file.close();
    return ERR_CF_FILE;
  }
  file.write((uint8_t *)&vars, sizeof(vars));
  file.close();
  return ERR_NO_ERR;
}

/**
* @brief  Formatiere Variablenname
*
* Formatiere Werte von String um in den Typ und Range der im type der Variable
* hinterlegt ist und aktualisiere Wert. Wenn der Range verletzt wird setze das
* nächste Maximum.
* @param  uint8_t name  Id der Varable im Array
* @param  String  var   Neuer Wert
* @return void
*/
void config::format(uint8_t name, String var) {
  if (var.equals("")) return;
  switch (config::vars[name].type) {
    case 0: // Bool
    {
      if (var.equals("true") || var.equals("1")) config::vars[name].b = true;
      else if (var.equals("false") || var.equals("0")) config::vars[name].b = false;
    }
    break;

    case 1: // uint8_t
    {
      uint8_t temp = var.toInt();
      if (!isnan(temp) || temp >= 0 || temp <= 255) config::vars[name].ui8 = temp;
    }
    break;

    case 2: // uint16_t
    {
      uint16_t temp = var.toInt();
      if (!isnan(temp) || temp >= 0 || temp <= 65535) config::vars[name].ui16 = temp;
    }
    break;

    case 3: // uint32_t
    {
      uint32_t temp = var.toInt();
      if (!isnan(temp) || temp >= 0 || temp <= 4294967295) config::vars[name].ui32 = temp;
    }
    break;

    case 4: // int8_t
    {
      int8_t temp = var.toInt();
      if (!isnan(temp) || temp >= -128 || temp <= 127) config::vars[name].i8 = temp;
    }
    break;

    case 5: // int16_t
    {
      int16_t temp = var.toInt();
      if (!isnan(temp) || temp >= -32768 || temp <= 32767) config::vars[name].i16 = temp;
    }
    break;

    case 6: // int32_t
    {
      int16_t temp = var.toInt();
      if (!isnan(temp) || temp >= -2147483648 || temp <= 2147483647) config::vars[name].i32 = temp;
    }
    break;

    case 7: // float
    {
      float temp = var.toFloat();
      if (!isnan(temp)) config::vars[name].f = temp;
    }
    break;
  }
}

/**
* @brief  Suche Variablen
*
* Sucht Variablen in vars anhand des gegebenen Strings und gibt Pointer des
* Typs configVariable zurück. Pointer Klassen-intern für die spätere schnellere
* Verwendung abspeichern.
* @note   Höchst ineffizient, nicht innerhalb schneller Loops oder zeitkritischer
*         Funktionen zu empfehlen.
* @post   Prüfe auf NULL!
* @param  String          Name der Variable im Array
* @return configVariable* Pointer für späteren schnellen Zugriff,
*                         NULL wenn nicht gefunden.
*/
config::configVariable* config::get(String name) {
  uint8_t len = name.length() - 1;
  uint8_t pos = 0;
  for (uint8_t i = 0; i < CONF_NUM; i++) {
    for (uint8_t j = 0; j <= len; j++) {
      if (name.charAt(pos) != vars[i].name[j]) {
        pos = 0;
        break;
      }
      if (j == len) return &vars[i];
      pos++;
    }
  }
  return NULL;
}

/**
* @brief  Lösche Konfiguration
*
* Löscht die Konfigurations-Datei vom Dateisystem.
* @post   Höhere Instanz muss danach die Instanz dieser Klasse löschen und eine
*         neue erstellen damit die Standart-Werte wieder gesetzt sind.
* @return bool true bei Fehler, false bei Erfolg
*/
bool config::del() {
  bool ret = false;
  ret = SPIFFS.remove(CONF_FILE);
  //config::setDefault();
  //ret += config::write();
  return ret;
}
