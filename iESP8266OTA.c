#include<iESP8266OTA.h>

void initUpdateServer() //Call this as soon as you have WiFi
{
  updateServer.on("/", HTTP_GET, [](){
    updateServer.sendHeader("Connection", "close");
    updateServer.send(200, "text/html", "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>");
  });
  updateServer.on("/restart", HTTP_GET, [](){
    updateServer.sendHeader("Location", (std::string("http://")+std::string(WiFi.localIP().toString().c_str())+std::string(":80/")).c_str(), true);
    updateServer.send ( 302, "text/plain", "");
    delay(200);
    ESP.restart();    
  });
  updateServer.on("/update", HTTP_POST, [](){
    updateServer.sendHeader("Connection", "close");
    updateServer.send(200, "text/html", (Update.hasError())?"FAIL":"OK <a href='/restart'>restart?</a>");
  },[](){
    HTTPUpload& upload = updateServer.upload();
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
  updateServer.begin();
}
void handleUpdateServerClient()
{
  updateServer.handleClient();
}
