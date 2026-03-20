#pragma once

#include <Arduino.h>

#include "AudioHandler.h"
#include "Config.h"
#include "DisplayHandler.h"
#include "TouchHandler.h"
// #include "Connectivity.h" // Nanti kita aktifkan

class Siochi {
 private:
  SystemState currentState;

  DisplayHandler display;
  TouchHandler touch;
  AudioHandler audio;
  // Connectivity connectivity;

  bool wasTouched;

  bool isAwake;
  unsigned long lastInteractionTime;
  unsigned long moodTimer;
  FaceState currentFaceState;

 public:
  Siochi();
  void setup();
  void loop();
};