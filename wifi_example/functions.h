#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

const char ssid[] = "ANDESELEE";
const char pass[] = "u1pyrttnk8sgs";

const char server[] = "dpstogglapi1.davidinformatico.com";
const int port = 443;

int status = WL_IDLE_STATUS;
WiFiSSLClient client;
HttpClient http_client = HttpClient(client, server, port);

void connect_to_wifi() {
  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("[WiFi] Connecting to: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(100);
  }

  // you're connected now, so print out the data:
  Serial.println("[WiFi] Connected");
}

String sendAuth(String email, String password) {
  StaticJsonDocument<2000> doc;
  Serial.println("[HTTP] Authenticating...");
  http_client.post("/_/auth/authenticate", "application/x-www-form-urlencoded", "email="+email+"&password="+password);
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

void getTasks(String token) {
  StaticJsonDocument<6000> doc;
  Serial.println("[HTTP] Getting tasks...");
  http_client.beginRequest();
  http_client.get("/_/items/tasks");
  http_client.sendHeader("Authorization", "Bearer "+token);
  http_client.endRequest();
  
  // read the status code and body of the response
  int statusCode = http_client.responseStatusCode();
  String response = http_client.responseBody();
  deserializeJson(doc, response);
  String tasks = doc["data"];

  Serial.print("[HTTP] Status code: ");
  Serial.println(statusCode);
  Serial.print("[HTTP] Response: ");
  Serial.println(tasks);
}
