/*
Garage Door Opener
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "Secret.h"

ESP8266WebServer server(80);
Secret secret;

void setup() {
  Serial.begin(115200);  
  WiFi.begin(secret.getWifiSSID(), secret.getWifiPassword()); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Waiting to connect…");
  }
  Serial.print("Connect with IP address: ");
  Serial.println(WiFi.localIP());
  
  server.on("/open", open);
  server.on("/close", close);
  server.begin();
  Serial.println("Server started");
}

void loop() {
  server.handleClient();
}

void open() {
  server.send(200, "text/plain", "Hello world open");
}
void close() {
  server.send(200, "text/plain", "Hello world close");
}


