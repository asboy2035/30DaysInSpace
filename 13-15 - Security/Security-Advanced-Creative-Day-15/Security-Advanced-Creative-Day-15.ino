/*
 * 30 Days - Lost in Space
 * Day 14 - Advanced Security Panel
 *
 * Learn more at https://learn.inventr.io/adventure
 *
 * Time to enhance our security system, taking it from functional to fabulous.  We'll
 * add in our RGB LED to give us visual feedback and also expand the tones that play
 * to give us audible feedback as you enter your PIN.
 *
 * IMPORTANT NOTE: The wiring for this sketch builds on the Day 13 wiring. But the
 *                 addition of the RGB LED requires us to MOVE some of the Day 13
 *                 wires.  This is because the RGB LED only works when connected to
 *                 pins that support Pulse Width Modulation (PWM).  HERO pins that
 *                 support PWM have a tilde ('~') preceeding their pin numbers.
 *
 *                 If you look at the Day 13 diagrams you see that we used PWM pins
 *                 for the keypad and buzzer, leaving only one available.  Pins 12
 *                 and 13 are still available so we'll move the wires on pins 9 and 10
 *                 (PWM pins) to 12 and 13.
 *
 *                 Pin 13 is also used while the Arduino IDE is uploading sketches, and
 *                 if you connect the buzzer to THAT pin you get noise every time you
 *                 upload a new sketch (try it!).  So the buzzer will now be on pin 12
 *                 and the keypad wire on pin 9 will move to pin 13.
 *
 *                 Because we used constants, we only need to make two small changes
 *                 from the Day 13 sketch instead of hunting down and changing each use.
 *
 * Alex Eschenauer
 * David Schmidt
 * Greg Lyzenga
 */

/*
 * Arduino concepts introduced/documented in this lesson.
 * - complex logic with multiple devices
 *
 * Parts and electronics concepts introduced in this lesson.
 * - Changing wiring when particular pins (PWM) are required for added components.
 * - Tone() and PWM analogWrite() cannot both operate at the same time.
 */

// Explicitly include Arduino.h
#include "Arduino.h"
#include <Keypad.h>

// Our HERO keypad has 4 rows, each with 4 columns.
const byte ROWS = 4;
const byte COLS = 4;

const byte PIN_LENGTH = 4;                           // PIN code is 4 button presses
char password[PIN_LENGTH + 1] = { '0', '0', '0', '0', 0 };  // Initial password is four zeros.

// Define what characters will be returned by each button
const char BUTTONS[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

// Define row and column pins connected to the keypad
const byte ROW_PINS[ROWS] = { 5, 4, 3, 2 };
const byte COL_PINS[COLS] = { 6, 7, 12, 13 };  // NOTE wire moved from Day 13's sketch to pin 13

// Create our keypad object from the keypad configuration above
Keypad heroKeypad = Keypad(makeKeymap(BUTTONS), ROW_PINS, COL_PINS, ROWS, COLS);

const byte BUZZER_PIN = 8;  // NOTE that pin 12 drives the buzzer now

const byte RED_PIN = 11;    // PWM pin controlling the red leg of our RGB LED
const byte GREEN_PIN = 10;  // PWM pin ccontrolling the green leg of our RGB LED
const byte BLUE_PIN = 9;    // PWM pin ccontrolling the blue leg of our RGB LED

// unpacked -> Packed colors 
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

unsigned long AllColors[] = {
  RED,
  ORANGE,
  YELLOW,
  GREEN,
  BLUE,
  PURPLE
};

// Settings
typedef struct {
  char* password;
  bool showPin;
  unsigned long homeColor;
} Settings;

Settings currentSettings = {
  .password = password,
  .showPin = 0,
  .homeColor = GREEN
};

Settings defaultSettings = {
  .password = password,
  .showPin = 0,
  .homeColor = GREEN
};

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // Since we start out "locked", we initially display a red color.
  showColor(RED);

  Serial.begin(9600);  // Begin monitoring via the serial monitor
}

void loop() {
  delay(1500);
  fillSerial(15);
  Serial.println("* - Settings        # - Unlock");
  char button_character = heroKeypad.waitForKey();  // Wait for a button press and save character

  // The '#' button unlocks our console
  if (button_character == '#') {          // Button to access system
    giveInputFeedback();                  // Short beep with blue light
    bool access_allowed = validatePIN(currentSettings.password);  // Have user enter PIN to allow access

    if (access_allowed) {
      Serial.println("Welcome, authorized user. You may now begin using the system.");
    } else {
      Serial.println("Access Denied.");
      Serial.println("* - Settings        # - Unlock");
    }
  }

  // Settings
  if (button_character == '*') {
    openSettings();
  }
}

void fillSerial(
  byte lines
) {
  for (int i = 0; i < lines; i++) {
    Serial.println();
  }
}

void showPasswordDigit(
  char digit
) {
  if (currentSettings.showPin) {
    Serial.print(digit);
  } else {
    Serial.print('*');
  }
}

// Settings functions
void openSettings() {
  fillSerial(15);
  int isAdmin = validatePIN(currentSettings.password);
  if (!isAdmin) {
    return;
  }

  fillSerial(15);
  giveInputFeedback();

  Serial.println("Settings v1");
  Serial.println("1 - Change Password");
  Serial.println("2 - Toggle show password");
  Serial.println("3 - Set custom home color");

  Serial.println();
  Serial.println("# - Back");
  Serial.println("0 - Restore settings");

  char button_character = heroKeypad.waitForKey();
  fillSerial(15);
  switch (button_character) {
    case '1': {
      changePassword();
      break;
    }

    case '2': {
      toggleShowPassword();
      break;
    }

    case '3': {
      setHomeColor();
      break;
    }

    case '#': {
      break;
    }

    case '0': {
      restoreSettings();
      break;
    }
  }
}

void changePassword() {
  giveInputFeedback();
  bool access_allowed = validatePIN(password);  // Have user enter PIN to allow access

  if (access_allowed) {
    showColor(YELLOW);  // Display yellow LED while entering a NEW PIN
    Serial.println("Welcome. Enter a new password: ");

    // Use a for() loop that runs once for each PIN character.  Each character read
    // replaces a character in our original PIN.
    for (int i = 0; i < PIN_LENGTH; i++) {
      password[i] = heroKeypad.waitForKey();  // replace PIN character with new character
      // NOTE: After the LAST button press we will give Success feedback, but up to the
      //       last character we simply give input feedback.  Since Array indices start
      //       at 0, this if() statement will be true for all but the LAST character of
      //       the new PIN.
      if (i < (PIN_LENGTH - 1)) {
        giveInputFeedback();
        showColor(YELLOW);  // override color
      }

      showPasswordDigit(password[i]);
    }

    Serial.println();  // add new line after last asterisk so next message is on next line
    Serial.println("PIN Changed.");
    giveSuccessFeedback();  // TADA sound and green light for successful PIN change
  } else {
    Serial.println("-01 Access Denied.");
    Serial.println("* - Settings        # - Unlock");
  }
}

void toggleShowPassword() {
  currentSettings.showPin = !currentSettings.showPin;
  Serial.println("Toggled show password.");
}

void setHomeColor() {
  Serial.println("Change Home Color...");
  Serial.println("1 - Red");
  Serial.println("2 - Orange");
  Serial.println("3 - Yellow");
  Serial.println("4 - Green");
  Serial.println("5 - Blue");
  Serial.println("6 - Purple");

  char button_character = heroKeypad.waitForKey();
  currentSettings.homeColor = AllColors[(button_character - '0') - 1];
  Serial.print("Chose: ");
  Serial.println(button_character);
}

void restoreSettings() {
  currentSettings.password = defaultSettings.password;
  currentSettings.homeColor = defaultSettings.homeColor;
  currentSettings.showPin = defaultSettings.showPin;

  Serial.println("Settings restored.");
}

// Enter PIN and return false for bad PIN or true for good PIN
bool validatePIN(
  char* password
) {
  Serial.println("Enter PIN to continue.");
  char pin[PIN_LENGTH + 1];

  for (int i = 0; i < PIN_LENGTH; i++) {
    char button_character = heroKeypad.waitForKey();
    pin[i] = button_character;
    giveInputFeedback();
    showPasswordDigit(button_character);
  }
  pin[PIN_LENGTH] = 0;

  int isValidated = strcmp(password, pin);
  if (isValidated == 0) {
    giveSuccessFeedback();  // PIN matched - TADA! sound with green LED
    Serial.println();       // add new line after last asterisk so next message is on next line
    Serial.println("Device Unlocked");
    return true;
  }

  Serial.print("-01 Access Denied.");
  giveErrorFeedback();  // Error sound and red light
  return false;
}

/*
 * NOTE:
 * Both the PWM functions and tone() functions use some of the same HERO hardware
 * for their functions.  If the RGB LED is on while a tone is played it will flicker
 * or diplay other colors.  Because of this, we turn off the LED while a tone is
 * being played and restore it immediately afterwards.
 */

// Feedback
void giveFeedback(
  unsigned long color,
  int note
) {
  showColor(NONE);
  delay(100);
  showColor(color);
  tone(BUZZER_PIN, note, 250);
}

// A recognized button was pressed.  Give short beep and blue LED
void giveInputFeedback() {
  giveFeedback(BLUE, 659);
  // showColor(NONE);  // Turn off LED while playing tone
  // tone(BUZZER_PIN, 659, 300);
  // delay(200);               // Delay while tone is playing because tone() returns immediately
  // showColor(BLUE);
}

// A matching PIN has been entered or a new PIN has been accepted.
// Play TADA! sound and display green LED
void giveSuccessFeedback() {
  giveFeedback(currentSettings.homeColor, 783);
  
  // showColor(NONE);  // Turn off LED while playing tone
  // tone(BUZZER_PIN, 523, 240);
  // delay(250);
  // tone(BUZZER_PIN, 783, 150);
  // delay(500);  // Delay while tone is playing because tone() returns immediately
  // showColor(GREEN);
}

// Bad PIN entered.  Play descending tone and display red LED
void giveErrorFeedback() {
  giveFeedback(RED, 523);

  // showColor(NONE);
  // tone(BUZZER_PIN, 587, 150);
  // delay(225);
  // tone(BUZZER_PIN, 523, 300);
  // delay(500);  // Delay while tone is playing because tone() returns immediately
  // showColor(RED);
}

// Color display
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
 