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

#define relayPin 16

void setup() 
{
  Serial.begin(115200);  

  pinMode(relayPin, OUTPUT);

  WiFi.begin(secret.getWifiSSID(), secret.getWifiPassword()); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Waiting to connect…");
  }
  Serial.print("Connect with IP address: ");
  Serial.println(WiFi.localIP());
  
  server.on("/open", HTTP_OPTIONS, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
    server.send(200, "text/plain", "" );
  });
  server.on("/open", HTTP_PUT, [](){
    server.sendHeader("Access-Control-Allow-Origin", "*");    
    open();    
  });  
  server.on("/close", close);
  server.begin();
  Serial.println("Server started");
}

void loop() 
{
  server.handleClient();
}

void open() 
{
  String token = getUserToken();
  if(token.equals("error"))
  {
    server.sendHeader("Access-Control-Allow-Origin", "*");          
    server.send(422, "text/plain", "Uhh, token?");   
    return;
  }
  if(facebook.checkGaragePermissions(token))
  {
    toggleGarage();
    server.sendHeader("Access-Control-Allow-Origin", "*");        
    server.send(200, "text/plain", "Hello world open");
  }
  else 
  {
    server.sendHeader("Access-Control-Allow-Origin", "*");        
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
    toggleGarage();
    server.send(200, "text/plain", "Hello world close");
  }
  else 
  {
    server.send(403, "text/plain", "Oops, you can't do that.");    
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
  digitalWrite(relayPin, HIGH);
  delay(100); 
  digitalWrite(relayPin, LOW);
}
