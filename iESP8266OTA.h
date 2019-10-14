#ifndef IESP8266OTA_H_
#define IESP8266OTA_H_

#include <ESP8266WebServer.h>
//Do we need additional headers for ESP.restart() and Update.begin()?

class iESP8266OTA {
  ESP8266WebServer mWebServer;
  public:
    iESP8266OTA();
    handleClient();
};

#endif /* IESP8266OTA_H_ */
