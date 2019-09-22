/**
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

Adafruit_BME280 bme; // I2C

float temperature;
float pressure;
float humidity;
  
void setup() {
   #ifdef DEBUG
   Serial.begin(9600);
   delay(2000);
   pinMode(1, OUTPUT);   
   #endif
}

void loop() {
  #ifdef DEBUG
  Serial.println("Activando sensor bme280");
  #endif DEBUG
  digitalWrite(1,HIGH); //enciende sensor
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
  Serial.println("Desactivando sensor bme280");
  #endif DEBUG
  
  digitalWrite(1,LOW); //apago el sensor bme
  
  activate_ble_and_publish(temperature, pressure, humidity);
  BLEDevice central = BLE.central();
  while (!central) {
    central = BLE.central();
    #ifdef DEBUG
    //Serial.println("Esperando a que la central se conecte");
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
  deactivate_ble_and_sleep(); 
}

void printValues(float t, float h, float p) {
    Serial.print("Temperatura = ");
    Serial.print(t);
    Serial.println(" *C");

    Serial.print("Humedad = ");
    Serial.print(h);
    Serial.println(" %");

    Serial.print("Presion = ");
    Serial.print(p);
    Serial.println(" hPa");

    Serial.println();
}
