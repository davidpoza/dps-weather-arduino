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
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



unsigned long delayTime;
float temperature;
float pressure;
float humidity;
String token;

void setup() {
   Serial.begin(9600);
   //token = sendAuth(API_USER, API_PASSWORD);
   token = "eyJhbGciOiJIUzI1NiJ9.eyJzdWIiOiI1ZDgwZmUyMDk0NDEyMTAwMTNiNjQ0NzMiLCJleHAiOjE1NjkwODc4NDEwNTgsImVtYWlsIjoicG96YXN1YXJlekBnbWFpbC5jb20ifQ.evIjoVUDz_PYVd0ZB_UmzMkXAkYURz3OLXHoN3CAQf8";
   
   // Serial.println(F("BMP280 test"));
    //if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    //  Serial.println(F("SSD1306 allocation failed"));
    // for(;;); // Don't proceed, loop forever
   //}
    //enciendo el sensor bme
   

}

void loop() {
  connect_to_wifi();
  digitalWrite(1,HIGH);
  while (!bmp.begin()) {
       Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
  }
  //temperature = bme.readTemperature();
  //humidity = bme.readHumidity();
  //pressure = bme.readPressure() / 100.0F;
  pressure = bmp.seaLevelForAltitude(920, bmp.readPressure() / 100.0F);
  printValues(temperature, humidity, pressure);
  //drawData(temperature, humidity, pressure);
  //logData(token, temperature, humidity, pressure);  
  disconnect_wifi();
  digitalWrite(1,LOW); //apago el sensor bme
  digitalWrite(11,LOW); //apago el sensor bme 
  digitalWrite(12,LOW); //apago el sensor bme    
  LowPower.sleep(1000*15*REFRESH_TIME_MIN);
  //delay(1000*60);
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
