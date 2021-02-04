#ifndef _WIFI_F_H
#define _WIFI_F_H

uint8_t wifiGetLinkStatus(String arg) {
  uint8_t wifi_status;
  switch (WiFi.status()) {
    case 0:
      wifi_status = 0;
      Serial.println(arg + F(":idle-status"));
      break;
    case 1:
      wifi_status = 1;
      Serial.println(arg + F(":no-ssid-available"));
      break;
    case 3:
      wifi_status = 3;
      Serial.println(arg + F(":connected"));
      break;
    case 5:
      wifi_status = 5;
      Serial.println(arg + F(":connection-lost"));
      break;
    case 6:
      wifi_status = 6;
      Serial.println(arg + F(":connected"));
      break;
  }

  return wifi_status;
}

#endif
