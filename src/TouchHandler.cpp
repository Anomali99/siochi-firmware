#include "TouchHandler.h"

void TouchHandler::setup() {
  pinMode(PIN_TOUCH, INPUT);
}

bool TouchHandler::isTouched() {
  // TTP223 akan bernilai HIGH (1) saat disentuh
  return digitalRead(PIN_TOUCH) == HIGH;
}