#include <Arduino.h>

#include "ErrorDisplay.h"
#include "HardwareConfig.h"

ErrorDisplay& errorDisplay = ErrorDisplay::instance();

void setup() {
  Serial.begin(115200);

  Serial.println("Setup complete.");
}

void loop() {
  errorDisplay.toggle();

  delay(500);
}
