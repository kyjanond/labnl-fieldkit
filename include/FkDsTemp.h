#ifndef FkDsTemp_h
#define FkDsTemp_h
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class FkDsTemp
{
  private:
    //resolution 9, delay 750
    const unsigned long DELAY = 750 / (1 << (12 - 9)); 
    OneWire ds;
    DallasTemperature sensors;
  public:
    FkDsTemp(uint8_t pin) { 
      ds = OneWire(pin);
      sensors = DallasTemperature(&ds);
    }

    float read(){
      unsigned long start = millis();
      start = millis();
      sensors.setWaitForConversion(false);  // makes it async
      sensors.requestTemperatures();
      sensors.setWaitForConversion(true);

      unsigned long dur = millis() - start;
      if (dur < DELAY) {
        delay(DELAY-dur);
      }
      return sensors.getTempCByIndex(0);
    }
};

#endif