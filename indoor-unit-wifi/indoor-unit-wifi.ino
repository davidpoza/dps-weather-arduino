/**
 * Este código se encarga de esperar a que la unidad exterior se anuncie por
 * bluetooth BLE, y entonces lee sus caracteristicas (temperatura, humedad y presión),
 * redibuja la pantalla y raliza el envío via wifi al servidor usando http,
 * En el servidor se encuentra una API REST.
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
float outdoorTemperature = 0.0;
float outdoorHumidity = 0.0;
float pressure = 0.0;
String lastLogDate = "";
int secondsBetweenBLE = 0; //segundos entre lecturas de outdoor via BLE
int secondsBetweenWIFI = 0; //segundos entre envios wifi
String token = "";

void setup() {
   Serial.begin(9600);
   delay(2000);
   connectToWifi();
   token = sendAuth(API_USER, API_PASSWORD);
   disconnectWifi();   //actualmente wifinina no puede usarse al mismo tiempo que arduinoble
   connectBle();

   if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
     Serial.println(F("SSD1306 allocation failed"));
     for(;;); // Don't proceed, loop forever
   }
   unsigned status = bme.begin();
   if (!status) {
     Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
     Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
     Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
     Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
     Serial.print("        ID of 0x60 represents a BME 280.\n");
     Serial.print("        ID of 0x61 represents a BME 680.\n");
     while (1);
   }
   BLE.scanForUuid(BLE_OUTDOOR_STATION_ID);
   readLocalSensors(bme, &indoorTemperature, &indoorHumidity, &pressure);
}

void loop() {    
  if(secondsBetweenBLE == OUTDOOR_REFRESH_TIME_SEC) {
    Serial.println("Buscando unidad de exterior... ");
    BLE.scanForUuid(BLE_OUTDOOR_STATION_ID);
    BLEDevice peripheral = BLE.available();
    if (peripheral) {
      Serial.println("Descubierta unidad");
      if (peripheral.hasLocalName()) {
        Serial.print("Local Name: ");
        Serial.println(peripheral.localName());
      }
      
      readLocalSensors(bme, &indoorTemperature, &indoorHumidity, &pressure);
      readRemoteSensors(peripheral, &outdoorTemperature, &outdoorHumidity);
      secondsBetweenBLE = 0;           
    }
  }
  
  if(secondsBetweenWIFI == FREQ_UPDATE_SERVER_MIN*60) {
    Serial.print("ENTRA ");
    disconnectBle();
    lastLogDate = logData(token, indoorTemperature, indoorHumidity, outdoorTemperature, outdoorHumidity, pressure);
    secondsBetweenWIFI = 0;
    connectBle();
  } 
  
  delay(1000);
  drawData(indoorTemperature, indoorHumidity, pressure, outdoorTemperature, outdoorHumidity, lastLogDate, secondsBetweenWIFI);
  
  secondsBetweenBLE++;
  secondsBetweenWIFI++;
}



void drawData(float t, float h, float p, float te, float he, String lastDate, int seconds) {
  Serial.println("Actualizando pantalla....");
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner

  display.print(F("T: "));
  display.print(t);
  display.print(" | ");
  display.print(te);
  display.println(F(" *C"));

  display.print(F("H: "));
  display.print(h);
  display.print(" | ");
  display.print(he);
  display.println(F(" %"));

  display.print(F("P: "));
  display.print(p);
  display.println(F(" hPa"));

  display.print(seconds);
  display.print(" | ");
  display.println(lastDate);
  display.display();
}

void readRemoteSensors(BLEDevice peripheral, float *temp, float *humidity){
    BLE.stopScan(); //si no paro el escaneo no puedo conectar
    // connect to the peripheral
    Serial.println("Conectando ...");

    if (peripheral.connect()) {
      Serial.println("Conexión realizada con unidad externa");
    } else {
    Serial.println("Error al conectar con unidad externa!");
      return;
    }

    // IMPORTANTE: hay que hacer un discover antes de poder obtener las caracteristicas
    if (peripheral.discoverAttributes()) {
      Serial.println("Attributes discovered");
    } else {
      Serial.println("Attribute discovery failed!");
      peripheral.disconnect();
      return;
    }

    BLECharacteristic temperatureCharacteristic = peripheral.characteristic(TEMP_ID);
    BLECharacteristic humidityCharacteristic = peripheral.characteristic(HUM_ID);

    uint8_t tmp[4];


    temperatureCharacteristic.readValue(tmp, 4);
    *temp = *(float*)&tmp;

    humidityCharacteristic.readValue(tmp, 4);
    *humidity = *(float*)&tmp;

    printValues(*temp, *humidity, 0);

    /* tenemos que desconectar para que la unidad exterior
     *  salga del bucle de espera a que leamos
     */
    peripheral.disconnect(); 
}
