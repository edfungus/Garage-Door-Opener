#ifndef facebook_h
#define facebook_h

#include "secret.h"

class Facebook
{
  public:
    Facebook(Secret secret);
    bool checkGaragePermissions(char* userToken);    
  private:
    Secret _secret;
    char* getAccessToken();
    char* getUserIdFromUserToken(char* userToken);    
};

Facebook::Facebook(Secret secret)
{
  _secret = secret;
}

bool Facebook::checkGaragePermissions(char* userToken)
{
  return true;
}
char* Facebook::getAccessToken()
{
  return "sfd";
}
char* Facebook::getUserIdFromUserToken(char* userToken)
{
  return "sfd";
}

#endif
