/*
Garage Door Opener
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include "Secret.h"

ESP8266WebServer server(80);
WiFiClient espClient;
Secret secret;
PubSubClient mqttClient(espClient);

const long garageThreshold = 40;

#define relayPin 12 // pin D6 on wemos
#define pingEchoPin 13 // pin D7 on wemos 
#define pingTrigPin 15 // pin D8 pm wemos

void setup() 
{
  Serial.begin(115200);  

  digitalWrite(relayPin, HIGH);
  pinMode(relayPin, OUTPUT);
  pinMode(pingEchoPin, INPUT);
  pinMode(pingTrigPin, OUTPUT);

  WiFi.begin(secret.getWifiSSID(), secret.getWifiPassword()); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Waiting to connect…");
  }
  Serial.print("Connect with IP address: ");
  Serial.println(WiFi.localIP());

  mqttClient.setServer(secret.getMQTTIp(), 1883);
  mqttClient.setCallback(callback);

  server.on("/status", HTTP_GET, status);
  server.begin();
  Serial.println("Server started");
}

void loop() 
{
  if (!mqttClient.connected()) {
    reconnectMqtt();
  }
  mqttClient.loop();
  server.handleClient();
}


void reconnectMqtt() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "garageDoor";
    // Attempt to connect
    if (mqttClient.connect(clientId.c_str(), secret.getMQTTUsername(), secret.getMQTTPassword())) {
      Serial.println("connected to mqtt");
      mqttClient.subscribe("open");
      mqttClient.subscribe("close");
      mqttClient.subscribe("toggle");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic,"open")==0) {
    if(!isDoorOpen()) {
      toggleGarage();
    }
  }
  if (strcmp(topic,"close")==0) {
    if(isDoorOpen()) {
      toggleGarage();
    }
  }
  if (strcmp(topic,"toggle")==0) {
    toggleGarage();
  }
}

void status() {
  Serial.println("Getting garage status!");  
  if(isDoorOpen())
  {
    server.send(200, "application/json", "{\"state\":\"open\"}");
  } else {
    server.send(200, "application/json", "{\"state\":\"closed\"}");
  }
}

boolean isDoorOpen() {
  long duration, cm;
  long totalCm = 0;
  int loops = 3;

  for(int x = 0; x < loops; x++)
  {
    digitalWrite(pingTrigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(pingTrigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(pingTrigPin, LOW);
    duration = pulseIn(pingEchoPin, HIGH);
    cm = microsecondsToCentimeters(duration);
    totalCm = totalCm + cm;
    delay(100);
  }
  if((totalCm / loops) < garageThreshold)
  {
    return true;
  } else {
    return false;
  }
}

void toggleGarage(){
  Serial.println("Toggling garage!");
  digitalWrite(relayPin, LOW);
  delay(700); 
  digitalWrite(relayPin, HIGH);
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}