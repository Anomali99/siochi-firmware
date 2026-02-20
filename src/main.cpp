#include "Siochi.h"

Siochi siochi;

void setup() {
  Serial.begin(115200);
  siochi.setup();
}

void loop() {
  siochi.loop();
}