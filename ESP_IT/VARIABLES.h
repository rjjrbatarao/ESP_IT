#ifndef __VAR_COM_H
#define __VAR_COM_H

int DEBUG_DEV = OFF_DEBUG;
int MACHINE_MODE = BASIC_LOGIN;
int LISTEN_PORT = DEFAULT_PORT;
int WEB_PORT = DEFAULT_PORT;
boolean WEB_MODE = false;
boolean REST_MODE = false;
boolean SSE_MODE = false;
boolean SSL = false;
boolean AXTLS = false;
int WIFI_MODE = STA_MODE;
int WIFI_PHY_MODE = G_PHY;
int PHY_B_POWER = B_DB_MAX;
int PHY_G_POWER = G_DB_MAX;
int PHY_N_POWER = N_DB_MAX;
boolean LICENSE_STATUS = false;
boolean PHY_MIN_FLAG = false;
boolean RESPONSE_DIMENSION_FLAG = false;
int TOKEN_LENGTH = 8;
boolean SMART_CONNECT = false;
boolean WIFI_IS_CONNECTED = false;
const byte DNS_PORT = 53;

byte uuidNumber[16];
char token[32];
char mikrotikUsername[32] = ""; // your mikrotik login user name
char mikrotikPassword[32] = ""; // your mikrotik log in password
char ap_ssid[32] = "";
char ap_password[32] = "";
char sta_ssid[32] = "";
char sta_password[32] = "";
uint16_t port = 0;
char fingerprint[96];
int response_count;
int current_index;
int event_count;
char masterData[128];
char serverToken[32];
boolean wifiFlag = false;
boolean webFlag = false;
boolean serverFlag = false;
boolean eventFlag = false;
boolean typeFlag = false;
char responseCustom[64];

const char* www_username = "admin";
const char* www_password = "admin";
const char *myHostname = "esp8266";

const char* _title = "MIKRODEV OTA";
const char* _banner = "MIKRODEV Firmware Updater";
const char* _build = "Build : 5.3.3b";
const char* _branch = "Branch : development";
String _deviceInfo = "";
const char* _footer = "All Rights Reserved 2020";
String _deviceIp = "192.168.1.1";
String _devicePort = "7000";
String _css = "/style.css";

struct variable_type_t {
  boolean variableType = false;
} dataType[10];

struct verify_secure_t {
  boolean sourceIp = false;
  boolean sourceToken = false;
} verifySource[5];

struct json_response_t {
  int value;
  char data[32];
  char words[32];
} response[10];

struct event_t {
  char data[32];
} parameter[5];

struct license_key_t {
  boolean licenseFlag = false;
} license_check;

struct dimensional_response_t {
  char words[17][10];
  char data[17][10];
  int value[10];
  int type[10];
  int dimension_count;
} responseDimension[5];

struct task_scheduler_t{
  char name[32];
} taskNames[5];

struct flexi_timer_t{
  char name[32];
  uint8_t interval = 1;
} flexibleTimer[5];

#endif
