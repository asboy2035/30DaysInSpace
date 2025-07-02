#include "Arduino.h"

const int CABIN_LIGHTS_PIN = 10;
const int CABIN_LIGHTS_SWITCH_PIN = 2;


void setup() {
  pinMode(CABIN_LIGHTS_PIN, OUTPUT);        // Set light control pin as an OUTPUT
  pinMode(CABIN_LIGHTS_SWITCH_PIN, INPUT);  // Since we read from the switch, this pin is an INPUT
}

void loop() {
/*
  // First version of our loop.  It works, but could be shorter, more efficient and
  // more understandable.  Commented out but left in to show how we can incrementally
  // improve our code.

  byte switch_setting;

  switch_setting = digitalRead(CABIN_LIGHTS_SWITCH_PIN);   // Read state of light switch

  if (switch_setting == HIGH) {
    digitalWrite(CABIN_LIGHTS_PIN, HIGH);  // if switch is ON then turn on our lander's light
  }

  if (switch_setting == LOW) {
    digitalWrite(CABIN_LIGHTS_PIN, LOW);  // if switch is OFF then turn off lander's light
  }
*/

/*
  // Second version of our loop.  Better.  Define the variable and set it in the same statement
  // and then use the else statement to show that we should always execute ONE of our two
  // actions.

  byte switch_setting = digitalRead(CABIN_LIGHTS_SWITCH_PIN);

  if (switch_setting == HIGH) {
    digitalWrite(CABIN_LIGHTS_PIN, HIGH);  // Switch is ON, turn on our lander's light
  } else {
    digitalWrite(CABIN_LIGHTS_PIN, LOW);  // Switch is OFF, turn off lander's light
  }
*/

  // Final version!  Since we only use our value once, just do our digitalRead once and
  // remove the (now unnecessary) variable.  Clean, fast and less likely to allow bugs
  // to be introduced.

  // Each time loop() begins digitalRead() reads the input pin attached to the switch and
  // compares the value read to HIGH (switch is ON)
  if (digitalRead(CABIN_LIGHTS_SWITCH_PIN) == HIGH) {
    delay(1000);
    digitalWrite(CABIN_LIGHTS_PIN, HIGH);  // Switch is ON, turn on our lander's light
    delay(500);
    digitalWrite(CABIN_LIGHTS_PIN, LOW);
    delay(500);
    digitalWrite(CABIN_LIGHTS_PIN, HIGH);
  } else {
    delay(2000);
    digitalWrite(CABIN_LIGHTS_PIN, LOW);  // Switch is OFF, turn off lander's light
  }
}
