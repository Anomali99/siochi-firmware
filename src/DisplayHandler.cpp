#include "DisplayHandler.h"

#include "FacesBitmap.h"

DisplayHandler::DisplayHandler() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1) {
  lastBlinkTime = 0;
  lastFrameTime = 0;
  lastAnimTime = 0;
  isBlinking = false;
  currentFace = FACE_SLEEPING;
  animStep = 0;
}

void DisplayHandler::setup() {
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  Wire.setClock(400000);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25, 28);
  display.println("Siochi Bangun...");
  display.display();
  delay(1500);
}

void DisplayHandler::showNotification(String app, String sender, String message, int mode) {
  nApp = app;
  nSender = sender;
  nMessage = message;
  nMode = mode;
  isShowingNotif = true;
  notifStartTime = millis();
}

void DisplayHandler::setFace(FaceState face) {
  if (currentFace != face) {
    currentFace = face;
    animStep = 0;
  }
}

void DisplayHandler::update() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastFrameTime < 33) return;
  lastFrameTime = currentMillis;

  display.clearDisplay();

  const unsigned char* bitmapToDraw = face_neutral;

  switch (currentFace) {
    case FACE_HAPPY:
      bitmapToDraw = face_happy;
      break;

    case FACE_LOVE:
      bitmapToDraw = face_love;
      break;

    case FACE_SPEAKING:
      if (currentMillis - lastAnimTime > 150) {
        animStep = !animStep;
        lastAnimTime = currentMillis;
      }
      bitmapToDraw = (animStep == 0) ? face_speaking_a : face_speaking_b;
      break;

    case FACE_THINKING:
      if (currentMillis - lastAnimTime > 300) {
        animStep = !animStep;
        lastAnimTime = currentMillis;
      }
      bitmapToDraw = (animStep == 0) ? face_thinking_1 : face_thinking_2;
      break;

    case FACE_SLEEPING:
      if (currentMillis - lastAnimTime > 500) {
        animStep++;
        if (animStep > 2) animStep = 0;
        lastAnimTime = currentMillis;
      }
      if (animStep == 0)
        bitmapToDraw = face_sleep_1;
      else if (animStep == 1)
        bitmapToDraw = face_sleep_2;
      else
        bitmapToDraw = face_sleep_3;
      break;

    case FACE_NEUTRAL:
    default:
      if (!isBlinking && currentMillis - lastBlinkTime > random(2000, 5000)) {
        isBlinking = true;
        lastBlinkTime = currentMillis;
      }

      if (isBlinking) {
        if (currentMillis - lastBlinkTime < 150) {
          bitmapToDraw = face_blink;
        } else {
          isBlinking = false;
          lastBlinkTime = currentMillis;
        }
      }
      break;
  }

  if (isShowingNotif && nMode == 2) {
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("--- " + nApp + " ---");
    display.println(nSender + ":");
    display.println(nMessage);
  } else {
    display.drawBitmap(0, 0, bitmapToDraw, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);

    if (isShowingNotif) {
      display.fillRect(0, 44, 128, 20, SSD1306_BLACK);
      display.drawLine(0, 43, 128, 43, SSD1306_WHITE);

      display.setCursor(2, 47);
      display.setTextWrap(false);
      display.print(nSender + ": " + nMessage);
      display.setTextWrap(true);
    }
  }

  if (isShowingNotif && (currentMillis - notifStartTime > 5000)) {
    isShowingNotif = false;
  }

  display.display();
}