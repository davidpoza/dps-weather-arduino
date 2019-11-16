/**
 * 
 * Update: 16 Noviembre 2019. Finalmente he decidido alimentar el arduino por usb y descarto la idea de usar baterias, pues
 * sigo teniendo problemas de cuelgues tras entrar en modo sleep. No ocurre siempre, pero no me da ninguna fiabilidad.
 * Por lo tanto en la unidad exterior no voy a apagar el BLE en ningun momento, ya que su consumo en reposo es muy bajo.
 * ------------------------------------------------
 * Este código es para la unidad de exterior, que va a ser un periférico
 * que usará el protocolo bluetooth BLE, para transmitir a la unidad interior
 * los datos de los sensores. Gastando muy poca corriente y siendo muy rápido.
 * 
 * La unidad interior, que estará conectada a la corriente será la encargada 
 * de ser la centra en el protocolo bluetooth BLE y a su vez de enviar por WIFI
 * los datos al servidor en internet.
 * 
 * De este modo optimizamos la bateria de la unidad exterior, pues el protocolo wifi 
 * consume mucho y es lento en comparación. (65ma vs 15ma) y un 15seg vs 0.5seg).
 */

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "functions.h"
#include "config.h"

volatile bool just_wakeup = true;
Adafruit_BME280 bme; // I2C

float temperature;
float pressure;
float humidity;
BLEService wstationService(STATION_ID);
BLEFloatCharacteristic temperatureCharacteristic(TEMP_ID, BLERead);
BLEFloatCharacteristic pressureCharacteristic(PRESS_ID, BLERead);
BLEFloatCharacteristic humidityCharacteristic(HUM_ID, BLERead);

  
void setup() {
  #ifdef DEBUG
  Serial.begin(9600);
  delay(2000);
  #endif
  pinMode(1, OUTPUT);
  digitalWrite(1,HIGH); //enciendo el sensor
  activate_ble(&wstationService, &temperatureCharacteristic, &pressureCharacteristic, &humidityCharacteristic);
}

void loop() {
  #ifdef DEBUG
  Serial.println("Activando sensor bme280");
  #endif DEBUG

  while (!bme.begin()) {
    #ifdef DEBUG
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    #endif
  }
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.seaLevelForAltitude(920, bme.readPressure() / 100.0F);
  #ifdef DEBUG  
  printValues(temperature, humidity, pressure);
  #endif DEBUG
  

  publish_data(&wstationService, temperature, humidity, pressure, &temperatureCharacteristic, &pressureCharacteristic, &humidityCharacteristic);
  BLEDevice central = BLE.central();
  while (!central) {
    central = BLE.central();
    #ifdef DEBUG
    Serial.println("Esperando a que la central se conecte");
    #endif DEBUG
  }  
  
  if (central) {
    while (central.connected()) {
      central = BLE.central();
      #ifdef DEBUG
      Serial.println("Esperando a que la central lea los datos y se desconecte");
      #endif DEBUG     
    }  
  }
}
