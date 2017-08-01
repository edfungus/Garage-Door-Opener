#ifndef facebook_h
#define facebook_h

#include "secret.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

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
    char* searchJsonForKey(char* json, char* key);
    char* stringToChar(String value);
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
  Serial.println("+++++++++++++++++");  
  Serial.println(json);   
  char* token = searchJsonForKey(json, "access_token\":\"");
  free(json);

  return token;
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
  Serial.println("+++++++++++++++++");  
  Serial.println(json);

  char* token = searchJsonForKey(json, "user_id\":\"");
  free(json);

  return token;
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

char* Facebook::searchJsonForKey(char* json, char* key)
{
  char* tokenStart;
  char* matchStart = key;
  tokenStart = strstr(json, matchStart);
  char* tokenInJson = tokenStart + (int) strlen(matchStart);
  char* matchEnd = "\"";  
  int tokenLength = (int) strstr(tokenInJson, matchEnd) - (int) tokenInJson;
  char* token = (char*) malloc (tokenLength + 1);
  strncpy(token, tokenInJson, tokenLength);
  token[tokenLength] = (char) 0;
  return token;
  
}

char* Facebook::stringToChar(String value)
{
  int length = value.length() + 1;
  Serial.println(length);
  char* newValue = (char*) malloc (length);
  value.toCharArray(newValue, length);
  return newValue;
}

#endif
