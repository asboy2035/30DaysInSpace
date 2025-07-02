/*
 * 30 Days - Lost in Space
 * Day 16 - A Fancy New Display
 *
 * Learn more at https://learn.inventr.io/adventure
 *
 * Sound and light gives us some good feedback from our HERO, but what about
 * when we need numbers and the Serial Console isn't available?  Today we
 * introduce a 4-digit 7-segment display (TM1637).  This is mostly for numbers,
 * like a counter or perhaps a clock display, but with some imagination we can
 * also show some characters.
 *
 * Alex Eschenauer
 * David Schmidt
 * Greg Lyzenga
 */
 
/*
 * Arduino concepts introduced/documented in this lesson.
 * - Binary notation (0b00000001)
 * - logical OR
 * -
 *
 * Parts and electronics concepts introduced in this lesson.
 * - TM1637 4-digit 7-segment display
 */
 
// Explicitly include Arduino.h
#include "Arduino.h"
 
/*
 * Please refer to the lesson and lesson videos for instructions on how to
 * load the TM1637 library into your Arduino IDE before you use it for the
 * first time.
 *
 * In order to use the TM1637 library we include the file "TM1637Display.h".
 */
#include <TM1637Display.h>
 
// all_on pins connected to the TM1637 display
const byte CLK_PIN = 6;
const byte DIO_PIN = 5;
 
// Create display object of type TM1637Display:
TM1637Display display = TM1637Display(CLK_PIN, DIO_PIN);

// Numbers
const byte n0 = SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;
const byte n1 = SEG_B | SEG_C;
const byte n2 = SEG_A | SEG_B | SEG_D | SEG_E | SEG_G;
const byte n3 = SEG_A | SEG_B | SEG_C | SEG_D | SEG_G;
const byte n4 = SEG_B | SEG_C | SEG_F | SEG_G;
const byte n5 = SEG_A | SEG_C | SEG_D | SEG_F | SEG_G;
const byte n6 = SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
const byte n7 = SEG_A | SEG_B | SEG_C;
const byte n8 = SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
const byte n9 = SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G;

// Letters
const byte A = SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G;
const byte b = SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
const byte C = SEG_A | SEG_D | SEG_E | SEG_F;
const byte d = SEG_B | SEG_C | SEG_D | SEG_E | SEG_G;
const byte E = SEG_A | SEG_D | SEG_E | SEG_F | SEG_G;
const byte F = SEG_A | SEG_E | SEG_F | SEG_G;
const byte G = SEG_A | SEG_C | SEG_D | SEG_E | SEG_F;
const byte H = SEG_B | SEG_C | SEG_E | SEG_F | SEG_G;
const byte I = SEG_B | SEG_C;
const byte J = SEG_B | SEG_C | SEG_D | SEG_E;
const byte L = SEG_D | SEG_E | SEG_F;
const byte n = SEG_C | SEG_E | SEG_G;
const byte O = SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;
const byte P = SEG_A | SEG_B | SEG_E | SEG_F | SEG_G;
const byte q = SEG_A | SEG_B | SEG_C | SEG_F | SEG_G;
const byte r = SEG_E | SEG_G;
const byte S = SEG_A | SEG_C | SEG_D | SEG_F | SEG_G;
const byte t = SEG_D | SEG_E | SEG_F | SEG_G;
const byte U = SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;
const byte Y = SEG_B | SEG_C | SEG_D | SEG_F | SEG_G;
const byte Z = SEG_A | SEG_B | SEG_D | SEG_E | SEG_G;
 
/*
 * A 7-segment display is shaped like an "8" and has 7 segments (A through G) that can be
 * lit up. The diagram below shows the placement of each of the segments.
 *
 *   A
 * F   B
 *   G
 * E   C
 *   D
 *
 * You can set the individual segments per digit to spell words or create other symbols.
 * The TM1637 library provides constants for each segment on the display.  If you hover
 * your mouse over each of these constants you can see that each defines a byte with only
 * one bit set to 1.  SEG_A = 0b00000001, SEG_B = 0b00000010, etc.
 *
 * The Arduino C++ language uses the '|' operator to do a "bitwise or" between values.  It
 * takes each corresponding bits in two values and the output is 1 if *either* or both of the
 * corresponding bits is 1.  If both corresponding bits are 0 then the corresponding bit is
 * set to 0.
 *
 * In other words:
 * 0  0  1  1    operand1
 * 0  1  0  1    operand2
 * ----------
 * 0  1  1  1    (operand1 | operand2) - returned result
 */
 
 // By turning on selected segments we can display *some* alphabetic characters.
 // Here we spell out the word "dOnE".

// Strings
const byte done[] = { d, O, n, E };
const byte helo[] = { H, E, L, O };
const byte all_on[] = { n8, n8, n8, n8 };
const byte LETTERS[] = {
    A,  // Index 0
    b,  // Index 1
    C,  // Index 2
    d,  // Index 3
    E,  // Index 4
    F,  // Index 5
    G,  // Index 6
    H,  // Index 7
    I,  // Index 8
    J,  // Index 9
    L,  // Index 10
    n,  // Index 11
    O,  // Index 12
    P,  // Index 13
    q,  // Index 14
    r,  // Index 15
    S,  // Index 16
    t,  // Index 17
    U,  // Index 18
    Y,  // Index 19
    Z   // Index 20
};
const int NUM_LETTERS = sizeof(LETTERS) / sizeof(LETTERS[0]);

const byte NUMBERS[] = {
    n0,  // Index 0
    n1,  // Index 1
    n2,  // Index 2
    n3,  // Index 3
    n4,  // Index 4
    n5,  // Index 5
    n6,  // Index 6
    n7,  // Index 7
    n8,  // Index 8
    n9  // Index 9
};
const int NUM_NUMBERS = sizeof(NUMBERS) / sizeof(NUMBERS[0]);

// Functions
void displayArray(
  byte array[],
  int arrayLength,
  int itemDelay = 1
) {
  for (int i = 0; i < arrayLength; i+=4) {
    display.setSegments(&array[i], 4, 0);
    delaySeconds(itemDelay);
  }
}

void displayAlphabet(
  int letterDelay = 1
) {
  displayArray(LETTERS, NUM_LETTERS, letterDelay);
}

void delaySeconds(
  int seconds
) {
  delay(seconds * 1000); // in MS
}

// Main
void setup() {
  display.setBrightness(4);  // Configure the display brightness (0-7):
  Serial.begin(9600);
  Serial.println(NUM_NUMBERS);
}
 
void loop() {
  display.clear();

  display.setSegments(helo);
  delaySeconds(1);
 
  display.setSegments(all_on);
  delaySeconds(1);

  display.clear();
  delaySeconds(1);
 
  displayAlphabet();
  displayArray(NUMBERS, NUM_NUMBERS);

  /*
   *
   * Microwave after a power outage
   * Blinking 12:00.  The .showNumberDecEx() function has an additional parameter to turn
   * on the ':' after the 2nd digit.  This works well for time displays.
   *
   * The second parameter can also be used to control dots between digits if the
   * display has them.  In our case, the HERO display only has the colon.
   *
   * Dot/Colon enable. The argument is a bitmask, with each bit corresponding to a dot
   *        between the digits (or colon mark, as implemented by each module). i.e.
   *        For displays with dots between each digit:
   *        * 0.000 (0b10000000)
   *        * 00.00 (0b01000000)
   *        * 000.0 (0b00100000)
   *        * 0.0.0.0 (0b11100000)
   *        For displays with just a colon:     <== This is true for the HERO display
   *        * 00:00 (0b01000000)
   *        For displays with dots and colons colon:
   *        * 0.0:0.0 (0b11100000)
   */
  for (int i = 0; i < 4; i++) {
    display.showNumberDecEx(1200, 0b01000000);
    delay(500);
    // display.clear();
    display.showNumberDecEx(1200, 0b00000000);
    delay(500);
  }
 
  // Show counter including negative sign for negative numbers
  // NOTE: negative numbers cannot be less than -999 since the negative sign
  //       uses the left most digit of the display.
  // for (int i = -100; i <= 9999; i++) {
  //   display.showNumberDec(i);
  //   delaySeconds(5);
  // }
  // delaySeconds(1000);
 
  // Clear the display (all segments off)
  display.clear();
  delaySeconds(1);
 
  // Display the message "dOnE"
  display.setSegments(done);
 
  delaySeconds(10);
}