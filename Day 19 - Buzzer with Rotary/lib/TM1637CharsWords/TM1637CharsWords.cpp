#include "Arduino.h"
#include <TM1637Display.h>
#include <TM1637CharsWords.h>

// Loader positions
const byte L1 = SEG_A;
const byte L2 = SEG_B;
const byte L3 = SEG_C;
const byte L4 = SEG_D;
const byte L5 = SEG_E;
const byte L6 = SEG_F;
// Loader positions on all 4 digits
const byte LOADER[] = {
  L1, L2, L3, L4, L5, L6
};
const int LOADER_LENGTH = sizeof(LOADER) / sizeof(LOADER[0]);

// Functions
void delaySeconds(
  int seconds
) {
  delay(seconds * 1000); // in MS
}

void displayArray(
  TM1637Display display,
  byte array[],
  int arrayLength,
  int itemDelay = 1
) {
  for (int i = 0; i < arrayLength; i+=4) {
    display.setSegments(&array[i], 4, 0);
    delaySeconds(itemDelay);
  }
}

void displayLoader(
  TM1637Display display,
  unsigned int position
) {
    for (int i = 0; i < LOADER_LENGTH; i++) {
    display.setSegments(&LOADER[i], 1, position);
    delay(100);
  }
}

void playLoaderSequence(
  TM1637Display display,
  unsigned int position,
  unsigned int times
) {
  for (int i = 0; i < times; i++)
  {
    displayLoader(display, position);
  }
  
}

void displayAlphabet(
  TM1637Display display,
  int letterDelay = 1
) {
  displayArray(display, (byte *)LETTERS, NUM_LETTERS, letterDelay);
}
