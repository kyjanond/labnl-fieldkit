#include <Arduino.h>
#include <ArduinoBLE.h>
#include "FkDsTemp.h"
#include "FkDHT.h"
#include "FkAnalog.h"
#include "utils.h"

#define DEBUG

//2575734d-9d0a-4bdd-a05a-fdbc29031fd3
#define BLE_UUID(val) ("2575734d-" val "-4bdd-a05a-fdbc29031fd3")
#define TRANSMIT_DELAY 1000
#define BODY_COLOR COLOR_PASTEL_WATERMELON

BLEService waterTemperatureService (BLE_UUID("0000"));
BLEShortCharacteristic waterTemperatureCharacteristic("2A6E", BLERead | BLENotify);

BLEService airTemperatureService (BLE_UUID("0001"));
BLEShortCharacteristic airTemperatureCharacteristic("2A6E", BLERead | BLENotify);

BLEService airHumidityService (BLE_UUID("0002"));
BLEShortCharacteristic airHumidityCharacteristic("2A6F", BLERead | BLENotify);

BLEService luminousIntensityService (BLE_UUID("0003"));
BLEUnsignedShortCharacteristic luminousIntensityCharacteristic("2B01", BLERead | BLENotify);

BLEService commandService (BLE_UUID("0004"));
BLEByteCharacteristic commandCharacteristic(BLE_UUID("0104"), BLEWrite | BLERead);

FkDsTemp ds(GPIO_NUM_15);
FkDHT dht(GPIO_NUM_14); //GPIO_NUM_16
FkAnalog analogSensor(GPIO_NUM_4);

//lumen
const uint16_t LUMEN_LOW_MV = 0;
const uint16_t LUMEN_HIGH_MV = 4095;

String name = "MoBio_FieldKit-";
unsigned long previousMillis;
int16_t waterTempOld = INT16_MIN;
int16_t airTempOld = INT16_MIN;
int16_t airRHOld = INT16_MIN;
int16_t lumenOld = INT16_MIN;


void resetCharacteristics(){
  waterTemperatureCharacteristic.writeValue(INT16_MIN);
  airTemperatureCharacteristic.writeValue(INT16_MIN);
  airHumidityCharacteristic.writeValue(INT16_MIN);
  luminousIntensityCharacteristic.writeValue(INT16_MIN);
}

void resetValues(){
  waterTempOld = INT16_MIN;
  airTempOld = INT16_MIN;
  airRHOld = INT16_MIN;
  lumenOld = INT16_MIN;
}

void blinkLED(){
  digitalWrite(LED_BUILTIN, LOW);
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH);
}

void readWrittenCharacteristics() {
  // command
  if (commandCharacteristic.written()) {
#ifdef DEBUG 
    Serial.print("Command received: ");
    Serial.println(commandCharacteristic.value());
#endif
    if (commandCharacteristic.value() == 1) {  // blink cmd
      blinkLED(); // will turn the LED on
    }
  }
}

void updateSubscribedCharacteristics() {
  // water temp
  float floatWaterTemp = ds.read();
  int16_t waterTemp = (int16_t)(floatWaterTemp*100);
  if (waterTemp != waterTempOld) { 
#ifdef DEBUG 
    Serial.print("Water temperature: ");
    Serial.println(waterTemp);
#endif
    waterTemperatureCharacteristic.writeValue(waterTemp);  
    waterTempOld = waterTemp; 
  }
  // dht
  FkDHT_response_t dhtResponse = dht.read();

  int16_t airTemp = (int16_t)(dhtResponse.temperature*100);
  if (airTemp != airTempOld) { 
#ifdef DEBUG 
    Serial.print("Air temperature: ");
    Serial.println(airTemp);
#endif
    airTemperatureCharacteristic.writeValue(airTemp);  
    airTempOld = airTemp; 
  }

  int16_t airRH = (int16_t)(dhtResponse.humidity*100);
  if (airRH != airRHOld) { 
#ifdef DEBUG 
    Serial.print("Air RH: ");
    Serial.println(airRH);
#endif
    airHumidityCharacteristic.writeValue(airRH);  
    airRHOld = airRH; 
  }

  // luminous intensity
  uint32_t mVRaw = analogSensor.readRaw();
  uint16_t lumen = (uint16_t)map(
    analogSensor.VALUE_MAX - mVRaw, 
    0, 
    analogSensor.VALUE_MAX, 
    LUMEN_LOW_MV, 
    LUMEN_HIGH_MV
  );
  if (lumen != lumenOld) { 
#ifdef DEBUG 
    Serial.print("Luminous intensity: ");
    Serial.println(lumen);
#endif
    luminousIntensityCharacteristic.writeValue(lumen);  
    lumenOld = lumen; 
  }
}

void setup(){
#ifdef DEBUG
    Serial.begin(9600);
    while (!Serial){
      sleep(1);
    }
    Serial.println("Begin");
#endif
  // begin initialization
  if (!BLE.begin()) {
#ifdef DEBUG
    Serial.println("starting BLE failed!");
#endif
    while (1);
  }
  
  // name+=(char)BODY_COLOR[0];
  // name+=(char)BODY_COLOR[1];
  // name+=(char)BODY_COLOR[2];

  String address = BLE.address();
  address.toUpperCase();
  name += address[address.length() - 5];
  name += address[address.length() - 4];
  name += address[address.length() - 2];
  name += address[address.length() - 1];
#ifdef DEBUG
  Serial.print("Device full name: ");
  Serial.print(name);
  Serial.print(", address = ");
  Serial.println(address);
#endif
  BLE.setLocalName(name.c_str());
  BLE.setDeviceName(name.c_str());

  // water temperature
  BLE.setAdvertisedService(waterTemperatureService);
  waterTemperatureService.addCharacteristic(waterTemperatureCharacteristic);
  BLE.addService(waterTemperatureService);

  //air temperature
  BLE.setAdvertisedService(airTemperatureService);
  airTemperatureService.addCharacteristic(airTemperatureCharacteristic);
  BLE.addService(airTemperatureService);

  //air humidity
  BLE.setAdvertisedService(airHumidityService);
  airHumidityService.addCharacteristic(airHumidityCharacteristic);
  BLE.addService(airHumidityService);

  //luminous intensity
  BLE.setAdvertisedService(luminousIntensityService);
  luminousIntensityService.addCharacteristic(luminousIntensityCharacteristic);
  BLE.addService(luminousIntensityService);

  //command 
  BLE.setAdvertisedService(commandService);
  commandService.addCharacteristic(commandCharacteristic);
  BLE.addService(commandService);

  resetCharacteristics();


  BLE.setAppearance(0x015);
  BLE.advertise();

  pinMode(LED_BUILTIN, OUTPUT); // pin 2

#ifdef DEBUG
  Serial.println("Bluetooth® device active, waiting for connections...");
#endif
}

void loop(){
  BLEDevice central = BLE.central();

  if (central) {
#ifdef DEBUG
    Serial.print("Connected to central: ");
    Serial.println(central.address());
#endif
    // turn on the LED to indicate the connection:
    digitalWrite(LED_BUILTIN, HIGH);
    while (central.connected()) {
      unsigned long currentMillis = millis();
      readWrittenCharacteristics();
      // if 200ms have passed, check the battery level:
      if (currentMillis - previousMillis >= TRANSMIT_DELAY) {
        previousMillis = currentMillis;
        updateSubscribedCharacteristics();
      }
    }
    // when the central disconnects, turn off the LED:
    digitalWrite(LED_BUILTIN, LOW);
    resetValues();
#ifdef DEBUG
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
#endif
  }
}