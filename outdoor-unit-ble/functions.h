#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <ArduinoLowPower.h>
#include "config.h"


WiFiSSLClient client;
HttpClient httpClient = HttpClient(client, API_SERVER, API_PORT);

/**
 * Update: 16 Noviembre 2019. Finalmente he decidido alimentar el arduino por usb y descarto la idea de usar baterias, pues
 * sigo teniendo problemas de cuelgues tras entrar en modo sleep. No ocurre siempre, pero no me da ninguna fiabilidad.
 * Por lo tanto en la unidad exterior no voy a apagar el BLE en ningun momento, ya que su consumo en reposo es muy bajo.
 * ------------------------------------------------
 * Los procesadores SAMD21 se cuelgan cuando entran en modo sleep.
 * Esto se debe a que la cpu no espera a que la RAM se levante y el sistema colapsa
 * al tratar de acceder a la misma.
 * 
 * https://www.avrfreaks.net/forum/samd21-samd21e16b-sporadically-locks-and-does-not-wake-standby-sleep-mode
 * We have identified the issue with WDT reset. It happens due to SysTick timer.
 * Issue is that WDT initiates the wake up, but then SysTick interrupt starts to get handled first before system is actually ready. 
 * Basically, what happens is that SysTick interrupt does not wait for the RAM to properly wake up from sleep.
 * So if you wake up from WDT, the system will wait for the RAM, but the core clock will actually be running, so SysTick interrupt may happen too.
 * SysTick interrupt does not wait on the RAM, so the core attempts to run the SysTick handler and fails, since RAM is not ready.
 * This causes a Hard Fault (in our testing SRAM is so slow to wake up even Hard Fault handler).
 * This mean that device wakes up, getting into the Hard Fault, stay there until WDT fully expires.
 * You can reproduce the issue quicker by running SysTick timer faster, and WDT wake ups also quicker.
 * The solution for the customer is to disable SysTick interrupt before going to sleep and enable it back after the sleep.
 */

void resetBME (Adafruit_BME280 *bme) {
  digitalWrite(BME280_PIN, LOW);    // apago el sensor
  delay(900);
  digitalWrite(BME280_PIN, HIGH); // turn the sensor on
  delay(1000);
  while (!(*bme).begin(0x76)) {
    #ifdef DEBUG
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    #endif
    delay(1000);
  }
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

/**
 * Transforms ticks per second to Km/h
 */
float windLinearTransformation(float ticksSec, int duracion){
  Serial.print("La medicion dura: ");
  Serial.println(duracion);
  Serial.print("Ticks per sec = ");
  Serial.println(ticksSec);
  Serial.print("Wind = ");
  Serial.println(1.41*ticksSec+1.71);
  if(ticksSec>0)
    return(1.41*ticksSec+1.71);
  else
    return(0);
}

/**
 * Calculates average wind speed using last ten measurements
 */
float averageWindSpeed(float measurements[10]) {
  float acc = 0;
  for(int i=0; i<10; i++) {
    acc += measurements[i];
  }
  return acc / 10;
}

void connectToWifi() {
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("[WiFi] Connecting to: ");
    Serial.println(WIFI_SSID);
    // Connect to WPA/WPA2 network:
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    delay(1000);
  }
  Serial.println("[WiFi] Connected");
}

void disconnectWifi() {
  WiFi.disconnect();
  while(WiFi.status() != WL_DISCONNECTED){
    Serial.println("disconnect BLE failed!");
    WiFi.disconnect();
    delay(1000);
  }

  WiFi.end();
  Serial.println("[WiFi] Disconnected");
  delay(1000);
}

String sendAuth(String email, String password) {
  StaticJsonDocument<2000> doc;
  Serial.println("[HTTP] Authenticating...");
  httpClient.post("/api/auth/authenticate", "application/x-www-form-urlencoded", "email="+email+"&password="+password);
  // read the status code and body of the response
  int statusCode = httpClient.responseStatusCode();
  String response = httpClient.responseBody();
  deserializeJson(doc, response);
  String token = doc["data"]["token"];

  Serial.print("[HTTP] Status code: ");
  Serial.println(statusCode);
  Serial.print("[HTTP] Response: ");
  Serial.println(token);
  return token;
}

String postData(String token, String postData) {
  StaticJsonDocument<1000> doc;
  Serial.println("[HTTP] Logging data...");
  Serial.println("token: "+token);
  Serial.println("request: "+postData);
  httpClient.beginRequest();
  httpClient.post("/api/logging/log");
  httpClient.sendHeader(HTTP_HEADER_CONTENT_TYPE, "application/x-www-form-urlencoded");
  httpClient.sendHeader(HTTP_HEADER_CONTENT_LENGTH, postData.length());
  httpClient.sendHeader("Authorization", "Bearer "+token);
  httpClient.print(postData);
  httpClient.endRequest();

  // read the status code and body of the response
  int statusCode = httpClient.responseStatusCode();
  String response = httpClient.responseBody();
  deserializeJson(doc, response);
  String lastDate = doc["created_on"];
  lastDate = lastDate.substring(11,16);

  Serial.print("[HTTP] Status code: ");
  Serial.println(statusCode);
  Serial.print("[HTTP] Response: ");
  Serial.println(response);
  Serial.println(lastDate);
  return lastDate;
}

String logData(String token, float temperature, float humidity, float pressure, float wind) {
  connectToWifi();

  String temperatureString = String(temperature);
  String humidityString = String(humidity);
  String pressureString = String(pressure);
  String windString = String(wind);

  // indoor  
  String postDataString = "station_id="+String(STATION_ID_OUTDOOR)+"&temperature="+temperatureString+"&humidity="+humidityString+"&pressure="+pressureString+"&wind="+windString;
  String lastDate = postData(token, postDataString);

  disconnectWifi();
  return lastDate;
}
