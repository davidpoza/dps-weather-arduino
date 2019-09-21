#include "functions.h"
#include "config.h"
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP280.h"

#include <ArduinoLowPower.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_BMP280 bmp; // I2C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



unsigned long delayTime;
float temperature;
float pressure;
float humidity;
String token;

void setup() {
   Serial.begin(9600);
   connect_to_wifi();
   token = sendAuth(API_USER, API_PASSWORD);
   
   delay(4000);   
   //while(!Serial);    // time to get serial running
    Serial.println(F("BMP280 test"));
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
      Serial.println(F("SSD1306 allocation failed"));
     for(;;); // Don't proceed, loop forever
   }
   
    while (!bmp.begin()) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    }
    
    Serial.println("-- Default Test --");
    delayTime = 3000;

    Serial.println();

}

void loop() {
  connect_to_wifi();
  //temperature = bme.readTemperature();
  //humidity = bme.readHumidity();
  //pressure = bme.readPressure() / 100.0F;
  pressure = bmp.seaLevelForAltitude(920, bmp.readPressure() / 100.0F);
  printValues(temperature, humidity, pressure);
  drawData(temperature, humidity, pressure);
  //logData(token, temperature, humidity, pressure);  
  disconnect_wifi();
  LowPower.sleep(1000*60*REFRESH_TIME_MIN);
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

void drawData(float t, float h, float p) {
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
}
