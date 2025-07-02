#include "Arduino.h"

const int CABIN_LIGHTS_PIN = 12


void setup() {
  pinMode(CABIN_LIGHTS_PIN, OUTPUT);    // We will control our lander's lights as an OUTPUT.
}

void loop() {
  loopn(4);
  delay(3000);                           // Wait for one second (1000 milliseconds) with the light ON.
  loopn(3);
  delay(3000);                           // Wait for one second (1000 milliseconds) with the light ON.
  loopn(2);
  delay(3000);                           // Wait for one second (1000 milliseconds) with the light ON.
  loopn(1);
  // digitalWrite(CABIN_LIGHTS_PIN, HIGH);  // This line turns the lander's light ON.
  // delay(1000);                           // Wait for one second (1000 milliseconds) with the light ON.
  // digitalWrite(CABIN_LIGHTS_PIN, LOW);   // This line turns the lander's light OFF.
  // delay(2000);                            // Wait for a tenth of a second (100 milliseconds) with the light OFF.
  delay(3600 * 1000);
}

void loopn(int n) {
  for(int i = 0; i < n; i++) {
    digitalWrite(CABIN_LIGHTS_PIN, HIGH);  // This line turns the lander's light ON.
    delay(1000);                           // Wait for one second (1000 milliseconds) with the light ON.
    digitalWrite(CABIN_LIGHTS_PIN, LOW);   // This line turns the lander's light OFF.
    delay(500);                            // Wait for a tenth of a second (100 milliseconds) with the light OFF.
  }
}
