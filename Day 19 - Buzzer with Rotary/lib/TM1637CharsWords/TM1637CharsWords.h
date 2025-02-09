#ifndef TM1637CharsWords_H  // Prevent multiple inclusion
#define TM1637CharsWords_H

#include "Arduino.h"
#include <TM1637Display.h>

// Numbers
extern const byte n0 = SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;
const byte n1 = SEG_B | SEG_C;
const byte n2 = SEG_A | SEG_B | SEG_D | SEG_E | SEG_G;
const byte n3 = SEG_A | SEG_B | SEG_C | SEG_D | SEG_G;
const byte n4 = SEG_B | SEG_C | SEG_F | SEG_G;
const byte n5 = SEG_A | SEG_C | SEG_D | SEG_F | SEG_G;
const byte n6 = SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
const byte n7 = SEG_A | SEG_B | SEG_C;
const byte n8 = SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
const byte n9 = SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G;

// Symbols
const byte _ = SEG_D;
const byte dash = SEG_G;

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
const byte X = SEG_B | SEG_C | SEG_E | SEG_F | SEG_G;
const byte Z = SEG_A | SEG_B | SEG_D | SEG_E | SEG_G;

// Strings
const byte helo[] = { H, E, L, O };
const byte good[] = { G, O, O, d };
const byte stop[] = { S, t, O, P };
const byte hold[] = { H, O, L, d };
const byte exit1[] = { E, X, I, t };
const byte done[] = { d, O, n, E };
const byte nope[] = { n, O, P, E };
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

extern const byte NUMBERS[] = {
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

extern void displayArray(
  TM1637Display display,
  byte array[],
  int arrayLength,
  int itemDelay = 1
);

extern void displayAlphabet(
  TM1637Display display,
  int letterDelay = 1
);

extern void displayLoader(
  TM1637Display display,
  unsigned int position
);

extern void playLoaderSequence(
  TM1637Display display,
  unsigned int position,
  unsigned int times
);

extern void delaySeconds(
  int seconds
);

#endif
