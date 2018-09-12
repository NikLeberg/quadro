/**
* @file    config.cpp
* @brief   Konfigurationsspeicher
*
* Liest und schreibt die Konfiguration aus einer Konfigurations-Datei.
* @author  Niklaus R. Leuenberger
* @date    22/07/2018
*/

#include "config.hpp"

/**
* @brief  Setup
*
* Setzt alles auf. Initialisiert Dateisystem im SPIFFS und liest mit
* configClass::read() die Konfiguration ein.
* @return uint8_t Fehlercode
*/
uint8_t configClass::setup() {
  if (SPIFFS.begin()) return configClass::read();
  // Dateisystem ev. unformatiert, formatiere...
  if (!SPIFFS.format()) return ERR_CF_FORMAT;
  if (SPIFFS.begin()) return configClass::read();
  // nope, Dateisystem ist defekt
  return ERR_CF_FS;
}

/**
* @brief  Lade Konfiguration aus Dateisystem
*
* Lese CONF_FILE in vars ein. Wenn Datei nicht existiert oder fehlerhaft ist,
* dann erstelle neue Standart-Datei mittels conf::write().
* @note   vars    wird als Array von ints behandelt
* @return uint8_t Fehlercode
*/
uint8_t configClass::read() {
  File file = SPIFFS.open(CONF_FILE, "r");
  if (!file || file.size() != sizeof(vars)) {
    file.close();
    return configClass::write();
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
uint8_t configClass::write() {
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
void configClass::format(uint8_t name, String var) {
  if (var.equals("")) return;
  switch (vars[name].type) {
    case 0: // Bool
    {
      if (var.equals("true") || var.equals("1")) vars[name].b = true;
      else if (var.equals("false") || var.equals("0")) vars[name].b = false;
    }
    break;

    case 1: // uint8_t
    {
      uint8_t temp = var.toInt();
      if (!isnan(temp) || temp >= 0 || temp <= 255) vars[name].ui8 = temp;
    }
    break;

    case 2: // uint16_t
    {
      uint16_t temp = var.toInt();
      if (!isnan(temp) || temp >= 0 || temp <= 65535) vars[name].ui16 = temp;
    }
    break;

    case 3: // uint32_t
    {
      uint32_t temp = var.toInt();
      if (!isnan(temp) || temp >= 0 || temp <= 4294967295) vars[name].ui32 = temp;
    }
    break;

    case 4: // int8_t
    {
      int8_t temp = var.toInt();
      if (!isnan(temp) || temp >= -128 || temp <= 127) vars[name].i8 = temp;
    }
    break;

    case 5: // int16_t
    {
      int16_t temp = var.toInt();
      if (!isnan(temp) || temp >= -32768 || temp <= 32767) vars[name].i16 = temp;
    }
    break;

    case 6: // int32_t
    {
      int16_t temp = var.toInt();
      if (!isnan(temp) || temp >= -2147483648 || temp <= 2147483647) vars[name].i32 = temp;
    }
    break;

    case 7: // float
    {
      float temp = var.toFloat();
      if (!isnan(temp)) vars[name].f = temp;
    }
    break;
  }
}

/**
* @brief  Suche Variablen
*
* Sucht Variablen in vars anhand des gegebenen Strings und gibt Pointer des
* Typs configVariable zurück. Pointer Klassen-intern für die spätere schnellere
* Verwendung abspeichern. Falls ein gültiger Index angegeben wurde, wird auf
* diese direkt zugegriffen. Ansonsten lediglich aktualisert.
* @note   Zugriff über name sehr ineffizient.
* @post   Prüfe auf NULL!
* @param  uint8_t  index  Index der Variable im Array
* @param  String  name    Name der Variable im Array
* @param  bool null       true wenn Variable nicht gefunden
* @return configVariable* Pointer für späteren schnellen Zugriff,
*                         NULL wenn nicht gefunden.
*/
configClass::configVariable* configClass::get(String name, bool &null, uint8_t *index) {
  // Direktzugriff mittels Index
  if (index != NULL && *index > 0 && *index < CONF_NUM) return &vars[*index];
  // Suche nach Name
  if (name == "") {
    null = true;
    return NULL;
  }
  uint8_t len = name.length() - 1;
  uint8_t pos = 0;
  for (uint8_t i = 0; i < CONF_NUM; i++) {
    for (uint8_t j = 0; j <= len; j++) {
      if (name.charAt(pos) != vars[i].name[j]) {
        pos = 0;
        break;
      }
      if (j == len) {
        if (index != NULL) *index = i;
        return &vars[i];
      }
      pos++;
    }
  }
  // die gesuchte Variable existiert nicht
  null = true;
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
bool configClass::del() {
  bool ret = false;
  ret = SPIFFS.remove(CONF_FILE);
  //configClass::setDefault();
  //ret += configClass::write();
  return ret;
}
