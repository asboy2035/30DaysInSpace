/*
 * 30 Days - Lost in Space
 * Day 26 - Fun With Bitmaps
 *
 * Explorer, your lander is finally back in space, but we still need to rejoin
 * the mothership so more work is required.
 *
 * As we continue to build up your control panel, we can see that our loop
 * is running slower and slower, especially as we draw more complex graphics
 * on the display (like our lander).
 *
 * There is a more efficient way to display graphics on our display, though.
 * What if we draw an image *once* and save that picture?  Then we can just
 * move it quickly onto our display rather than going through the work to draw
 * each piece separately.
 *
 * Our graphics library provides a way to do JUST that using something called
 * a "bitmap".  This is simply a representation of an image using zero and one
 * bit.  A zero bit leaves a pixel turned off, and a one bit turns it on.
 *
 * Our image can now be saved in our sketch as an array of bits that then gets
 * mapped to our display.  (Thus, "bitmaps").  You can find a number of sample
 * images below our loop() below, along with an explanation of how they were
 * created and represented in our sketch.
 *
 * Learn more at https://learn.inventr.io/adventure
 *
 * Alex Eschenauer
 * David Schmidt
 * Greg Lyzenga
 */

/*
 * Arduino concepts introduced/documented in this lesson.
 * - bitmaps
 * - hexadecimal
 * - forward declarations
 * - pointers
 *
 * Parts and electronics concepts are introduced in this lesson.
 * -
 */

// Explicitly include Arduino.h
#include "Arduino.h"

// Extensive documentation for this library can be found at https://github.com/olikraus/u8g2
#include <U8g2lib.h>  // Include file for the U8g2 library.
#include "Wire.h"     // Sometimes required for I2C communications.
#include "bitmaps.h"  // Include our bitmap definitions.

U8G2_SH1106_128X64_NONAME_2_HW_I2C lander_display(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

#define BITMAP(bitmapName) extern const unsigned char bitmapName[] U8X8_PROGMEM // define bitmap stuff

BITMAP(niceRocketBro);
BITMAP(planetBitmap);
BITMAP(earthBitmap);
BITMAP(inventrCorpLogoBitmap);

BITMAP(socratesQuote);
BITMAP(installingOptimism);
BITMAP(liverpool);

/*
 * Data pointers (addresses)
 *
 * In this sketch we wish to display each of the images in turn
 * on our lander display.  We COULD write this like:
 *
 * lander_display.drawXBM(...); // Draw first image
 * delay(2000);                 // delay 2 seconds
 * lander_display.drawXBM(...); // Draw second image
 * delay(2000);                 // delay 2 seconds
 *
 * But this is very repetitive and wordy.  If we simply had a list
 * of images in an array, we could then write a loop that displays
 * each in turn.
 *
 * Arduino C++ allows us to do this, using something called a "pointer".
 * A pointer represents the ADDRESS in memory of the first byte of
 * a piece of data (like the arrays of data for our images).
 *
 * This array simply holds pointers to each of our image arrays, and
 * we can now reference each image using bitmaps[0], bitmap[1], etc.
 */
const static unsigned char* bitmaps[] = {
  niceRocketBro,
  planetBitmap,
  earthBitmap,
  inventrCorpLogoBitmap,
  socratesQuote,
  installingOptimism,
  liverpool,
};

// ************************************************
void setup(void) {
  Serial.begin(9600);
  lander_display.begin();   // initialize lander display
}

// ************************************************
void loop(void) {

  // The number of items in our array is the total size of our array (in bytes)
  // divided by the size of the first item.
  int number_of_bitmaps = sizeof(bitmaps) / sizeof(bitmaps[0]);

  // Now display each of the bitmaps followed by a delay.
  for (int i = 0; i < number_of_bitmaps; i++) {
    lander_display.firstPage();
    do {
      // .drawXBMP() displays each bitmap starting in the upper left corner
      // (0, 0) that has a width of 128 and height of 64.
      lander_display.drawXBMP(0, 0, 128, 64, bitmaps[i]);
    } while (lander_display.nextPage());

    // Display each bitmap for 2 seconds.
    delay(4000);
  }
}

/*
 * Graphics data arrays definition
 *
 * We need to define each of our images using zeros and ones, and there are a
 * number of ways we can do this.
 *
 * 1) We could use decimal numbers to represent each byte of data using the
 *    numbers 0-255:
 *      234, 0, 127, 255, ...
 *    But this is hard to align since each byte is 1-3 digits long.
 * 2) We could use binary constants like you've used before:
 *      0b11101101, 0b00000000, 0b01111111, 0b11111111, ...
 *    But this isn't very compact and is more error-prone if typed manually.
 *
 * There is another way of representing data that is commonly used for computer
 * code.  Instead of only having 0 or 1 (two values) to represent a single bit,
 * we can use more symbols.  We could use decimal (base 10) numbers, but this doesn't
 * map well onto computers with data lengths that are powers of two (1, 2, 4, 8, etc.).
 *
 * Programmers created Hexadecimal notation to address these issues.  Hexadecimal
 * is a base-16 number system, meaning it has 16 distinct symbols: 0-9 to represent
 * values zero to nine, and A-F to represent values ten to fifteen.  This allows
 * data to be represented in a compact format that aligns well with bit lengths that
 * are powers of 2.
 */

