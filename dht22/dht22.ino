#include "DHT.h"

#define DHTPIN 1
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
void setup() {
  Serial.begin(9600);
  Serial.println("Iniciando...");
  dht.begin();
}

void loop() {
  delay(3000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.print("Humedad: ");
  Serial.print(h);
  Serial.println(" %");
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.println(" *C ");
}
