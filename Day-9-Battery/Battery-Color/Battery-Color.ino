/*
 * 30 Days - Lost in Space
 * Day 9 - A Better Way to Do Things
 *
 * Learn more at https://learn.inventr.io/adventure
 *
 * Now that we know how to measure our battery charging rate and we know how to display
 * multiple colors with the RGB LED, let's combine those two concepts to display different
 * colors as our battery charges to give a good indication of our current battery state.
 *
 * Alex Eschenauer
 * David Schmidt
 * Greg Lyzenga
 */

/*
 * Arduino concepts introduced/documented in this lesson.
 * - float:   Variable type for decimal numbers that include a decimal point
 * - else if: Control structure for making multiple if decisions together
 *
 * Parts and electronics concepts introduced in this lesson.
 */
#include "Arduino.h"

// Our photoresistor will give us a reading of the current light level on this analog pin
const byte PHOTORESISTOR_PIN = A0;

// RGB LED pins
const byte RED_PIN = 11;
const byte GREEN_PIN = 10;
const byte BLUE_PIN = 9;

const long BATTERY_CAPACITY = 50000;  // Maximum battery capacity


unsigned long makeColor(
  byte red,    // red LED intensity (0-255)
  byte green,  // green LED intensity (0-255)
  byte blue    // blue LED intensity (0-255)  
) {
  unsigned long ured = (unsigned long)((unsigned long)red << 16);
  unsigned long ugreen = (unsigned long)((unsigned long)green << 8);
  unsigned long ublue = (unsigned long)blue;

  unsigned long packedColor = ublue | ugreen | ured;
  return packedColor;
}

// Colors
unsigned long NONE = makeColor( 0, 0, 0 );
unsigned long RED = makeColor( 255, 0, 0 );
unsigned long ORANGE = makeColor( 255, 20, 0 );
unsigned long YELLOW = makeColor( 255, 100, 0 );
unsigned long GREEN = makeColor( 0, 255, 0 );
unsigned long BLUE = makeColor( 0, 0, 255 );
unsigned long PURPLE = makeColor( 255, 0, 255 );

/*
 * Display a color on our RGB LED by providing an intensity for
 * our red, green and blue LEDs.
 */

void setup() {
  // Declare the RGB LED pins as outputs:
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  delay(2000);
  Serial.begin(9600);
}

void loop() {
  // Use static because we need this variable to maintain it's value across
  // multiple loop() runs.
  static long battery_level = 0;
  static unsigned long battery_increment = 0;

  battery_increment = analogRead(PHOTORESISTOR_PIN);
  if (battery_increment < 15) {
    battery_level -= (15 - battery_increment);
  } else {
    battery_level += battery_increment;  // Add current "charge amount" to our battery
  }

  // We can't charge the battery higher than it's capacity, set level as full if it goes over
  if (battery_level > BATTERY_CAPACITY) {
    battery_level = BATTERY_CAPACITY;
  }

  if (battery_level < 0) {
    battery_level = 0;
  }

  /*
   * IMPORTANT NOTE:
   * If you perform an operation using integer arithemetic each value is not
   * automatically converted to a floating point (decimal) number.
   *
   * In the calculation below, if we divide the integer battery value by the
   * integer battery capacity (battery_level / BATTERY_CAPACITY) then the result
   * would ALWAYS be 0, even if that 0 gets converted afterwards to a float (0.0).
   *
   * To avoid this, we use the "(float)" declaration before each of our values to
   * instruct the compiler to FIRST convert the value to floating point and then
   * perform the calculation using floating point math.
   *
   * If our current_battery_level is less than the battery capacity the initial
   * calculation gives us a number from 0.0 to 1.0.  We then multiple by 100 to
   * get a percentage value from 0.0 up to 100.0.
   */

  // Calculate Battery percentage
  float percentage = ((float)battery_level / (float)BATTERY_CAPACITY) * 100;

  if (percentage == 100.0) {
    showColor(PURPLE);
    Serial.print("#####");
  } else if (percentage >= 75.0) {
    showColor(BLUE);
    Serial.print("####-");
  } else if (percentage >= 50.0 && percentage < 75.0) {
    showColor(GREEN);
    Serial.print("###--");
  } else if (percentage >= 25.0 && percentage < 50.0) {
    showColor(YELLOW);
    Serial.print("##---");
  } else if (percentage >= 5.0 && percentage < 25.0) {
    showColor(ORANGE);
    Serial.print("#----");
  } else {
    // pulsate(RED, 20);
    prettyPulsate(RED, 0);
    Serial.print("#----");
  }

  Serial.print("    ");
  Serial.print(percentage);  // Display our floating point percentage (like 12.34) WITHOUT a newline
  Serial.print("%    ");
  if (battery_increment < 15) {
    Serial.print("-");
    Serial.println((15 - battery_increment));
  } else {
    Serial.print("+");
    Serial.println(battery_increment);
  }

  delay(20);  // Delay 1/10 of a second
}


// Functions

void displayColorXX(
  unsigned long color
) {
  byte red = (byte)((color & 0xFF0000) >> 16);
  byte green = (byte)((color & 0xFF00) >> 8);
  byte blue = (byte)(color & 0xFF);

  displayColor(red, green, blue);
}

void displayColor(
  byte red_intensity,    // red LED intensity (0-255)
  byte green_intensity,  // green LED intensity (0-255)
  byte blue_intensity    // blue LED intensity (0-255)
) {
  analogWrite(RED_PIN, red_intensity);      // write red LED intensity using PWM
  analogWrite(GREEN_PIN, green_intensity);  // write green LED intensity using PWM
  analogWrite(BLUE_PIN, blue_intensity);    // write blue LED intensity using PWM
}

void showColor(
  unsigned long color
) {
  displayColorXX(color);
}

// Display color functions
void displayColorWithDelay(  
  byte red_intensity,    // red LED intensity (0-255)
  byte green_intensity,  // green LED intensity (0-255)
  byte blue_intensity,    // blue LED intensity (0-255)
  unsigned int color_delay
) {
  displayColorXX(makeColor(red_intensity, green_intensity, blue_intensity));
  delay(color_delay);
}

// Transition
void transition(
  byte startColor[],
  byte endColor[],
  byte numSteps
) {
  byte d1 = (startColor[0] - endColor[0]) / numSteps;
  byte d2 = (startColor[1] - endColor[1]) / numSteps;
  byte d3 = (startColor[2] - endColor[2]) / numSteps;

  // Current colors
  byte currentRed = startColor[0];
  byte currentGreen = startColor[1];
  byte currentBlue = startColor[2];

  for (int i = 0; i < numSteps; i++) {
    currentRed = currentRed - d1;
    currentGreen = currentGreen - d2;
    currentBlue = currentBlue - d3;
    displayColorWithDelay(currentRed, currentGreen, currentBlue, 20);
  }
}

void prettyPulsate(
  unsigned long color,
  byte gap
) {
  byte colorValues[] = {
    (byte)((color & 0xFF0000) >> 16),
    (byte)((color & 0xFF00) >> 8),
    (byte)(color & 0xFF)
  };

  byte none[] = { 0, 0, 0 };

  transition(colorValues, none, 10);
  delay(gap);
  transition(none, colorValues, 10);
}

void pulsate(
  byte color[], 
  byte gap
) {
  showColor(NONE);
  delay(gap);
  showColor(color);
}
