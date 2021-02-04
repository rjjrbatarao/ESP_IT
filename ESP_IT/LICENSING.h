#ifndef __LICENSING_H
#define __LICENSING_H

#include <EspCrypto.h>

bool ICACHE_FLASH_ATTR licenseDecrypt(char * licenseIn) {
  char chipId[32];
  byte key[KEY_LENGTH];
  progmemBuffer(2, 2, key);
  SHA256HMAC hmac(key, KEY_LENGTH);

  itoa (ESP.getChipId(), chipId, 16);
  hmac.doUpdate(chipId, strlen(chipId));
  byte authCode[SHA256HMAC_SIZE];
  char license[SHA256HMAC_SIZE * 2];
  hmac.doFinal(authCode);
  LICENSE_STATUS = true;
  for (byte i = 0; i < SHA256HMAC_SIZE; i++)
  {
    if (authCode[i] < 0x10) {
      // Serial.print('0');
    }
    sprintf(&license[i * 2], "%02X", authCode[i]);
    if (licenseIn[i] != license[i]) {
      LICENSE_STATUS = false;
      break;
    }
  }

  if (LICENSE_STATUS == true) {
    //Serial.println(F("[INIT] License: valid"));
    return true;
  } else {
    //Serial.println(F("[ERR] License: invalid"));
    return false;
  }
}

void ICACHE_FLASH_ATTR licenseCheck() {
  char licenseBufferIn[65];
  sprintf(licenseBufferIn, "%s", eeprom_get_data(0));
  if (licenseBufferIn[0] != 0 && strlen(licenseBufferIn) == 64) {
#ifdef showLicenseKey
    Serial.print(F("[INIT] License Key: "));
    Serial.println(F(""));
#endif

#ifdef showLicenseKey
    Serial.println(licenseBufferIn);
#endif

    if (licenseDecrypt(licenseBufferIn)) {
#ifdef showLicenseKey
      Serial.print(F("[INIT] License Status: Activated"));
      Serial.println(F(""));
#endif
      LICENSE_STATUS = true;
    } else {
      Serial.println(F("[ERR] Contact Service"));

    }
  } else {
    //Serial.println(F("[ERR] Could not open file "));
    return;
  }

}

void ICACHE_FLASH_ATTR licenseWrite(char* license) {
  Serial.print(F("Adding license... "));
  Serial.println(F("Writing license to device"));
  if (strlen(license) == 64) {
    Serial.println(license);
    eeprom_put_data(0, license);
    Serial.println(F("DONE"));
  } else {
    Serial.println(F("ERROR"));
  }
}

void ICACHE_FLASH_ATTR licensing_init() {
  // spiffs_init();
  licenseCheck();
  Serial.println("READY");
  if (!LICENSE_STATUS) {
    Serial.print("ID: ");
    Serial.println(ESP.getChipId());
  }
}

#endif
