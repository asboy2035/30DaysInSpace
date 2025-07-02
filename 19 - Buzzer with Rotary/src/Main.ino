#include "Arduino.h"
#include <TM1637Display.h> // 7-seg display
#include <TM1637CharsWords.h> // Extra words/funcs for 7-seg
#include <BasicEncoder.h> // Rotary encoder
 
// Keys from Day 17
constexpr unsigned int KEYS[] = {
  23,
  353,
  1688
};


// Pins
// The Rotary Encoder will be used to control our lander's depth underwater using
// interrupts (explained below).  Since the HERO board only supports interrupts on
// pins 2 and 3, we MUST use those pins for rotary encoder inputs.
constexpr byte DEPTH_CONTROL_CLK = 2;
constexpr byte DEPTH_CONTROL_DT = 3;

// Define the display connection pins:
constexpr byte DEPTH_GAUGE_CLK = 6;
constexpr byte DEPTH_GAUGE_DIO = 5;

// Buzzer
constexpr byte BUZZER_PIN = 10;


// Hardware
// Our TM1637 4-digit 7-segment display will be used as our "depth gauge".
auto depth_gauge = TM1637Display(DEPTH_GAUGE_CLK, DEPTH_GAUGE_DIO);

// Create BasicEncoder instance for our depth control (which initializes counter to 0)
BasicEncoder depth_control(DEPTH_CONTROL_CLK, DEPTH_CONTROL_DT);


constexpr byte BLINK_COUNT = 3;   // blink depth gauge this many times for attention.

// Our lander is currently this deep underwater so this is what is initially
// shown on our "depth gauge" (4-digit 7-segment display).
constexpr int INITIAL_DEPTH = -60;

// Alert user when we have risen 50% and 75% of the way up.
constexpr int ALERT_DEPTH_1 = INITIAL_DEPTH * 0.50;  // First alert when 50% of the way up
constexpr int ALERT_DEPTH_2 = INITIAL_DEPTH * 0.25;  // Second alert when 75% of the way up (25% of original depth)

constexpr int SURFACE_DEPTH = 0;    // Depth of the sea surface


// Funcs
void playTone(
  const String& reason,
  const unsigned int frequency,
  const unsigned long duration
) {
  Serial.println(reason);
  tone(BUZZER_PIN, frequency, duration);
}

// Validate that the explorer has entered the correct key values
// This is deliberately cryptic so it's not apparent what the 3 keys are.
bool keysAreValid() {
  unsigned int i = 0155;
  if (KEYS[0]!=0b10110*'+'/051)i+= 2;
  if (KEYS[1]==uint16_t(0x8f23)/'4'-0537)i|= 0200;
  if (KEYS[2]!=0x70b1/021-0b1001)i+=020;
  return !(18^i^0377);
}

// Blink our current depth off and on to alert the user.
void blinkDepth(int depth) {
  for (int i = 0; i < BLINK_COUNT; i++) {
    depth_gauge.setSegments(hold);
    delay(300);
    depth_gauge.showNumberDec(depth);  // display current depth
    delay(300);
  }
}

// Interrupt Service Routine (ISR).  Let BasicEncoder library handle the rotator changes
void updateEncoder() {
  depth_control.service();  // Call BasicEncoder library .service()
}


// Main
void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
 
  // Setup Serial Monitor
  Serial.begin(9600);
  delay(1000);
 
  depth_gauge.setBrightness(7);  // Set depth gauge brightness to max (values 0-7)
  depth_gauge.clear();
  playLoaderSequence(depth_gauge, 3, 6);
 
  if (keysAreValid()) {
    depth_gauge.showNumberDec(INITIAL_DEPTH);  // Display our initial depth on our depth gauge.
  } else {
    depth_gauge.setSegments(nope);  // Display "nOPE" on display to show key error
    Serial.println("ERROR: Invalid keys.  Please enter the 3 numeric keys from Day 17");
    Serial.println("       in order in the KEYS array at the start of this sketch.");
    while (true);
  }
 
  // Call Interrupt Service Routine (ISR) updateEncoder() when any high/low change
  // is seen on A (DEPTH_CONTROL_CLK_PIN) interrupt  (pin 2), or B (DEPTH_CONTROL_DT_PIN) interrupt (pin 3)
  attachInterrupt(digitalPinToInterrupt(DEPTH_CONTROL_CLK), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DEPTH_CONTROL_DT), updateEncoder, CHANGE);
}


constexpr unsigned int LOOP_DELAY = 200;  // Delay in ms between loop() executions.
 
void loop() {
  // Depth from the previous loop, initialized to our initial depth first time
  // through the loop().  When changed it retains it's value between loop executions.
  static int previous_depth = INITIAL_DEPTH;  // Depth from our previous loop(),
 
  if (depth_control.get_change()) {  // If the depth control value has changed since last check
    // The rotary encoder library always sets the initial counter to 0, so we will always
    // add our initial depth to the counter to properly track our current depth.
    int current_depth = INITIAL_DEPTH + depth_control.get_count();
 
    // Compute our percentage of the way up.
    // NOTE: We can avoid using slower floating point arithmetic by first multiplying the
    //       current depth by 100.  This is a trick we often use when the final result
    //       doesn't require any decimal portion.

    // Rising too quickly could stress the hull of our lander.  Because of this we will
    // play an alert if the lander is instructed to rise faster than 1 meter every time
    // through our loop.
    const int rise_rate = current_depth - previous_depth;
    if (rise_rate > 1) {
      playTone("rising too quick", 80, LOOP_DELAY);
      playLoaderSequence(depth_gauge, 0, 2);
    }
 
    // We cannot go deeper than the sea floor where the lander sits, so reset the counter
    // if the user tries to go deeper than our initial depth.
    if (current_depth < INITIAL_DEPTH) {
      current_depth = INITIAL_DEPTH;
      depth_control.reset();
    }
 
    // Display our current depth on our digital depth gauge
    depth_gauge.showNumberDec(current_depth);
 
    // Since BasicEncoder can be clicked multiple times per loop via interrupts we
    // track when counter has passed milestones THIS time through the loop.  We do
    // by testing whether the previous counter was less than the milestone and current
    // counter is greater or equal.
 
    // If we crossed our first alert level then flash "hold" on the display.
    if (
      previous_depth < ALERT_DEPTH_1 &&
      current_depth >= ALERT_DEPTH_1
    ) {
      blinkDepth(current_depth);
    }
 
    // If we crossed our second alert level then flash "hold" on the display.
    if (
      previous_depth < ALERT_DEPTH_2 &&
      current_depth >= ALERT_DEPTH_2
    ) {
      blinkDepth(current_depth);
    }
 
    // We have reached the surface!  Blink "dOnE" on our depth gauge and play a
    // happy completion tone.
    if (current_depth >= SURFACE_DEPTH) {
      // Play 'tada!' tune on our buzzer.
      playTone("Tada 1", 440, LOOP_DELAY);
      delay(LOOP_DELAY);
      playTone("Tada 2", 600, LOOP_DELAY * 4);

      for (int i = 0; i < BLINK_COUNT; i++) {
        playLoaderSequence(depth_gauge, 1, 4);
        delay(300);
        depth_gauge.setSegments(done);  // Display "dOnE"
        delay(300);
      }
    }

    previous_depth = current_depth;  // save current depth for next time through the loop
  }

  delay(LOOP_DELAY);
}
