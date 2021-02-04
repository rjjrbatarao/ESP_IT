#ifndef __EEPROM_B_H
#define __EEPROM_B_H
void eeprom_init() {
  EEPROM.begin(322);
}
//void eeprom_put(uint8_t addr, char* eepromData) {
//  EEPROM.put(addr, eepromData);
//  delay(10);
//  EEPROM.commit();
//  EEPROM.end();
//}

void eeprom_put_data(uint8_t addr, char* eepromData){
  uint8_t ind = 0;
  while(ind != strlen(eepromData)){
    EEPROM.write((int)(addr+ind), eepromData[ind]);
    ind++;
  }
  EEPROM.write((int)(addr+ind), '\0');
  EEPROM.commit();
}

char* eeprom_get_data(uint8_t addr) {
  char c;
  char eeprom256[256] = "";
  uint8_t ind = 0;
  while(c != 0xFF){
    c = EEPROM.read((int)(addr + ind));
    eeprom256[ind] = c;
    ind++;
  }
  return (char*)eeprom256;
}

//void eeprom_put(uint8_t addr, char* eepromData){
//  uint8_t ind;
//  while(ind != strlen(eepromData)){
//    EEPROM.write((int)(addr+ind), eepromData[ind]);
//    ind++;
//  }
//  EEPROM.commit();
//}
//char* eeprom_get_license(uint8_t addr){
//  char c;
//  char eeprom64[65] = "";
//  uint8_t ind = 0;
//  while(ind != 64){
//    c = EEPROM.read((int)(addr+ind));
//    eeprom64[ind] = c;
//    ind++;
//  }
//  eeprom64[ind] = '\0';
//  //Serial.println(eeprom256);
//  return (char*)eeprom64;
//}


#endif
