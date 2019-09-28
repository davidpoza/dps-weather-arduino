#include <WiFiNINA.h>
#include <ArduinoBLE.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include "config.h"

WiFiSSLClient client;
HttpClient http_client = HttpClient(client, API_SERVER, API_PORT);

void connect_ble() {
  Serial.println("[BLE] Activating...");
  while (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    delay(1000);
  }
  Serial.println("[BLE] Activated");
}

void disconnect_ble() {
  while(BLE.connected()){
    BLE.disconnect();
    Serial.println("disconnect BLE failed!");
    delay(1000);
  }
  BLE.end();
  Serial.println("[BLE] Deactivated");
  delay(1000);
}

void connect_to_wifi() {
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("[WiFi] Connecting to: ");
    Serial.println(WIFI_SSID);
    // Connect to WPA/WPA2 network:
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    delay(1000);
  }
  Serial.println("[WiFi] Connected");
}

void disconnect_wifi() {
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
  http_client.post("/api/auth/authenticate", "application/x-www-form-urlencoded", "email="+email+"&password="+password);
  // read the status code and body of the response
  int statusCode = http_client.responseStatusCode();
  String response = http_client.responseBody();
  deserializeJson(doc, response);
  String token = doc["data"]["token"];

  Serial.print("[HTTP] Status code: ");
  Serial.println(statusCode);
  Serial.print("[HTTP] Response: ");
  Serial.println(token);
  return token;
}

void logData(String token, String station_id, float temperature, float humidity, float pressure) {
  connect_to_wifi();
  String temperatureString = String(temperature);
  String humidityString = String(humidity);
  String pressureString = String(pressure);
  String postData = "station_id="+station_id+"&temperature="+temperatureString+"&humidity="+humidityString+"&pressure="+pressureString;
  StaticJsonDocument<1000> doc;
  Serial.println("[HTTP] Logging data...");
  Serial.println("token: "+token);
  Serial.println("request: "+postData);
  http_client.beginRequest();
  http_client.post("/api/logging/log");
  http_client.sendHeader(HTTP_HEADER_CONTENT_TYPE, "application/x-www-form-urlencoded");
  http_client.sendHeader(HTTP_HEADER_CONTENT_LENGTH, postData.length());
  http_client.sendHeader("Authorization", "Bearer "+token);
  http_client.print(postData);
  http_client.endRequest();

  
  // read the status code and body of the response
  int statusCode = http_client.responseStatusCode();
  String response = http_client.responseBody();
  deserializeJson(doc, response);
  String tasks = doc["data"];

  Serial.print("[HTTP] Status code: ");
  Serial.println(statusCode);
  Serial.print("[HTTP] Response: ");
  Serial.println(response);
  disconnect_wifi();
}
