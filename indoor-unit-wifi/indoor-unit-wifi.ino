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

//Adafruit_BME280 bme; // I2C
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float temperature;
float pressure;
float humidity;
String token;

void setup() {
   Serial.begin(9600);
   while (!BLE.begin()) {
     Serial.println("starting BLE failed!");
   }
   //connect_to_wifi();
   //token = sendAuth(API_USER, API_PASSWORD);   
   delay(4000);   
   
   /*if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
     Serial.println(F("SSD1306 allocation failed"));
     for(;;); // Don't proceed, loop forever
   }*/
   /*unsigned status = bme.begin();  
   if (!status) {
      Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
      Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
      Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
      Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
      Serial.print("        ID of 0x60 represents a BME 280.\n");
      Serial.print("        ID of 0x61 represents a BME 680.\n");
      while (1);
    }*/
   BLE.scanForUuid(OUTDOOR_STATION_ID);
}

void loop() {  
  BLEDevice peripheral = BLE.available();
  if (peripheral) {
    Serial.println("Descubierta unidad");
    if (peripheral.hasLocalName()) {
      Serial.print("Local Name: ");
      Serial.println(peripheral.localName());      
    }
    readSensors(peripheral);    
  }
  //temperature = bme.readTemperature();
  //humidity = bme.readHumidity();
  //pressure = bme.seaLevelForAltitude(920, bme.readPressure() / 100.0F);
  //printValues(temperature, humidity, pressure);
  //waitForSensor();
  //drawData(temperature, humidity, pressure);
  //logData(token, temperature, humidity, pressure);  
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

/*void drawData(float t, float h, float p) {
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  
  display.print(F("Temperatura: "));
  display.print(t);
  display.println(F(" *C"));
  
  display.print(F("Humedad: "));
  display.print(h);
  display.println(F(" %"));

  display.print(F("Presion: "));
  display.print(p);
  display.println(F(" hPa"));
  
  display.display();
}*/

void readSensors(BLEDevice peripheral){
    BLE.stopScan(); //si no paro el escaneo no puedo conectar
    // connect to the peripheral
    Serial.println("Conectando ...");
    
    if (peripheral.connect()) {
      Serial.println("Conexión realizada con unidad externa");
    } else {
    Serial.println("Error al conectar con unidad externa!");
      return;
    }
    BLECharacteristic temperatureCharacteristic = peripheral.characteristic(TEMP_ID);
    BLECharacteristic humidityCharacteristic = peripheral.characteristic(HUM_ID);
    BLECharacteristic pressureCharacteristic = peripheral.characteristic(PRESS_ID);
    float temp;
    float humidity;
    float pressure;
    byte tmp_value;
  
    temperatureCharacteristic.readValue(tmp_value); //aquí me lee 0
    Serial.print("temp en byte ...");
    Serial.println(tmp_value);
    temp = (float) tmp_value;
    humidityCharacteristic.readValue(tmp_value);
    humidity = (float) tmp_value;
    pressureCharacteristic.readValue(tmp_value);
    pressure = (float) tmp_value;
    
    printValues(temp, humidity, pressure);
    peripheral.disconnect();
    BLE.scanForUuid(OUTDOOR_STATION_ID);
}
