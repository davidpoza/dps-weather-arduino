#include <ArduinoBLE.h>
#include <ArduinoLowPower.h>
#include "config.h"

void deactivate_ble_and_sleep(){
  #ifdef DEBUG
  Serial.println("Desconectando BLE");
  #endif DEBUG  
  BLE.end();
  LowPower.sleep(1000*60*SLEEP_TIME_MIN);
}

void activate_ble_and_publish(float temperature, float pressure, float humidity) {
  #ifdef DEBUG
  Serial.println("Conectando BLE");
  #endif DEBUG
  BLEFloatCharacteristic temperatureCharacteristic(TEMP_ID, BLERead);
  BLEFloatCharacteristic pressureCharacteristic(PRESS_ID, BLERead);
  BLEFloatCharacteristic humidityCharacteristic(HUM_ID, BLERead);
  temperatureCharacteristic.writeValue(temperature);
  pressureCharacteristic.writeValue(pressure);
  humidityCharacteristic.writeValue(humidity);
  BLEService wstationService(STATION_ID);
  BLE.begin();
  BLE.setLocalName(STATION_NAME);
  BLE.setAdvertisedService(wstationService);
  wstationService.addCharacteristic(temperatureCharacteristic);
  wstationService.addCharacteristic(pressureCharacteristic);
  wstationService.addCharacteristic(humidityCharacteristic);
  BLE.addService(wstationService);
  BLE.advertise();
  #ifdef DEBUG
  Serial.println("Anunciando periférico");
  #endif DEBUG  
}
