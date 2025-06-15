// Libraries
#include "Arduino.h"
#include <U8g2lib.h> // OLED display
#include <TM1637Display.h> // 7-seg display
#include <Keypad.h>  // 4x4 button matrix

// Images
#include "radarArrows.h"
#include "smallLandingGearBitmaps.h"
#include "endingBitmaps.h"


// Pins
constexpr byte DISTANCE_DISPLAY_DIO = 4;
constexpr byte DISTANCE_DISPLAY_CLK = 5;

constexpr byte CONFIRM_LEVER = A2;
constexpr byte SYSTEMS_LEVER = A1;
constexpr byte THRUST_LEVER = A0;


// Hardware
// Use _2_ version of constructor and firstPage()/nextPage() for OLED
// graphics to save memory.
U8G2_SH1106_128X64_NONAME_2_HW_I2C landerDisplay(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

// Initialize the 7-segment display.
TM1637Display distanceDisplay(DISTANCE_DISPLAY_CLK, DISTANCE_DISPLAY_DIO);


// States and Enums
const static unsigned char* GEAR_BITMAPS[] = {
  LANDING_GEAR_1,  // Gear up
  LANDING_GEAR_2,  // Gear lowering
  LANDING_GEAR_3,  // Gear lowering
  LANDING_GEAR_4,  // Gear down
};

constexpr int GEAR_BITMAP_COUNT = sizeof(GEAR_BITMAPS) / sizeof(GEAR_BITMAPS[0]);

// Gear states with defined values used to change bitmap index.
enum GEAR_STATE {
  GEAR_IDLE = 0,      // Landing gear idle.  Don't change index when added to current
  GEAR_LOWERING = 1,  // Lowering gear, adds one to index for next "lower" bitmap
  GEAR_RAISING = -1   // Raising gear, subtracts one from index, showing gear retracting
};

// The Keypad library isn't restricted to just returning numbers or letters.
// Since we will be using the button matrix to control our lander we define
// an enum with our commands and then set up the buttons to return those
// values.
enum LANDER_CONTROLS {
  UNUSED,
  STEER_UP,
  STEER_DOWN,
  STEER_LEFT,
  STEER_RIGHT,
  STEER_UP_RIGHT,
  STEER_UP_LEFT,
  STEER_DOWN_RIGHT,
  STEER_DOWN_LEFT,
  LOWER_GEAR,
  RAISE_GEAR,
  RAISE_SPEED,
  LOWER_SPEED,
};

constexpr byte CONTROL_ROW_COUNT = 4;
constexpr byte CONTROL_COLUMN_COUNT = 4;

constexpr byte COLUMN_PINS[CONTROL_COLUMN_COUNT] = { 10, 11, 12, 13 };
constexpr byte ROW_PINS[4] = {9, 8, 7, 6};

// Define our button array using constants to be returned for each button
char control_buttons[CONTROL_ROW_COUNT][CONTROL_COLUMN_COUNT] = {
  { STEER_UP_LEFT, STEER_UP, STEER_UP_RIGHT, LOWER_GEAR },         // 1st row
  { STEER_LEFT, UNUSED, STEER_RIGHT, RAISE_GEAR },                 // 2nd row
  { STEER_DOWN_LEFT, STEER_DOWN, STEER_DOWN_RIGHT, RAISE_SPEED },  // 3rd row
  { UNUSED, UNUSED, UNUSED, LOWER_SPEED },                         // 4th row
};

// Create lander button control object.
auto lander_controls = Keypad(
  makeKeymap(control_buttons),
  const_cast<byte*>(ROW_PINS),
  const_cast<byte*>(COLUMN_PINS),
  CONTROL_ROW_COUNT,
  CONTROL_COLUMN_COUNT
);

// Here are the states our code will run through to perform the actual
// approach flight and landing in the mother ship bay.
enum APPROACH_STATE {
  APPROACH_INIT,       // Ensure all switches are off to begin
  APPROACH_PREFLIGHT,  // Wait for all switches to be enabled
  APPROACH_IN_FLIGHT,  // Begin to approach mother ship
  APPROACH_FINAL       // Lower landing gear!
};

constexpr int INITIAL_DISTANCE = 1476;  // Distance to mother ship

constexpr byte MAX_MOTHER_SHIP_WIDTH = 21;
constexpr byte MAX_MOTHER_SHIP_HEIGHT = 15;


// Function hoisting
String liftoffStateToString(enum APPROACH_STATE approach_state);

bool processGearState(
  LANDER_CONTROLS action
);

bool processSpeedState(
  LANDER_CONTROLS action
);

void processSteeringState(
  LANDER_CONTROLS action
);

void displayInFlight(
  int lander_distance,
  int lander_speed,
  int mother_ship_x_offset,
  int mother_ship_y_offset
);

void displayFinal(
  int current_gear_bitmap_index
);

byte drawString(
  byte x, byte y,
  const char* string
);

LANDER_CONTROLS controlButtonPressed();

int getRandomDrift();

void displayPreFlight(
  APPROACH_STATE approach_state,
  bool thrusterLever,
  bool systemsLever,
  bool confirmLever
);

byte displayLeverSetting(
  const String& leverName,
  bool leverVal,
  byte yOffset
);

String onOff(bool val);

const unsigned char* getEndingBitmap();

void processInflightState();


// Main
void setup() {
  Serial.begin(9600);

  randomSeed(analogRead(A3));

  // Configure OLED display
  landerDisplay.begin();                     // Initialize lander display
  landerDisplay.setFont(u8g2_font_6x10_tr);  // Set text font
  landerDisplay.setFontRefHeightText();      // Define how max text height is calculated
  landerDisplay.setFontPosTop();             // Y coordinate for text is at top of tallest character

  // Configure counter display
  distanceDisplay.setBrightness(7);  // Set maximum brightness (value is 0-7)
  distanceDisplay.clear();           // Clear the display

  // Configure DIP switch pins
  pinMode(CONFIRM_LEVER, INPUT);
  pinMode(SYSTEMS_LEVER, INPUT);
  pinMode(THRUST_LEVER, INPUT);
}

unsigned long approachStartTime = 0;    // Time thrusters are first fired
int current_gear_bitmap_index = 0;      // Image of lander with gear up
constexpr byte DRIFT_CONTROL = 3;       // Must be > 1.  Higher numbers slow drift rate.

APPROACH_STATE approach_state = APPROACH_INIT;     // Initial approach state
GEAR_STATE gear_state = GEAR_IDLE;                 // Initial landing gear state

int lander_distance = INITIAL_DISTANCE;
int lander_speed = 0;  // Initial lander speed relative to mother ship

// These will track the "drift" of the mother ship from center of radar
int mother_ship_x_offset = 0;
int mother_ship_y_offset = 0;

[[noreturn]] void loop() {
  // Read current values of all of our switches as booleans ("on" is true, "off" is false)
  const bool thrust_lever = digitalRead(THRUST_LEVER);
  const bool systems_lever = digitalRead(SYSTEMS_LEVER);
  const bool confirm_lever = digitalRead(CONFIRM_LEVER);

  /*
   * Primary control state machine.
   *
   * Based on the current state of our approach to the lander, handle each
   * state's decisions, changing state when appropriate.
   */
  switch (approach_state) {
    case APPROACH_INIT: {
      Serial.println("case APPROACH_INIT");
      // All levers off
      if (!thrust_lever && !systems_lever && !confirm_lever) {
        approach_state = APPROACH_PREFLIGHT;
      }
      break;
    }

    case APPROACH_PREFLIGHT: {
      Serial.println("case APPROACH_PREFLIGHT");
      // All levers on
      if (thrust_lever && systems_lever && confirm_lever) {
        approach_state = APPROACH_IN_FLIGHT;
      }
      break;
    }

    case APPROACH_FINAL:
      Serial.println("case APPROACH_FINAL");
      // Near to mothership, lower gear
      // Check to see if a gear change button is pressed. Do NOT wait for a key.
      // NOTE - code then drops into in-flight state!!

    // IN-FLIGHT state - Add thrust to move closer to mother ship.
    // (Remember, you will have to REDUCE thrust as you get closer)
    case APPROACH_IN_FLIGHT: {
      processInflightState();

      // Here we compute the drift of the mother ship using random numbers.
      // The mother ship cannot drift off the display, done by setting a
      // maximum drift.
      constexpr byte MAX_DRIFT = 18;

      mother_ship_x_offset += getRandomDrift();  // returns -1, 0 or 1
      mother_ship_y_offset += getRandomDrift();  // returns -1, 0 or 1
      // Ensure mother ship doesn't drift off our radar display
      if (mother_ship_x_offset > MAX_DRIFT) mother_ship_x_offset = MAX_DRIFT;
      if (mother_ship_x_offset < -MAX_DRIFT) mother_ship_x_offset = -MAX_DRIFT;
      if (mother_ship_y_offset > MAX_DRIFT) mother_ship_y_offset = MAX_DRIFT;
      if (mother_ship_y_offset < -MAX_DRIFT) mother_ship_y_offset = -MAX_DRIFT;

      // Prepare for landing on final approach
      if (lander_distance < (INITIAL_DISTANCE / 10)) {
        approach_state = APPROACH_FINAL;  // on final approach - enable gear and warn
      }
      break;
    } // case APPROACH_IN_FLIGHT
  } // switch (approach_state)

  // Because we specified our gear states as 0, 1 or -1 we can change bitmaps by
  // simply adding the gear state to our current gear bitmap index.
  current_gear_bitmap_index += gear_state;

  // If the gear animation has completed (index is either 0 or the index of our last bitmap)
  // then we change the gear state to IDLE to complete animation.
  if (current_gear_bitmap_index == 0 || current_gear_bitmap_index == GEAR_BITMAP_COUNT - 1) {
    gear_state = GEAR_IDLE;
  }

  // Update our lander display (OLED) using firstPage()/nextPage() methods which
  // use a smaller buffer to save memory.  Draw the exact SAME display each time
  // through this loop!
  landerDisplay.firstPage();
  do {
    switch (approach_state) {
      // Display switch status for INIT and PREFLIGHT states.
      case APPROACH_INIT:
      case APPROACH_PREFLIGHT:
        displayPreFlight(approach_state, thrust_lever, systems_lever, confirm_lever);
        break;

      case APPROACH_FINAL:
        displayFinal(current_gear_bitmap_index);

      case APPROACH_IN_FLIGHT:
        displayInFlight(lander_distance, lander_speed, mother_ship_x_offset, mother_ship_y_offset);
        break;
    }
  } while (landerDisplay.nextPage());

  lander_distance -= lander_speed;  // Adjust distance by current speed

  // Determines outcome image
  if (lander_distance <= 0) {
    // Check to see if lander landed inside the mother ship.
    const unsigned char* endingBitmap = getEndingBitmap();

    // Show final distance
    distanceDisplay.showNumberDec(0);

    // Calculate elapsed time (in ms) from first thrust.
    const unsigned long elapsed_time = millis() - approachStartTime;

    // Now format to fractional seconds (SS.SSS) using sprintf
    char buffer[20];  // buffer long enough for final display line.

    // Since elapse time is a long integer we can get the number of seconds
    // by simply dividing by 1000 and the fractional portion is dropped.  The
    // Modulo operator ('%') returns the REMAINDER after dividing left side by
    // the value on the right.  This gives us the fractional number seconds.
    // Sprintf() uses "lu" to indicate that the value is an "unsigned long"
    // ("lu" does NOT work!).
    sprintf(buffer, "%4lu.%03lu Sec", elapsed_time / 1000, elapsed_time % 1000);

    // Final display.  Alternate between splash screen with time and
    // final radar view.
    do {
      landerDisplay.firstPage();
      do {
        landerDisplay.drawStr(0, 0, buffer);
        landerDisplay.drawXBMP(0, 10, ENDING_BITMAP_WIDTH, ENDING_BITMAP_HEIGHT, endingBitmap);
      } while (landerDisplay.nextPage());

      delay(2000);

      landerDisplay.firstPage();
      do {
        displayFinal(current_gear_bitmap_index);
        displayInFlight(lander_distance, lander_speed, mother_ship_x_offset, mother_ship_y_offset);
      } while (landerDisplay.nextPage());

      delay(2000);
    } while (true);
  }

  delay(100);  // Delay before next loop
}


// Functions
// Ending bitmap
const unsigned char* getEndingBitmap() {
  const bool missedMotherShip = !(
    abs(mother_ship_x_offset) < ((MAX_MOTHER_SHIP_WIDTH + 1) / 2) &&
    abs(mother_ship_y_offset) < ((MAX_MOTHER_SHIP_HEIGHT + 1) / 2)
  );

  if (missedMotherShip) {
    // Missed the mother ship.  No fuel for another try.  Bye!
    return const_cast<unsigned char *>(ENDING_BITMAP_MISSED_MOTHER_SHIP);
  }

  lander_distance = 0;

  // Check speed to see if we were slow enough.
  if (lander_speed > 2) {
    // Max safe landing speed is 2
    // Speed is too fast! Lander AND mother ship destroyed. (Ouch!)
    return const_cast<unsigned char *>(ENDING_BITMAP_TOO_FAST);
  }

  // Did we remember to lower the landing gear?
  if (current_gear_bitmap_index == GEAR_BITMAP_COUNT - 1) {
    // Gear is down! Success.
    return const_cast<unsigned char *>(ENDING_BITMAP_SUCCESS);
  }

  // Gear is up; damage to lander, but we survived.
  return const_cast<unsigned char *>(ENDING_BITMAP_NO_GEAR);
}

// Preflight display
// Update lander display with the status of our switches for INIT and PREFLIGHT states
void displayPreFlight(
  const APPROACH_STATE approach_state,
  const bool thrusterLever,
  const bool systemsLever,
  const bool confirmLever
) {
  // Display all text referenced from upper left bit X, Y
  landerDisplay.setFontPosTop();

  // Draw title lines at top of display, updating y_offset afterward
  byte yOffset = drawString(0, 0, "Exploration Lander");
  drawString(0, yOffset, "Approach Sequence");

  // Set y_offset to point four lines above bottom of display
  yOffset = landerDisplay.getDisplayHeight() - (4 * landerDisplay.getMaxCharHeight());

  // Display status of each switch
  yOffset = displayLeverSetting("Thrusters: ", thrusterLever, yOffset);
  yOffset = displayLeverSetting("Systems  : ", systemsLever, yOffset);
  yOffset = displayLeverSetting("Confirm  : ", confirmLever, yOffset);

  // Display final status line
  drawString(0, yOffset, (String("Countdown ") + liftoffStateToString(approach_state)).c_str());
}


// Inflight state processing
void processInflightState() {
  const auto currentKey = controlButtonPressed();
  bool actionCompleted = processSpeedState(currentKey);

  if (actionCompleted) {
    Serial.println("Speed action");
    return;
  }

  actionCompleted = processGearState(currentKey);

  if (actionCompleted) {
    Serial.println("Gear action");
    return;
  }

  processSteeringState(currentKey);
}

bool processSpeedState(
  const LANDER_CONTROLS action
) {
  bool actionCompleted = false;

  switch (action) {
    case RAISE_SPEED:
      actionCompleted = true;
      lander_speed++;  // Increase velocity
      // If this is first time increasing speed then save the start time
      if (approachStartTime == 0) {
        approachStartTime = millis();
      }
      break;

    case LOWER_SPEED:
      actionCompleted = true;
      // lower speed unless stopped
      if (lander_speed > 0) {
        lander_speed--;
      }
      break;

    default: break;
  }

  return actionCompleted;
}

bool processGearState(
  const LANDER_CONTROLS action
) {
  bool actionCompleted = false;

  switch (action) {
    case LOWER_GEAR: {
      actionCompleted = true;
      // Lower landing gear unless already lowered
      if (approach_state == APPROACH_FINAL) {  // Only works on final approach
        // Lowering gear is an animation created by changing bitmaps each frame
        // until the gear is completely lowered.
        if (current_gear_bitmap_index != GEAR_BITMAP_COUNT - 1) {
          gear_state = GEAR_LOWERING;  // increases bitmap index until lowered
        }
      }
      break;
    }

    case RAISE_GEAR: {
      // Raise landing gear unless already raised
      // Raising gear is an animation created by changing the bitmap index
      // each frame until gear is up.
      if (current_gear_bitmap_index != 0) {  // Ignore if gear is already up
        gear_state = GEAR_RAISING;
      }
      break;
    }

    default:
      break;
  }

  return actionCompleted;
}

void processSteeringState(
  const LANDER_CONTROLS action
) {
  switch (action) {
    case STEER_UP:
      mother_ship_y_offset++;  // Steer lander one pixel UP
      break;
    case STEER_DOWN:
      mother_ship_y_offset--;  // Steer lander one pixel DOWN
      break;
    case STEER_LEFT:
      mother_ship_x_offset++;  // Steer lander one pixel LEFT
      break;
    case STEER_RIGHT:
      mother_ship_x_offset--;  // Steer lander one pixel RIGHT
      break;

    case STEER_UP_RIGHT:
      mother_ship_x_offset--;  // Steer lander one pixel UP and RIGHT
      mother_ship_y_offset++;
      break;
    case STEER_UP_LEFT:
      mother_ship_x_offset++;  // Steer lander one pixel UP and LEFT
      mother_ship_y_offset++;
      break;
    case STEER_DOWN_RIGHT:
      mother_ship_x_offset--;  // Steer lander one pixel DOWN and RIGHT
      mother_ship_y_offset--;
      break;
    case STEER_DOWN_LEFT:
      mother_ship_x_offset++;  // Steer lander one pixel DOWN and LEFT
      mother_ship_y_offset--;
      break;

    default: break;
  }
}

// In-Flight display (including final approach)
// Radar display for steering into mother ship docking bay.
void displayInFlight(
  const int lander_distance,
  const int lander_speed,
  const int mother_ship_x_offset,
  const int mother_ship_y_offset
) {
  Serial.println("Inflight display");
  constexpr byte ARROW_SIZE_X = 11;
  // Mother ship initially appears as a single dot, but expands into a rectangle
  // as we get closer.  Scaled based on the maximum width, from 1 to MAX.
  //
  // Divide distance to mother ship into segments.  The number of segments is
  // one less than the width of our mother ship.
  constexpr unsigned int SEGMENT_SIZE = INITIAL_DISTANCE / (MAX_MOTHER_SHIP_WIDTH - 1);
  const byte segment_number = lander_distance / SEGMENT_SIZE;  // from (width - 1) to 0

  // subtract segment number from width/height to get visible width (minimum 1)
  const int mother_ship_width = MAX_MOTHER_SHIP_WIDTH - segment_number;
  int mother_ship_height = MAX_MOTHER_SHIP_HEIGHT - segment_number;

  if (mother_ship_height < 1) {
    mother_ship_height = 1;  // Always at least 1 pixel high
  }

  // Display distance to mother ship on our distance display
  distanceDisplay.showNumberDec(lander_distance);

  // coordinates of the center of our radar display
  const byte RADAR_CENTER_X = (landerDisplay.getDisplayWidth() / 2 / 2);  // center of left half
  const byte RADAR_CENTER_Y = (landerDisplay.getDisplayHeight() / 2);     // Vertical center
  constexpr byte RADAR_RADIUS = 25;

  // Display bitmaps with 0 bits set to transparent.  This allows us to overlay
  // our arrows over the radar circle cleanly.
  landerDisplay.setBitmapMode(1);

  // Draw radar display circle and center pointer dot.
  landerDisplay.drawCircle(RADAR_CENTER_X, RADAR_CENTER_Y, RADAR_RADIUS);
  landerDisplay.drawPixel(RADAR_CENTER_X, RADAR_CENTER_Y);

  /*
   * As the mother ship's radar return drifts on the screen this section
   * of the code will display an arrow on the outside of the radar circle.
   *
   * The first constants can be changed so that the arrows don't appear
   * until the target pointer is outside the landing bay box, but currently
   * the arrows appear after small amounts of drift.
   *
   * The coordinates for the arrows were originally generated by ChatGPT
   * and then individually tweaked for perfect location.  Because of this
   * the coordinates will need to be manually adjusted if the radius of
   * the circle is changed.
   */
  constexpr int DRIFT_BEFORE_ARROW_X = 2;  // try (mother_ship_width / 2) - 2; for late arrows
  constexpr int DRIFT_BEFORE_ARROW_Y = 2;  // try (mother_ship_height / 2) - 2; for late arrows

  // Outer set of if/else statements check for left/right drift (X coordinate)
  if (mother_ship_x_offset < -DRIFT_BEFORE_ARROW_X) {  // Left of center
    // Inner set of if/else statements check for up/down drift (Y coordinate)

    if (mother_ship_y_offset < -DRIFT_BEFORE_ARROW_Y) {                             // above center
      landerDisplay.drawXBMP(9, 9, ARROW_SIZE_X, ARROW_SIZE_Y, ARROW_UP_LEFT);     // Up-Left
    } else if (mother_ship_y_offset > DRIFT_BEFORE_ARROW_Y) {                       // below center
      landerDisplay.drawXBMP(8, 45, ARROW_SIZE_X, ARROW_SIZE_Y, ARROW_DOWN_LEFT);  // Down-Left
    } else {                                                                        // close to center in Y direction
      landerDisplay.drawXBMP(1, 27, ARROW_SIZE_X, ARROW_SIZE_Y, ARROW_LEFT);       // Left
    }

  } else if (mother_ship_x_offset > DRIFT_BEFORE_ARROW_X) {                           // Right of center
    if (mother_ship_y_offset < -DRIFT_BEFORE_ARROW_Y) {                               // above center
      landerDisplay.drawXBMP(45, 8, ARROW_SIZE_X, ARROW_SIZE_Y, ARROW_UP_RIGHT);     // Up-Right
    } else if (mother_ship_y_offset > DRIFT_BEFORE_ARROW_Y) {                         // below center
      landerDisplay.drawXBMP(45, 45, ARROW_SIZE_X, ARROW_SIZE_Y, ARROW_DOWN_RIGHT);  // Down-Right
    } else {                                                                          // close to center in Y direction
      landerDisplay.drawXBMP(53, 27, ARROW_SIZE_X, ARROW_SIZE_Y, ARROW_RIGHT);       // Right
    }

  } else {                                                                      // close to center in X direction

    if (mother_ship_y_offset < -DRIFT_BEFORE_ARROW_Y) {                         // above center
      landerDisplay.drawXBMP(27, 1, ARROW_SIZE_X, ARROW_SIZE_Y, ARROW_UP);     // Up
    } else if (mother_ship_y_offset > DRIFT_BEFORE_ARROW_Y) {                   // below center
      landerDisplay.drawXBMP(27, 53, ARROW_SIZE_X, ARROW_SIZE_Y, ARROW_DOWN);  // Down
    }                                                                           // else NO arrow as we're close to target
  }

  // We will use sprintf() to format the lander speed line
  char buffer[9]; // Length of line plus one for terminating null.
  sprintf(buffer, "SPD: %2d", lander_speed); // format speed into buffer
  const u8g2_uint_t width = landerDisplay.getStrWidth(buffer); // width of string in pixels
  landerDisplay.drawStr(landerDisplay.getDisplayWidth() - width, 0, buffer); // display in upper right

  // Now draw the image of the mother ship as a rectangle, sized based on distance and
  // offset the x/y coordinates so that ship offset is always from the CENTER of the
  // mother ship.
  const byte x_offset = RADAR_CENTER_X + mother_ship_x_offset - (mother_ship_width / 2);
  const byte y_offset = RADAR_CENTER_Y + mother_ship_y_offset - (mother_ship_height / 2);
  landerDisplay.drawFrame(x_offset, y_offset, mother_ship_width, mother_ship_height);
}

// When lander is about to land we'll add an image of the lander
// with landing gear status along with gear prompt messages.
void displayFinal(
  const int current_gear_bitmap_index
) {
  Serial.println("displayFinal");
  constexpr int gear_down_index = GEAR_BITMAP_COUNT - 1;  // index of the LAST bitmap image

  // Coordinates below  manually adjusted to display in upper right
  // two lines below the speed line.
  byte x_offset = (landerDisplay.getDisplayWidth() / 2) + 11;
  byte y_offset = landerDisplay.getMaxCharHeight() * 2;  // offset to third line

  if (current_gear_bitmap_index == 0) {                   // Gear is up.  Prompt to lower
    landerDisplay.drawStr(x_offset, y_offset, "Drop gear");
  } else if (current_gear_bitmap_index < gear_down_index) {  // Gear is lowering
    landerDisplay.drawStr(x_offset, y_offset, "Lowering");
  } else {
    landerDisplay.drawStr(x_offset, y_offset, "Gear OK");  // Gear is down ()
  }

  // Calculate our x and y offsets to center our bitmap graphics
  // Get offsets to lower-right area of screen.  Height allows 3 lines of text above
  x_offset = (landerDisplay.getDisplayWidth() / 2);  // 64
  x_offset += ((landerDisplay.getDisplayWidth() - x_offset) - LANDING_GEAR_BITMAP_WIDTH) / 2;
  y_offset = landerDisplay.getDisplayHeight() - (landerDisplay.getMaxCharHeight() * 3);
  y_offset += ((landerDisplay.getDisplayHeight() - y_offset) - LANDING_GEAR_BITMAP_HEIGHT) / 2;

  // Draw current bitmap centered in lower right quadrant
  landerDisplay.drawXBMP(
    x_offset, y_offset,
    LANDING_GEAR_BITMAP_WIDTH, LANDING_GEAR_BITMAP_HEIGHT,
    GEAR_BITMAPS[current_gear_bitmap_index]
  );
}

// "helper" function that returns a different string for each enum state.
String liftoffStateToString(
  const APPROACH_STATE approach_state
) {
  switch (approach_state) {
    case APPROACH_INIT:
      return ("Init");
    case APPROACH_PREFLIGHT:
      return ("Preflight");

    default: return("");
  }
}

// Draw a line of text on our OLED display at x, y, returning new y
// value that is immediately below the new line of text.
byte drawString(
  const byte x, const byte y,
  const char* string
) {
  landerDisplay.drawStr(x, y, string);
  return (y + landerDisplay.getMaxCharHeight());  // return new y_offset on display
}

// Display a lever setting with a title and state.
byte displayLeverSetting(
  const String& leverName,
  const bool leverVal,
  const byte yOffset
) {
  return drawString(
    0,
    yOffset,
    (leverName + onOff(leverVal)).c_str()
  );
}

// Convert bool to on/off
String onOff(const bool val) {
  return val ? "ON" : "OFF";
}

// They Keypad library only returns a button value when it is initially
// pressed.  We wish to continue to get the same value for as long as
// a button is pressed.  This function keeps track of the last button
// pressed and returns it until the button is released or another
// button is pressed.
LANDER_CONTROLS controlButtonPressed() {
  static char last_key = NO_KEY;
  char current_key = lander_controls.getKey();

  if (current_key != NO_KEY) {
    last_key = current_key;  // New key seen, save as last
  }

  // If the button is released reset to NO_KEY
  if (lander_controls.getState() == RELEASED) {
    last_key = NO_KEY;
    current_key = NO_KEY;
  } else {
    current_key = last_key;  // Button still pressed so return last button seen.
  }

  return (current_key);
}

int getRandomDrift() {
  int drift = static_cast<int>(random(-1, DRIFT_CONTROL));

  if (drift > 1) {  // Values over 1 are changed to 0
    drift = 0;
  }

  return (drift);
}
