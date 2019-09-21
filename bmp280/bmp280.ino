#include <Adafruit_Sensor.h>
#include "Adafruit_BMP280.h"
// por defecto la libreria lee en la direccion 0x77, y para poner el chip en esa dirección mandamos un 
// valor 1 en el pin sd0
Adafruit_BMP280 bmp; // I2C
float presion; // Almacena la presion atmosferica (Pa)
float temperatura; // Almacena la temperatura (oC)
int altitud; // Almacena la altitud (m) (se puede usar variable float)

void setup() {
  Serial.begin(9600);// Inicia comunicacion serie
  delay(2000);
  if(!bmp.begin()){ // Inicia el sensor
    Serial.println("BMP280 no encontrado");
    delay(10);
  }
  Serial.println("BMP280 Sensor de Presion y Temperatura");
}

void loop() {
   // Lee valores del sensor:
   presion = bmp.readPressure()/100;
   temperatura = bmp.readTemperature();
   altitud = bmp.readAltitude (1015); // presión en hPa a nivel del mar
   
   // Imprime valores por el serial monitor:
   Serial.print(F("Presion: "));
   Serial.print(presion);
   Serial.println(" hPa");

   delay(3000);
}
