#pragma once
#include <Arduino.h>

#include "Config.h"
#include "driver/i2s.h"

class AudioHandler {
 public:
  void setup();
  void playBeep();
  bool isHearingSound();
};