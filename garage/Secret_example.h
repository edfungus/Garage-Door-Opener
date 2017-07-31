#ifndef Secret_h
#define Secret_h

char WIFI_SSID[ ] = "???";
char WIFI_PASSWORD[ ] = "???";
char FB_ID[ ] = "???";
char FB_SECRET[ ] = "???";


class Secret_Example
{
  public:
    Secret();
    char* getWifiSSID();
    char* getWifiPassword();
    char* getFBId();
    char* getFBSecret();
};

Secret::Secret(){}

void Secret::getWifiSSID()
{
  return WIFI_SSID;
}
void Secret::getWifiPassword()
{
  return WIFI_PASSWORD;
}
void Secret::getFBId()
{
  return FB_ID;
}
void Secret::getFBSecret()
{
  return FB_SECRET;
}

#endif
