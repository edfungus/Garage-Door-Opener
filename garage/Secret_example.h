#ifndef secret_h
#define secret_h

char WIFI_SSID[ ] = "???";
char WIFI_PASSWORD[ ] = "???";
char FB_ID[ ] = "???";
char FB_SECRET[ ] = "???";

char* allowedUserIDs[] = {"???", "???"};

class Secret
{
  public:
    Secret();
    char* getWifiSSID();
    char* getWifiPassword();
    char* getFBId();
    char* getFBSecret();
};

Secret::Secret(){}

char* Secret::getWifiSSID()
{
  return WIFI_SSID;
}
char* Secret::getWifiPassword()
{
  return WIFI_PASSWORD;
}
char* Secret::getFBId()
{
  return FB_ID;
}
char* Secret::getFBSecret()
{
  return FB_SECRET;
}

#endif
