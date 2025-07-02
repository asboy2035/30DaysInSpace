// Libraries
#include "Arduino.h"
#include <Keypad.h>


// Hardware
// Our keypad has 4 rows, each with 4 columns.
const byte ROWS = 4;
const byte COLS = 4;

const byte ROW_PINS[ROWS] = { 5, 4, 3, 2 };
const byte COL_PINS[COLS] = { 6, 7, 8, 9 };

const char BUTTONS[ROWS][COLS] = {
  { '1', '2', '3', 'A' },  // Row 0
  { '4', '5', '6', 'B' },  // Row 1
  { '7', '8', '9', 'C' },  // Row 2
  { '*', '0', '#', 'D' }   // Row 3
};

const byte MAX_WORD_LENGTH = 4;

Keypad heroKeypad = Keypad(makeKeymap(BUTTONS), ROW_PINS, COL_PINS, ROWS, COLS);


// Functions
char* readWord(
  char word[],
  byte maxLen
) {
  char c = '\0';
  byte i = 0;

  do {
    c = heroKeypad.waitForKey();
    word[i] = c;
    i += 1;
  } while (c != '#' && i < maxLen);

  int lastIndex;

  if (word[i - 1] == '#') {
    lastIndex = i - 1;
  } else {
    lastIndex = i;
  }

  word[lastIndex] = '\0';
  return word;
}


// Main
void setup() {
  Serial.begin(9600);  // Initialize the serial monitor
}

void loop() {
  char word[MAX_WORD_LENGTH];
  readWord(word, MAX_WORD_LENGTH);
  // heroKeypad.waitForKey() will wait here until any button is pressed and
  // returns the character we defined in our BUTTONS array.
  // char pressedButton = heroKeypad.waitForKey();  // Wait until a button is pressed

  // Display the character returned for the button that was pressed.
  Serial.println(word);
}
