/////////////////////////////
// Houz Office node
//

//Houz
#include "src/HouzCore/HouzCore.h"
#include "src/HouzCore/devs.h"
#include "01_Office.h"
HouzCore houzCore;
OfficeNode office(&houzCore);

#include "src/wemos_WiFi/wemos_WiFi.h"
#include "Arduino.h"

void setup(){
  //set pins
  Serial.begin(115200);
  Serial.println("\n\n:: Houz office node");
  wemosWiFi.connect("houz_office");
  office.setup();
};

void loop(){
  office.update();
  wemosWiFi.update();
};
