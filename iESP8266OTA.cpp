#include<iESP8266OTA.h>

void iESP8266OTA::iESP8266OTA()
{
  mWebServer = ESP8266WebServer(8080);
  mWebServer.on("/", HTTP_GET, [](){
    mWebServer.sendHeader("Connection", "close");
    mWebServer.send(200, "text/html", "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>");
  });
  mWebServer.on("/restart", HTTP_GET, [](){
    mWebServer.sendHeader("Location", (std::string("http://")+std::string(WiFi.localIP().toString().c_str())+std::string(":80/")).c_str(), true);
    mWebServer.send ( 302, "text/plain", "");
    delay(200);
    ESP.restart();    
  });
  mWebServer.on("/update", HTTP_POST, [](){
    mWebServer.sendHeader("Connection", "close");
    mWebServer.send(200, "text/html", (Update.hasError())?"FAIL":"OK <a href='/restart'>restart?</a>");
  },[](){
    HTTPUpload& upload = mWebServer.upload();
    if(upload.status == UPLOAD_FILE_START){
      //Serial.setDebugOutput(true);
      //Serial.printf("Update: %s\n", upload.filename.c_str());
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if(!Update.begin(maxSketchSpace)){//start with max available size
        //Update.printError(Serial);
      }
    } else if(upload.status == UPLOAD_FILE_WRITE){
      if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
        //Update.printError(Serial);
      }
    } else if(upload.status == UPLOAD_FILE_END){
      if(Update.end(true)){ //true to set the size to the current progress
        //Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        //Update.printError(Serial);
      }
      //Serial.setDebugOutput(false);
    }
    yield();
  });
  mWebServer.begin();
}
void iESP8266OTA::handleClient()
{
  mWebServer.handleClient();
}
