#ifndef facebook_h
#define facebook_h

#include "secret.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

const char* facebookHost = "graph.facebook.com";
const int facebookPort = 443;
const char* accessTokenURLPath = "/oauth/access_token";
const char* userIDURLPath = "/debug_token";

class Facebook
{
  public:
    Facebook(Secret secret);
    bool checkGaragePermissions(String userToken);    
  private:
    Secret _secret;
    char* getAccessToken();
    char* getUserIdFromUserToken(String userToken, char* accessToken);
    WiFiClientSecure getWifiClient();   
    void makeRequest(String url, WiFiClientSecure client);
    char* getJsonFromRequest(WiFiClientSecure client); 
    JsonObject& getObjectFromString(char* json);
    char* constCharToChar(const char* key);
    char* stringToChar(String value);
    JsonObject& getObjectFromJson(JsonObject& jsonObj, char* key);
};

Facebook::Facebook(Secret secret)
{
  Serial.begin(115200);
  _secret = secret;
}

bool Facebook::checkGaragePermissions(String userToken)
{
  char* accessToken = getAccessToken();
  Serial.println(accessToken);
  char* userID = getUserIdFromUserToken(userToken, accessToken);
  Serial.println(userID);
  bool allowed = _secret.isUserAllowed(userID);
  free(accessToken);
  free(userID);
  return allowed;
}

char* Facebook::getAccessToken()
{
  WiFiClientSecure client = getWifiClient();

  String url = accessTokenURLPath;
  url += "?client_id=";
  url += _secret.getFBId();
  url += "&client_secret=";
  url += _secret.getFBSecret();
  url += "&grant_type=client_credentials";
  Serial.println(url);
  
  makeRequest(url, client);

  char* json = getJsonFromRequest(client);
  Serial.println();  
  Serial.println(json);

  JsonObject& jsonObj = getObjectFromString(json);
  strcpy(userData->name, root["name"]);
  const char* token = jsonObj["access_token"];
  free(json);
  return constCharToChar();
}

char* Facebook::getUserIdFromUserToken(String userToken, char* accessToken)
{
  WiFiClientSecure client = getWifiClient();

  String url = userIDURLPath;
  url += "?input_token=";
  url += userToken;
  url += "&access_token=";
  url += accessToken;
  Serial.println(url);
  
  makeRequest(url, client);

  char* json = getJsonFromRequest(client);
  Serial.println();  
  Serial.println(json);

  JsonObject& jsonObj = getObjectFromString(json);
  free(json);
  // const char* value = jsonObj["data"]["user_id"];
  // int length = strlen(value);
  // Serial.println(length);
  // char* newValue = (char*) malloc (length);
  // strcpy (newValue, value);
  // return newValue;  
  return constCharToChar(jsonObj["data"]["user_id"]);
}

WiFiClientSecure Facebook::getWifiClient()
{
  WiFiClientSecure client;
  while (!client.connect(facebookHost, facebookPort)) {
    Serial.println("Connection to Facebook failed...");
  }
  return client;  
}

void Facebook::makeRequest(String url, WiFiClientSecure client)
{
  String request = String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + facebookHost + ":" + String(facebookPort) + "\r\n" + 
               "Connection: close\r\n\r\n";
  Serial.println(request);
  client.print(request);
}

char* Facebook::getJsonFromRequest(WiFiClientSecure client)
{
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
    if(line.charAt(1) == '{')
    {
      return stringToChar(line);
    }
  }
  return "error";
}

JsonObject& Facebook::getObjectFromString(char* json)
{
  StaticJsonBuffer<300> jsonBuffer;  
  JsonObject& jsonObj = jsonBuffer.parseObject(json);
  if (!jsonObj.success()) {
    Serial.println("Could not parse repsonse from Facebook");
  }
  return jsonObj;
}

char* Facebook::constCharToChar(const char* value)
{
  int length = strlen(value);
  Serial.println(length);
  char* newValue = (char*) malloc (length);
  strcpy (newValue, value);
  return newValue;
}

char* Facebook::stringToChar(String value)
{
  int length = sizeof(value);
  Serial.println(length);
  char* newValue = (char*) malloc (length);
  value.toCharArray(newValue, length);
}

JsonObject& Facebook::getObjectFromJson(JsonObject& jsonObj, char* key)
{
  JsonObject& newJsonObj = jsonObj[key];
  return jsonObj;
}

#endif
