#include "DisplayHandler.h"

#include "FacesBitmap.h"  // Import data gambar kustom Anda

DisplayHandler::DisplayHandler() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1) {
  lastBlinkTime = 0;
  lastFrameTime = 0;
  lastAnimTime = 0;
  isBlinking = false;
  currentFace = FACE_SLEEPING;  // Set wajah awal
  animStep = 0;
}

void DisplayHandler::setup() {
  // Inisialisasi I2C di pin khusus ESP32-C3
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  // Naikkan kecepatan I2C ke 400kHz agar animasi layar (FPS) lebih mulus
  Wire.setClock(400000);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);  // Berhenti jika layar gagal
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25, 28);
  display.println("Siochi Bangun...");
  display.display();
  delay(1500);
}

void DisplayHandler::setFace(FaceState face) {
  if (currentFace != face) {
    currentFace = face;
    animStep = 0;  // Reset frame animasi jika ekspresi berubah
  }
}

void DisplayHandler::update() {
  unsigned long currentMillis = millis();

  // Batasi Frame Rate agar I2C tidak over-load (~30 FPS max)
  if (currentMillis - lastFrameTime < 33) return;
  lastFrameTime = currentMillis;

  display.clearDisplay();

  // Default pointer gambar
  const unsigned char* bitmapToDraw = face_neutral;

  // Logika pergantian gambar berdasarkan status wajah
  switch (currentFace) {
    case FACE_HAPPY:
      bitmapToDraw = face_happy;
      break;

    case FACE_LOVE:
      bitmapToDraw = face_love;
      break;

    case FACE_SPEAKING:
      // Animasi berbicara (Berganti frame A/B tiap 150ms)
      if (currentMillis - lastAnimTime > 150) {
        animStep = !animStep;  // Toggle antara 0 dan 1
        lastAnimTime = currentMillis;
      }
      bitmapToDraw = (animStep == 0) ? face_speaking_a : face_speaking_b;
      break;

    case FACE_THINKING:
      // Animasi berpikir (Berganti frame 1/2 tiap 300ms)
      if (currentMillis - lastAnimTime > 300) {
        animStep = !animStep;
        lastAnimTime = currentMillis;
      }
      bitmapToDraw = (animStep == 0) ? face_thinking_1 : face_thinking_2;
      break;

    case FACE_SLEEPING:
      // Animasi tidur zzz (Berganti frame 1, 2, 3 tiap 500ms)
      if (currentMillis - lastAnimTime > 500) {
        animStep++;
        if (animStep > 2) animStep = 0;  // Reset setelah frame 3
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

  display.drawBitmap(0, 0, bitmapToDraw, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
  display.display();
}