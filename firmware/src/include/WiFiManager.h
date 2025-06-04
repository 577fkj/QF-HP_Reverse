#ifndef _wifimanager_H_
#define _wifimanager_H_

#include <Arduino.h>
#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "EC11.h"
#include "eeprom_flash.h"
#include "MIOT.h"

class WiFiManager
{

public:
  bool startConfigPortal(const char *apName);

  //String blinker_id = "";
  String _ssid = "";
  String _pass = "";

  bool back_flg = 0;
  bool wifima_flg = 0;
};

extern WiFiManager wifima;

#endif
