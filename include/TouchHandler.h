#pragma once
#include <Arduino.h>

#include "Config.h"

class TouchHandler {
 public:
  void setup();
  bool isTouched();
};