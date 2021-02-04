#ifndef __PROGMM_BUFFER_H
#define __PROGMM_BUFFER_H
#include "unishox1_progmem.h"

#ifndef __progmem_uni_UNISHOX1_COMPRESSED__
#define __progmem_uni_UNISHOX1_COMPRESSED__
const byte progmem_uni_0[] PROGMEM = {21, 60, 30, 71, 44, 247, 207, 81, 26, 207, 217, 244, 118, 29, 249, 198, 118, 142, 252, 254, 17, 89};
const byte progmem_uni_1[] PROGMEM = {20, 61, 204, 255, 141, 163, 78, 135, 168, 230, 17, 10, 57, 68, 65, 31, 195, 188, 65, 29, 134};
const byte * const progmem_uni[] PROGMEM = {progmem_uni_0, progmem_uni_1};
#define progmem_uni_line_count 2
#define progmem_uni_max_len 15
#endif


void ICACHE_FLASH_ATTR progmemBuffer(uint8_t line, uint8_t address, byte (& key)[16]) {
  char unishoxBuffer[progmem_uni_max_len * 2];
  int len = unishox1_pgm_decompress(progmem_uni, line - 1, unishoxBuffer, 0);
  unishoxBuffer[len] = 0;
  if(address == 1){
    memcpy ( key, unishoxBuffer, KEY_API_LENGTH );
  } else {
    memcpy ( key, unishoxBuffer, KEY_LICENSE_LENGTH );
  }
  //Serial.println(unishoxBuffer);
  //Serial.write(unishoxBuffer);
  //Serial.write("\n\n");
  //Serial.println(key);
}

#endif
