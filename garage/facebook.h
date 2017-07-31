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
    String getAccessToken();
    String getUserIdFromUserToken(String userToken, String accessToken);
    WiFiClientSecure getWifiClient();   
    void makeRequest(String url, WiFiClientSecure client);
    String getJsonFromRequest(WiFiClientSecure client); 
    JsonObject& getObjectFromString(String json);
    String getValueFromJson(JsonObject& jsonObj, char* key);
    JsonObject& getObjectFromJson(JsonObject& jsonObj, char* key);
};

Facebook::Facebook(Secret secret)
{
  Serial.begin(115200);
  _secret = secret;
}

bool Facebook::checkGaragePermissions(String userToken)
{
  String accessToken = getAccessToken();
  Serial.println(accessToken);
  String userID = getUserIdFromUserToken(userToken, accessToken);
  Serial.println(userID);
  return _secret.isUserAllowed(userID);
}

String Facebook::getAccessToken()
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

  String json = getJsonFromRequest(client);
  Serial.println();  
  Serial.println(json);

  JsonObject& jsonObj = getObjectFromString(json);
  return getValueFromJson(jsonObj, "access_token");
}

String Facebook::getUserIdFromUserToken(String userToken, String accessToken)
{
  WiFiClientSecure client = getWifiClient();

  String url = userIDURLPath;
  url += "?input_token=";
  url += userToken;
  url += "&access_token=";
  url += accessToken;
  Serial.println(url);
  
  makeRequest(url, client);

  String json = getJsonFromRequest(client);
  Serial.println();  
  Serial.println(json);

  JsonObject& jsonObj = getObjectFromString(json);
  JsonObject& newJsonObj = getObjectFromJson(jsonObj, "data");
  return getValueFromJson(newJsonObj, "user_id");
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

String Facebook::getJsonFromRequest(WiFiClientSecure client)
{
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
    if(line.charAt(1) == '{')
    {
      return line;
    }
  }
  return String("error");
}

JsonObject& Facebook::getObjectFromString(String json)
{
  StaticJsonBuffer<300> jsonBuffer;  
  JsonObject& jsonObj = jsonBuffer.parseObject(json);
  if (!jsonObj.success()) {
    Serial.println("Could not parse repsonse from Facebook");
  }
  return jsonObj;
}

String Facebook::getValueFromJson(JsonObject& jsonObj, char* key)
{
  const char* valueConst = jsonObj[key]; 
  return String(valueConst);
}

JsonObject& Facebook::getObjectFromJson(JsonObject& jsonObj, char* key)
{
  JsonObject& newJsonObj = jsonObj[key];
  return jsonObj;
}

#endif
