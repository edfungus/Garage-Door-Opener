#### Garage Door Opener with Facebook Authentication

##### Introduction
This is a website that allows you to open your garage door. The actual door control is done by an ESP8266. There is alos Facebook authentication to control who has permission. The website handles the interface and authentication while the ESP8266 acts as a server to handle the `/open` and `/close` API calls. The ESP8266 in this case is programmed via the Arduino IDE. 

##### How To Use
* Run the go server...
* You must make a new Facebook app and use the domain from the website as an app domain. Note the App Id and App Secret
* Create a copy of the file `garage/Secret_example.h`, name it `secret.h` and fill in the secret infomation from Facebook and your Wifi information. Note that the user IDs from Facebook is unique to your app.
* Wire up your relay/control to the appropiate pin of you ESP8266 board. Beware of pin number conversions based on which type of board you have (NodeMUC, Wemos, no dev board, etc)
