#include <ArduinoBLE.h>
#include <ArduinoLowPower.h>
#include "config.h"

/**
 * Los procesadores SAMD21 se cuelgan cuando entran en modo sleep.
 * Esto se debe a que la cpu no espera a que la RAM se levante y el sistema colapsa
 * al tratar de acceder a la misma.
 * 
 * https://www.avrfreaks.net/forum/samd21-samd21e16b-sporadically-locks-and-does-not-wake-standby-sleep-mode
 * We have identified the issue with WDT reset. It happens due to SysTick timer.
 * Issue is that WDT initiates the wake up, but then SysTick interrupt starts to get handled first before system is actually ready. 
 * Basically, what happens is that SysTick interrupt does not wait for the RAM to properly wake up from sleep.
 * So if you wake up from WDT, the system will wait for the RAM, but the core clock will actually be running, so SysTick interrupt may happen too.
 * SysTick interrupt does not wait on the RAM, so the core attempts to run the SysTick handler and fails, since RAM is not ready.
 * This causes a Hard Fault (in our testing SRAM is so slow to wake up even Hard Fault handler).
 * This mean that device wakes up, getting into the Hard Fault, stay there until WDT fully expires.
 * You can reproduce the issue quicker by running SysTick timer faster, and WDT wake ups also quicker.
 * The solution for the customer is to disable SysTick interrupt before going to sleep and enable it back after the sleep.
 */
void deactivate_ble_and_sleep(){  
  #ifdef DEBUG
  Serial.println("Desconectando BLE y entrando en modo sleep");
  #endif DEBUG  
  BLE.end();
  delay(100);
  SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
  LowPower.sleep(1000*60*SLEEP_TIME_MIN);
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}

void activate_ble_and_publish(float temperature, float humidity, float pressure) {
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
