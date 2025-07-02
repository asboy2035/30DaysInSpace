#include "Arduino.h"
 
// Dependencies
#include <TM1637Display.h> // Display
#include <TM1637CharsWords.h> // Letters, words, and extra functions
#include <BasicEncoder.h> // Rotary dial
 
// Hardware
// Pins
const byte DIAL_CLK_PIN = 2;  // HERO interrupt pin connected to encoder CLK input
const byte DIAL_DT_PIN = 3;   // HERO interrupt pin connected to encoder DT input
const byte DIAL_SW_PIN = 4;

// Display pins
const byte CLOCK_CLK_PIN = 6;
const byte CLOCK_DIO_PIN = 5;

// Buzzer pin
const byte BUZZER_PIN = 10;
 
// Devices
BasicEncoder dial(DIAL_CLK_PIN, DIAL_DT_PIN);
TM1637Display clock = TM1637Display(CLOCK_CLK_PIN, CLOCK_DIO_PIN);

const int INITIAL_VALUE = 5700;
 
void setup() {
  pinMode(DIAL_SW_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
 
  // Setup Serial Monitor
  Serial.begin(9600);
  delay(1000);
 
  clock.setBrightness(7);  // Set depth gauge brightness to max (values 0-7)
  clock.clear();
  playLoaderSequence(clock, 3, 6);
  clock.clear();
 
  // Call Interrupt Service Routine (ISR) updateEncoder() when any high/low change
  // is seen on A (DIAL_CLK_PIN) interrupt  (pin 2), or B (DIAL_DT_PIN) interrupt (pin 3)
  attachInterrupt(digitalPinToInterrupt(DIAL_CLK_PIN), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DIAL_DT_PIN), updateEncoder, CHANGE);
}
 
const unsigned int LOOP_DELAY = 50;  // Delay in ms between loop() executions.
 
void loop() {
  static int currentValue = INITIAL_VALUE;

  if (dial.get_change()) {  // If the dial value has changed since last check
    currentValue = INITIAL_VALUE + dial.get_count();

    // Prevent going below 0
    if (currentValue < INITIAL_VALUE) {
      currentValue = INITIAL_VALUE;
      dial.reset();
    }

    showTimerConfig(currentValue);
  }

  if (isButtonPressed()) { // Start timer onclick
    Serial.println("Button clicked!");
    timer(currentValue);
    currentValue = INITIAL_VALUE;
  }

  delay(LOOP_DELAY);
}

void showTimerConfig(int currentValue) {
  if (currentValue <= 59) {
    showSeconds(currentValue);
    return;
  }

  int minutes = currentValue / 60;
  int seconds = currentValue % 60;

  showMinutes(minutes, seconds);
}

void showSeconds(unsigned int seconds) {
  clock.showNumberDec(seconds);
}

void showMinutes(unsigned int minutes, unsigned int seconds) {
  int units = minutes % 10;
  int tens = minutes / 10;

  int combinedValue = (minutes * 100) + seconds; // Example: 1:34 → 134
  clock.showNumberDecEx(combinedValue, 0b01000000); // Shows MM:SS format
}

bool isButtonPressed() {
  if (digitalRead(DIAL_SW_PIN) == LOW) {
    delay(50); // Basic debounce
    if (digitalRead(DIAL_SW_PIN) == LOW) { // Ensure button is still pressed
      return true;
    }
  }
  return false;
}

void timer(
  int timeSeconds
) {
  Serial.println("Timer started.");
  playLoaderSequence(clock, 3, 2);
  for (int i = timeSeconds; i >= 0; i--) {
    showTimerConfig(i);
    delaySeconds(1);
  }
  tone(BUZZER_PIN, 440, 2000);
  playLoaderSequence(clock, 3, 2);
  clock.clear();
}

// Interrupt Service Routine (ISR).  Let BasicEncoder library handle the rotator changes
void updateEncoder() {
  dial.service();  // Call BasicEncoder library .service()
}