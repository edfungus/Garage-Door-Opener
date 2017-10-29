#ifndef secret_h
#define secret_h

const char WIFI_SSID[ ] = "???";
const char WIFI_PASSWORD[ ] = "???";
const char MQTT_USERNAME[ ] = "???";
const char MQTT_PASSWORD[ ] = "???";
const char MQTT_IP[ ] = "???";

class Secret
{
  public:
    Secret();
    char const* getWifiSSID();
    char const* getWifiPassword();
    char const* getMQTTUsername();
    char const* getMQTTPassword();
    char const* getMQTTIp();
};

Secret::Secret(){}

char const* Secret::getWifiSSID()
{
  return WIFI_SSID;
}
char const* Secret::getWifiPassword()
{
  return WIFI_PASSWORD;
}
char const* Secret::getMQTTUsername()
{
  return MQTT_USERNAME;
}
char const* Secret::getMQTTPassword()
{
  return MQTT_PASSWORD;
}
char const* Secret::getMQTTIp()
{
  return MQTT_IP;
}

#endif
