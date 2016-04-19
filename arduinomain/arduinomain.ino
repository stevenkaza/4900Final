#include "dataDisplay.h"
#include "Arduino.h"

dataDisplay data;
void setup() {
      data.begin();
      data.setupLED();
   // data.enableLED();
      data.flashLED(500);

  // put your setup code here, to run once:

}

void loop() {
    data.process();
  // put your main code here, to run repeatedly:

}

