#ifndef utils_h
#define utils_h
#include <Arduino.h>


void printBytes(const uint8_t* addr, uint8_t count, bool newline=false) {
  for (uint8_t i = 0; i < count; i++) {
    Serial.print(addr[i]>>4, HEX);
    Serial.print(addr[i]&0x0f, HEX);
  }
  if (newline)
    Serial.println();
}

const byte COLOR_PASTEL_WATERMELON[] = {255,88,135};
const byte COLOR_AVOCADO[] = {192,177,0};

#endif