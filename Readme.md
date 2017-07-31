#### Garage Door Opener with Facebook Authentication

###### Introduction
This directory can be pushed to Cloud Foundary as the landing page for users to open or close the garage. After authenticating to Facebook, the "open" and "close" buttons will make requests to the ESP8266 that is connect to the garage door opener. The files inside `./garage` contains the code for the ESP8266 which I used the Arduino IDE to compile and upload. To use the ESP8266 code, a `Secret.h` file must be created with the appropiate credentials. Use `Secret_example.h` as a referenece. 