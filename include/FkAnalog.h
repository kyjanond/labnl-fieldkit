#ifndef FkAnalog_h
#define FkAnalog_h
#include <Arduino.h>

#define FK_ANALOG_LIGHT_SAMPLES 10 
#define FK_ANALOG_LIGHT_SAMPLE_DELAY 10 

class FkAnalog
{
  private:
    const uint8_t analogPin;
    uint32_t buffer[FK_ANALOG_LIGHT_SAMPLES] = {};
  public:
    const uint16_t VALUE_MAX =  3300;
    FkAnalog(uint8_t pin):analogPin(pin) { 
      pinMode(analogPin, ANALOG);
    }

    uint32_t readRaw(){
      uint32_t rawValue = analogReadMilliVolts(analogPin);
      uint32_t averageRawValue = 0;
      for (size_t i = 0; i < FK_ANALOG_LIGHT_SAMPLES; i++)
      {
        buffer[i] = rawValue;
        averageRawValue += rawValue;
        delay(FK_ANALOG_LIGHT_SAMPLE_DELAY);
      }
      averageRawValue /=  FK_ANALOG_LIGHT_SAMPLES;
      return averageRawValue;
    }
};

#endif