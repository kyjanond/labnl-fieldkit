#ifndef FkDHT_h
#define FkDHT_h
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

//#define FK_DHTDEBUG
#define FK_DHTTYPE DHT11

typedef struct FkDHT_response
{
  float temperature;
  float humidity;
}__attribute__((packed)) FkDHT_response_t;


class FkDHT
{
  private:
    //resolution 9, delay 750
    const uint32_t delayMS = 5000;
    DHT_Unified dht;
    unsigned long lastMillis;
    FkDHT_response_t lastResponse = {-50,-1};
  public:
    FkDHT(uint8_t pin): dht(pin, FK_DHTTYPE) { 
      dht.begin();
#ifdef FK_DHTDEBUG
      sensor_t sensor;
      dht.temperature().getSensor(&sensor);
      Serial.println(F("------------------------------------"));
      Serial.println(F("Temperature Sensor"));
      Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
      Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
      Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
      Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
      Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
      Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
      Serial.println(F("------------------------------------"));
      // Print humidity sensor details.
      dht.humidity().getSensor(&sensor);
      Serial.println(F("Humidity Sensor"));
      Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
      Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
      Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
      Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
      Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
      Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
      Serial.println(F("------------------------------------"));
#endif
    }

    FkDHT_response_t read(){
      unsigned long currentMillis = millis();
      if(currentMillis-lastMillis<delayMS){
#ifdef FK_DHTDEBUG
        Serial.println("Returning last response.");
#endif
        return lastResponse;
      }
      lastMillis = currentMillis;
      sensors_event_t event;
      float temperature = -50;
      float humidity = -1;
      dht.temperature().getEvent(&event);
      if (!isnan(event.temperature)) {
        temperature = event.temperature;
      } else {
#ifdef FK_DHTDEBUG
        Serial.println("Temperature is NaN");
#endif
      }
      

      dht.humidity().getEvent(&event);
      if (!isnan(event.relative_humidity)) {
        humidity = event.relative_humidity;
      } else {
#ifdef FK_DHTDEBUG
        Serial.println("Humidity is NaN");
#endif
      }
      lastResponse = {
        temperature,
        humidity
      };
      return lastResponse;
    }
};

#endif