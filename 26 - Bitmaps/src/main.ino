#include "Arduino.h"

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

void setup(void) {
  Serial.begin(9600);
  lander_display.begin();   // initialize lander display
}

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
