#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Wire.h>

#include "Config.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Status Wajah
enum FaceState {
  FACE_NEUTRAL,
  FACE_HAPPY,
  FACE_LOVE,
  FACE_SAD,
  FACE_ANGRY,
  FACE_CONFUSED,
  FACE_SPEAKING,
  FACE_SLEEPING,
  FACE_THINKING
};

class DisplayHandler {
 private:
  Adafruit_SSD1306 display;

  unsigned long lastBlinkTime;
  unsigned long lastFrameTime;
  unsigned long lastAnimTime;
  unsigned long expressionStartTime;
  bool isBlinking;

  FaceState currentFace;
  uint8_t animStep;

 public:
  DisplayHandler();
  void setup();
  void setFace(FaceState face);
  FaceState getCurrentFace();
  void update();
};