#include <WiFiNINA.h>
#include <ArduinoBLE.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include "config.h"

WiFiSSLClient client;
HttpClient httpClient = HttpClient(client, API_SERVER, API_PORT);

void connectBle() {
  Serial.println("[BLE] Activating...");
  while (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    delay(1000);
  }
  Serial.println("[BLE] Activated");
}

void disconnectBle() {
  while(BLE.connected()){
    BLE.disconnect();
    Serial.println("disconnect BLE failed!");
    delay(1000);
  }
  BLE.end();
  Serial.println("[BLE] Deactivated");
  delay(1000);
}

// after MAX_DISCOVER_ATTEMPTS reinit BLE and scan
void resetBle(int *attempts){
  if(*attempts > MAX_DISCOVER_ATTEMPTS) {
    disconnectBle();
    connectBle();
    *attempts = 0;
    BLE.scanForUuid(BLE_OUTDOOR_STATION_ID);
  }
  else{
    (*attempts)++;
  }
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

String logData(String token, float indoorT, float indoorH, float outdoorT, float outdoorH, float pressure) {
  connectToWifi();

  String indoorTemperatureString = String(indoorT);
  String indoorHumidityString = String(indoorH);
  String outdoorTemperatureString = String(outdoorT);
  String outdoorHumidityString = String(outdoorH);
  String pressureString = String(pressure);

  // indoor
  String postDataString = "station_id="+String(STATION_ID_INDOOR)+"&temperature="+indoorTemperatureString+"&humidity="+indoorHumidityString+"&pressure="+pressureString;
  postData(token, postDataString);

  // outdoor
  postDataString = "station_id="+String(STATION_ID_OUTDOOR)+"&temperature="+outdoorTemperatureString+"&humidity="+outdoorHumidityString+"&pressure="+pressureString;
  String lastDate = postData(token, postDataString);

  disconnectWifi();
  return lastDate;
}
