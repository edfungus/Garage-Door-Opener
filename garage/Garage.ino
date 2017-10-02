/*
Garage Door Opener
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "facebook.h"
#include "Secret.h"

ESP8266WebServer server(80);
Secret secret;
Facebook facebook(secret);
const char* argName = "token";
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
  
  server.on("/open", HTTP_POST, open);  
  server.on("/close", HTTP_POST, close);
  server.on("/toggle", HTTP_POST, toggle);
  server.on("/status", HTTP_GET, status);
  server.begin();
  Serial.println("Server started");
}

void loop() 
{
  server.handleClient();
}

void open() {
  String token = getUserToken();
  if(token.equals("error"))
  {
    server.send(422, "text/plain", "Uhh, token?");   
    return;
  }
  if(facebook.checkGaragePermissions(token))
  {
    if(!isDoorOpen())
    {
      toggleGarage();
    }
    server.send(200, "text/plain", "Hello world open");
  }
  else 
  {
    server.send(403, "text/plain", "Oops, you can't do that.");    
  }
}
void close() {
  String token = getUserToken();
  if(token.equals("error"))
  {
    server.send(422, "text/plain", "Uhh, token?");   
    return;
  }
  if(facebook.checkGaragePermissions(token))
  {
    if(isDoorOpen())
    {
      toggleGarage();
    }
    server.send(200, "text/plain", "Hello world close");
  }
  else 
  {
    server.send(403, "text/plain", "Oops, you can't do that.");    
  }
}
void toggle() {
  String token = getUserToken();
  if(token.equals("error"))
  {
    server.send(422, "text/plain", "Uhh, token?");   
    return;
  }
  if(facebook.checkGaragePermissions(token))
  {
    toggleGarage();
    server.send(200, "text/plain", "Hello world close");
  }
  else 
  {
    server.send(403, "text/plain", "Oops, you can't do that.");    
  }
}
void status() {
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

String getUserToken() {
  for (int i = 0; i < server.args(); i++) 
  {
    if(server.argName(i) == argName)
    {
      return server.arg(i);
    }
  } 
  return String("error");
}

void toggleGarage(){
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
