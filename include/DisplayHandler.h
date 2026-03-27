#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Wire.h>

#include "Config.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

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
  FaceState currentFace;
  uint8_t animStep;
  String nApp, nSender, nMessage;
  unsigned long lastBlinkTime;
  unsigned long lastFrameTime;
  unsigned long lastAnimTime;
  unsigned long expressionStartTime;
  unsigned long notifStartTime;
  bool isBlinking;
  bool isShowingNotif;
  int nMode;

 public:
  DisplayHandler();
  FaceState getCurrentFace();
  void setup();
  void setFace(FaceState face);
  void update();
  void showNotification(String app, String sender, String message, int mode);
};