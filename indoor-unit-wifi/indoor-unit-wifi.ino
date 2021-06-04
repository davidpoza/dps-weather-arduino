/**
 * Este código se encarga de esperar a que la unidad exterior se anuncie por
 * bluetooth BLE, y entonces lee sus caracteristicas (temperatura, humedad y presión),
 * redibuja la pantalla y raliza el envío via wifi al servidor usando http,
 * En el servidor se encuentra una API REST.
 *
 * Fecha: 08/09/2020
 * El sensor bme280 estaba volviendose loco, tras x horas funcionando, cada vez diferente.
 * He leído que este sensor se vuelve loco por i2c y la solución es resetearlo.
 * Aquí explican: https://community.particle.io/t/bme280-sensor-problem/49627/11
 * Asi que he modificado el wiring para alimentar el BME280 por el pin7 digital y poder resetearlo.
 */

#include "config.h"
#include "functions.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <ArduinoBLE.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_BME280 bme; // I2C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float indoorTemperature = 0.0;
float indoorHumidity = 0.0;
float wind = 0.0;
float pressure = 0.0;
String lastLogDate = "";
int secondsBetweenWIFI = 0; //segundos entre envios wifi
long cycles = 0; // contador de loops para forzar un reset al cabo de x loops
String token = "";

void setup() {
   pinMode(BME280_PIN, OUTPUT); // para alimentar el BME280 y poder resetearlo
   Serial.begin(9600);
   delay(2000);
   connectToWifi();
   token = sendAuth(API_USER, API_PASSWORD);
   disconnectWifi();   // actualmente wifinina no puede usarse al mismo tiempo que arduinoble

   if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
     Serial.println(F("SSD1306 allocation failed"));
     for(;;); // Don't proceed, loop forever
   }
   readLocalSensors(bme, &indoorTemperature, &indoorHumidity, &pressure);
}

void loop() {
  if (cycles >= CYCLES_FOR_RESET) {
    Serial.println("Reseting...");
    NVIC_SystemReset();
  }
  if(secondsBetweenWIFI >= FREQ_UPDATE_SERVER_MIN*60) {
    readLocalSensors(bme, &indoorTemperature, &indoorHumidity, &pressure);
    secondsBetweenWIFI = 0;
    lastLogDate = logData(token, indoorTemperature, indoorHumidity, pressure, wind);
  }

  delay(1000);
  secondsBetweenWIFI++;
  cycles++;
}
