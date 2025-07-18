/*
 * 30 Days - Lost in Space
 * Day 12 - Can you hear us?
 *
 * Learn more at https://learn.inventr.io/adventure
 *
 * Today we will use the Keypad from Day 11 to control musical TONES using a
 * small speaker.  To do this we will show how to take the characters returned
 * by the Keypad library and convert them to musical notes played by our speaker.
 * Tones will play until changed or stopped (by pressing the 'D' button at the
 * bottom right of our 4x4 button matrix)
 *
 * Alex Eschenauer
 * David Schmidt
 * Greg Lyzenga
 */

/*
 * Arduino concepts introduced/documented in this lesson.
 * - libraries: Code provided by others that we can use in our sketches
 * - char type: Represents a single character like 'A', 'd', '4' or '*'.
 * - arrays:    Variables of the same type arranged as a list where each item
 *              can be accessed using an index.  Arrays can be one dimensional
 *              like a list (with one index) or two dimensional like a spreadsheet
 *              table with rows and columns (using row and column indexes).
 * - for loop:  Used to repeately perform actions.
 *
 * Parts and electronics concepts introduced in this lesson.
 */

// Explicitly include Arduino.h
#include "Arduino.h"

// Include Keypad library
#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 4;

const byte ROW_PINS[ROWS] = { 5, 4, 3, 2 };
const byte COL_PINS[COLS] = { 6, 7, 8, 9 };

char BUTTONS[ROWS][COLS] = {
  { '1', '2', '3', 'A' },  // 1st row
  { '4', '5', '6', 'B' },  // 2nd row
  { '7', '8', '9', 'C' },  // 3rd row
  { '*', '0', '#', 'D' }   // 4th row
};

Keypad myAwesomePad = Keypad(makeKeymap(BUTTONS), ROW_PINS, COL_PINS, ROWS, COLS);

const unsigned int TONES[ROWS][COLS] = {
  // a frequency tone for each button
  { 31, 93, 147, 208 },
  { 247, 311, 370, 440 },
  { 523, 587, 698, 880 },
  { 1397, 2637, 3729, 0 }  // Use frequency of 0 for bottom right key to end tone.
};

unsigned int TONESV2[256];

const byte BUZZER_PIN = 10;  // pin 10 drives the buzzer

void setup() {
  Serial.begin(9600);  // Begin monitoring via the serial monitor

  // TONESV2['1'] = 31;
  // TONESV2['2'] = 93;
  // TONESV2['3'] = 147;
  // TONESV2['A'] = 208;
  // TONESV2['4'] = 247;
  // TONESV2['5'] = 311;
  // TONESV2['6'] = 370;
  // TONESV2['B'] = 440;
  // TONESV2['7'] = 523;
  // TONESV2['8'] = 587;
  // TONESV2['9'] = 698;
  // TONESV2['C'] = 880;
  // TONESV2['*'] = 1397;
  // TONESV2['0'] = 2637;
  // TONESV2['#'] = 3729;
  // TONESV2['D'] = 0;

  TONESV2['1'] = 523; // C
  TONESV2['2'] = 587; // D
  TONESV2['3'] = 659; // E
  TONESV2['A'] = 739; // F#

  TONESV2['4'] = 493; // B
  TONESV2['5'] = 554; // C#
  TONESV2['6'] = 622; // D#
  TONESV2['B'] = 698; // F

  TONESV2['7'] = 783; // G
  TONESV2['8'] = 880; // A
  TONESV2['9'] = 987; // B
  TONESV2['C'] = 1108; // C#

  TONESV2['*'] = 830; // G#
  TONESV2['0'] = 932; // A#
  TONESV2['#'] = 1046; // C
  TONESV2['D'] = 0;
}


void loop() {
  char button_character = myAwesomePad.waitForKey();  // Wait for a button to be pressed

  /*
   * For loops:
   * for() has three parts.  Each is part is :
   *              START - operations performed when loop starts
   *              CONDITION - evaluate this condition as a boolean and stop loop
   *                          when false
   *              STEP - operations performed every time for loop ends
   *
   * NOTE: you will often see the short variable names "i", "j" and "k" used
   *       for variables in the for statement.  Those particular letters are used
   *       mostly for historical reasons going back to the FORTRAN language, but
   *       the variables in loops tend to be short in order to keep the for command
   *       readable.  The short names also remind you when used that they change
   *       each time through the loop.
   */
  unsigned int tone_frequency = 0;  // Frequency to use for tone (default to 0, no tone)
  // for (byte i = 0; i < ROWS; i++) {
  //   for (byte j = 0; j < COLS; j++) {
  //     if (button_character == BUTTONS[i][j]) {  // found it, get the corresponding tone
  //       tone_frequency = TONES[i][j];
  //     }

  //   }  // end j loop
  // }    // end i loop

  tone_frequency = TONESV2[button_character];

  Serial.print("Key: ");  //   send the button_character to serial monitor...
  Serial.print(button_character);
  Serial.print("   Freq: ");
  Serial.println(tone_frequency);

  const unsigned int toneLength = 350;

  /*
   * The tone() function plays a tone until stopped.  The code continues to run as the tone plays.
   */
  if (tone_frequency > 0) {                // If tone frequency greater than 0...
    tone(BUZZER_PIN, tone_frequency);  // ...then play the tone at that frequency until stopped
    delay(toneLength);
    noTone(BUZZER_PIN);
  } else {
    Serial.println("Stop tone");
    noTone(BUZZER_PIN);  // Stop pressed (tone frequency of 0) so stop any tone playing
  }
}
