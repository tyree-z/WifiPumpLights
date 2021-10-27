#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#ifndef STASSID
#define STASSID "WuTangClan"
#define STAPSK  "Ziggy1111"
#define SERVERIP "10.0.0.197:3000"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const int pumpRead = 0;
const int pumpWrite = 2;

void handleRoot() {
  server.send(200, "text/plain", "hello from my waterpump");
}

void sendApiActivate() {
  WiFiClient client;
  HTTPClient http;
    
  StaticJsonDocument<200> doc;
  doc["isOn"] = true;
  String json;
  serializeJson(doc, json);
  
  Serial.print("[HTTP] begin...\n");
  http.begin(client, "http://" SERVERIP "/v1/iot/rx");
  http.addHeader("Content-Type", "application/json");
  Serial.print("[HTTP] POST...\n");
  int httpCode = http.POST(json);
  if (httpCode > 0) {
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}

void sendApiDeactivate() {
  WiFiClient client;
  HTTPClient http;

  StaticJsonDocument<200> doc;
  doc["isOn"] = false;
  String json;
  serializeJson(doc, json);
  
  Serial.print("[HTTP] begin...\n");
  http.begin(client, "http://" SERVERIP "/v1/iot/rx");
  http.addHeader("Content-Type", "application/json");
  Serial.print("[HTTP] POST...\n");
  int httpCode = http.POST(json);
  if (httpCode > 0) {
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}

void handleActivate() {
  digitalWrite(pumpWrite, LOW);
  sendApiActivate();
  }
void handleDeactivate() {
  digitalWrite(pumpWrite, HIGH);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  pinMode(pumpRead, INPUT);
  pinMode(pumpWrite, OUTPUT);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.on("/on", handleActivate);
  server.on("/off", handleDeactivate);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  if (digitalRead(pumpRead) == 0){
    handleActivate();
    delay(30001);
    handleDeactivate();
    sendApiDeactivate();
    }
    else {
      handleDeactivate();
      }
      
  server.handleClient();
}
