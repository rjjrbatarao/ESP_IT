#ifndef __BOARD_INFO_H
#define __BOARD_INFO_H

void ICACHE_FLASH_ATTR getBoardInfo() {
  StaticJsonDocument<128> info;
  char infoBuffer[128];
  info["ip"] = WiFi.localIP();
  info["id"] = ESP.getChipId();
  info["rssi"] = WiFi.RSSI();
  serializeJson(info, infoBuffer);
  Serial.printf("%s:%s\n",taskNames[2].name,infoBuffer);
}

#endif
