#include "Siochi.h"

#define TIME_TO_SLEEP 15000
#define TIME_TO_HOLD_MOOD 3000

Siochi::Siochi() {
  currentState = STATE_OFFLINE;
  wasTouched = false;
  isAwake = false;
  lastInteractionTime = 0;
  moodTimer = 0;
  currentFaceState = FACE_SLEEPING;
}

void Siochi::setup() {
  Serial.println("Siochi Menginisialisasi Hardware...");

  display.setup();
  touch.setup();
  audio.setup();

  // connectivity.setup(); // Disembunyikan sementara

  Serial.println("Siochi Siap dalam Mode OFFLINE.");

  isAwake = false;
  currentFaceState = FACE_SLEEPING;
  display.setFace(currentFaceState);
  lastInteractionTime = millis();
}

void Siochi::loop() {
  bool isTouchedNow = touch.isTouched();
  bool isHearingSoundNow = audio.isHearingSound();
  unsigned long currentMillis = millis();

  switch (currentState) {
    case STATE_OFFLINE:
      if (isTouchedNow && !wasTouched) {
        lastInteractionTime = currentMillis;

        if (!isAwake) {
          Serial.println("Siochi Bangun dari tidur!");
          isAwake = true;
          currentFaceState = FACE_NEUTRAL;
          audio.playBeep();
        } else {
          Serial.println("Mochi dielus! (Love)");
          currentFaceState = FACE_LOVE;
          moodTimer = currentMillis;
          audio.playBeep();
        }
        display.setFace(currentFaceState);
      }

      else if (isHearingSoundNow && isAwake && currentFaceState != FACE_THINKING) {
        Serial.println("Ada yang berbicara! (Berpikir)");
        currentFaceState = FACE_THINKING;
        moodTimer = currentMillis;
        lastInteractionTime = currentMillis;
        display.setFace(currentFaceState);
      }

      if (isAwake) {
        if (currentFaceState == FACE_LOVE || currentFaceState == FACE_THINKING) {
          if (currentMillis - moodTimer > TIME_TO_HOLD_MOOD) {
            Serial.println("Kembali ke ekspresi Netral.");
            currentFaceState = FACE_NEUTRAL;
            display.setFace(currentFaceState);
          }
        }

        if (currentFaceState == FACE_NEUTRAL) {
          if (currentMillis - lastInteractionTime > TIME_TO_SLEEP) {
            Serial.println("Siochi mengantuk... (Tidur)");
            isAwake = false;
            currentFaceState = FACE_SLEEPING;
            display.setFace(currentFaceState);
          }
        }
      }

      break;

    case STATE_BLE_WAITING:
      break;

    case STATE_WIFI_CONNECTING:
      break;

    case STATE_ONLINE:
      break;
  }

  wasTouched = isTouchedNow;

  display.update();

  delay(10);
}