/**

   Fecha: 16/11/2019. Finalmente he decidido alimentar el arduino por usb y descarto la idea de usar baterias, pues
   sigo teniendo problemas de cuelgues tras entrar en modo sleep. No ocurre siempre, pero no me da ninguna fiabilidad.
   Por lo tanto en la unidad exterior no voy a apagar el BLE en ningun momento, ya que su consumo en reposo es muy bajo.
   ------------------------------------------------
   Este código es para la unidad de exterior, que va a ser un periférico
   que usará el protocolo bluetooth BLE, para transmitir a la unidad interior
   los datos de los sensores. Gastando muy poca corriente y siendo muy rápido.

   La unidad interior, que estará conectada a la corriente será la encargada
   de ser la centra en el protocolo bluetooth BLE y a su vez de enviar por WIFI
   los datos al servidor en internet.

   De este modo optimizamos la bateria de la unidad exterior, pues el protocolo wifi
   consume mucho y es lento en comparación. (65ma vs 15ma) y un 15seg vs 0.5seg).

   Fecha: 08/09/2020
   El sensor bme280 estaba volviendose loco, tras x horas funcionando, cada vez diferente.
   He leído que este sensor se vuelve loco por i2c y la solución es resetearlo.
   Aquí explican: https://community.particle.io/t/bme280-sensor-problem/49627/11
   Asi que he modificado el wiring para alimentar el BME280 por el pin7 digital y poder resetearlo.

   Fecha: 04/10/2020
   Añadido el anemómetro

   Fecha 02/06/2020
   Modifico la unidad exterior para que suba las mediciones directamente él mismo via WIFI.
*/

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "functions.h"
#include "config.h"

volatile bool just_wakeup = true;
Adafruit_BME280 bme; // I2C

float temperature; // celsius
float pressure; // hPa
float humidity; // %
float wind; // km/h
float windMeasurements[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int mIndex = 0; // current measurement index in windMeasurements
volatile int ticks; // used by interrupt
unsigned long time;
long cycles = 0; // contador de loops para forzar un reset al cabo de x loops
String token = "";
int msBetweenWIFI = 0; //seconds between wifi connections

void setup() {
  #ifdef DEBUG
    Serial.begin(9600);
    delay(2000);
  #endif
  connectToWifi();
  token = sendAuth(API_USER, API_PASSWORD);
  disconnectWifi();
  pinMode(BME280_PIN, OUTPUT); // sensor bme280
  ticks = 0;
  time = millis();
  attachInterrupt(digitalPinToInterrupt(ANEMOMETER_DIGITAL_PIN), tickInc, FALLING);
  resetBME(&bme);
}

void loop() {
  delay(2000);
  resetBME(&bme);
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.seaLevelForAltitude(920, bme.readPressure() / 100.0F);
  detachInterrupt(digitalPinToInterrupt(ANEMOMETER_DIGITAL_PIN));
  // we dont want calculation to be interrupted
  windMeasurements[mIndex] = windLinearTransformation(ticks * 1000 / (millis() - time), millis() - time);
  attachInterrupt(digitalPinToInterrupt(ANEMOMETER_DIGITAL_PIN), tickInc, FALLING);
  msBetweenWIFI += millis() - time;

  time = millis();
  ticks = 0;
  mIndex = (mIndex + 1) % 10;
  wind = averageWindSpeed(windMeasurements);

  #ifdef DEBUG
    printValues(temperature, humidity, pressure);
  #endif DEBUG

  if(msBetweenWIFI >= 60*1000*FREQ_UPDATE_SERVER_MIN) {
    msBetweenWIFI = 0;
    logData(token, temperature, humidity, pressure, wind);
  }
  cycles++;
}

void tickInc() {
  ticks++;
}
