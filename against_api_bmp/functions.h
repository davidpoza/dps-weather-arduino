#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

const char ssid[] = "ANDESELEE";
const char pass[] = "u1pyrttnk8sgs";

const char server[] = "apiweather.davidinformatico.com";
const int port = 443;

int status = WL_IDLE_STATUS;
WiFiSSLClient client;
HttpClient http_client = HttpClient(client, server, port);

void connect_to_wifi() {
  IPAddress staticIP(192,168,1,66);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);
  IPAddress dns(8,8,8,8); 
  
  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    //Redice su consumo a 30-40mA.
    WiFi.lowPowerMode();
    //para reducir el tiempo de conexion
    WiFi.config(staticIP, gateway, subnet, dns); 
    Serial.print("[WiFi] Connecting to: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    
    WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(1000);
  }
  Serial.println("[WiFi] Connected");
}

void disconnect_wifi() {
  status = WiFi.disconnect();
  WiFi.end();
  Serial.println("[WiFi] Disconnected");
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

void logData(String token, float temperature, float humidity, float pressure) {
  String temperatureString = String(temperature);
  String humidityString = String(humidity);
  String pressureString = String(pressure);
  String postData = "temperature="+temperatureString+"&humidity="+humidityString+"&pressure="+pressureString;
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
}
