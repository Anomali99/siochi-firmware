#include "TouchHandler.h"

void TouchHandler::setup() {
  pinMode(PIN_TOUCH, INPUT);
}

bool TouchHandler::isTouched() {
  return digitalRead(PIN_TOUCH) == HIGH;
}