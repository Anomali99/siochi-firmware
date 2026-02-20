#pragma once

#include <Arduino.h>

#include "Connectivity.h"

class Siochi {
 private:
  Connectivity connectivity;

 public:
  Siochi();
  void setup();
  void loop();
};
