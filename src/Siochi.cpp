#include "Siochi.h"

Siochi::Siochi() : connectivity() {}

void Siochi::setup() {
  connectivity.setup();
}

void Siochi::loop() {
  connectivity.loop();
  delay(50);
}