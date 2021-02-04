/*
Todo make firmware update a DONE respose as standard serial parse response
*/
// finger 1D:5A:51:E6:EE:19:DE:67:50:2C:BA:87:1A:FF:E0:30:5F:74:14:AB

// Inlcude Library

extern "C" {
#include "c_types.h"
}

#include <SimpleCLI.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
//#include <DNSServer.h>
//#include <ESP8266mDNS.h>
#include <WiFiClientSecure.h>
#include <EEPROM.h>

#include <ArduinoJson.h>
#include <mkapiwifi.h>
#include <aREST.h>
#include <FS.h>
#include "HEADER_DEFINES.h"
#include "VARIABLES.h"
#include "WIFI_FUNCTIONS.h"
#include "TASK_SCHEDULER.h"
#include "ESP8266TrueRandom.h"
#include "PROGMEM_BUFFER.h"
#include "EEPROM.h"
#include "LICENSING.h"
#include "BOARD_INFO.h"
#include "REST_API.h":
#include "UPDATER.h"
#include "WEB_SERVER.h"
#include "CLI_COMMANDS.h"
#include "SS_EVENTS.h"

void setup() {
  eeprom_init();
  cli_init();
  licensing_init();
}

void loop() {
  cli_loop();
  if (REST_MODE) {
    rest_loop();
  }
  if (WEB_MODE) {
    web_loop();
    ota_loop();
  }
  if(SSE_MODE){
    
  }
  ts.execute();
}
