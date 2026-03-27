#pragma once

#include <Arduino.h>

#include "AudioHandler.h"
#include "Config.h"
#include "Connectivity.h"
#include "DisplayHandler.h"
#include "TouchHandler.h"

class Siochi {
 private:
  SystemState currentState;
  DisplayHandler display;
  TouchHandler touch;
  AudioHandler audio;
  Connectivity connectivity;
  FaceState currentFaceState;
  unsigned long lastInteractionTime;
  unsigned long moodTimer;
  bool wasTouched;
  bool isAwake;

 public:
  Siochi();
  void setup();
  void loop();
};