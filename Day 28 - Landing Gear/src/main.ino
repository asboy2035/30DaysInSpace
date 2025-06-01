#include "Arduino.h"
#include <U8g2lib.h>  // OLED Display library
#include <TM1637Display.h> // 7-seg display library
#include <Keypad.h>  // 4x4 button matrix keypad library
#include "landingGearBitmaps.h"


// Pins
constexpr byte BITMAP_NUMBER_DISPLAY_DIO = 2;
constexpr byte BITMAP_NUMBER_DISPLAY_CLK = 3;

constexpr byte CONFIRM_LEVER = A2;
constexpr byte SYSTEMS_LEVER = A1;
constexpr byte THRUST_LEVER = A0;


// Displays
// Use _2_ version of constructor and firstPage()/nextPage() for OLED
U8G2_SH1106_128X64_NONAME_2_HW_I2C landerDisplay(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

// Construct bitmap_number_display handle.
TM1637Display bitmapNumberDisplay(BITMAP_NUMBER_DISPLAY_CLK, BITMAP_NUMBER_DISPLAY_DIO);

// Create an array of pointers to each of the bitmap images.
const static unsigned char* SWITCH_BITMAPS[] = {
  LANDING_1,
  LANDING_2,
  LANDING_3,
  LANDING_4,
};
constexpr int GEAR_BITMAP_COUNT = sizeof(SWITCH_BITMAPS) / sizeof(SWITCH_BITMAPS[0]);


// Keypad (4x4 Button Matrix)
constexpr byte ROWS = 4;
constexpr byte COLS = 4;

byte colPins[ROWS] = { 10, 11, 12, 13 };
byte rowPins[COLS] = { 9, 8, 7, 6 };

char buttons[ROWS][COLS] = {
  { '1', '2', '3', 'A' },  // 1st row
  { '4', '5', '6', 'B' },  // 2nd row
  { '7', '8', '9', 'C' },  // 3rd row
  { '*', '0', '#', 'D' }   // 4th row
};

// Initialize keypad
auto controlPad = Keypad(makeKeymap(buttons), rowPins, colPins, ROWS, COLS);


// States
enum APPROACH_STATE {
  APPROACH_INIT, // Ensure all switches are off to begin
  APPROACH_PREFLIGHT, // Some switches on
  APPROACH_FINAL, // Lower landing gear!
};

enum GEAR_STATE {
  GEAR_IDLE = 0,
  GEAR_LOWERING = 1,
  GEAR_RAISING = -1
};


// Function Hoisting
byte drawString(
  byte x, byte y,
  const char* string
);

String liftoffStateToString(enum APPROACH_STATE approach_state);

void preflightDisplay(
  enum APPROACH_STATE approach_state,
  bool thruster_lever,
  bool systems_lever,
  bool confirm_lever
);

void finalDisplay(int current_gear_bitmap);

GEAR_STATE processLandingGear(
  char customKey,
  GEAR_STATE gear_state,
  int current_gear_bitmap
);

void processState(
  bool thrust_lever,
  bool systems_lever,
  bool confirm_lever
);

const char* showStatus(
  String title,
  bool status
);

String onOff(bool val);

byte displayLeverSetting(
  const String& leverName,
  bool leverVal,
  byte yOffset
);


void setup() {
  Serial.begin(9600);

  // Configure counter display
  bitmapNumberDisplay.setBrightness(7);  // Set maximum brightness (value is 0-7)
  bitmapNumberDisplay.clear();           // Clear the display

  // Configure DIP switch pins
  pinMode(CONFIRM_LEVER, INPUT);
  pinMode(SYSTEMS_LEVER, INPUT);
  pinMode(THRUST_LEVER, INPUT);

  landerDisplay.begin();                     // initialize lander display
  landerDisplay.setFont(u8g2_font_6x10_tr);  // Set text font
  landerDisplay.setFontRefHeightText();      // Define how max text height is calculated
  landerDisplay.setFontPosTop();             // Y coordinate for text is at top of tallest character
}

static APPROACH_STATE approach_state = APPROACH_INIT;
static int current_gear_bitmap = 0; // Start with image of lander with gear up
static char last_key = -1; // key previously seen
static GEAR_STATE gear_state = GEAR_IDLE;

void loop() {
  const bool thrust_lever = digitalRead(THRUST_LEVER);
  const bool systems_lever = digitalRead(SYSTEMS_LEVER);
  const bool confirm_lever = digitalRead(CONFIRM_LEVER);

  Serial.println(approach_state);
  Serial.print("  Switches: ");
  Serial.print(thrust_lever);
  Serial.print(", ");
  Serial.print(systems_lever);
  Serial.print(", ");
  Serial.println(confirm_lever);

  processState(thrust_lever, systems_lever, confirm_lever);

  // Display selected bitmap on our OLED lander display
  landerDisplay.firstPage();
  do {
    switch (approach_state) {
      case APPROACH_INIT:
      case APPROACH_PREFLIGHT:
        preflightDisplay(approach_state, thrust_lever, systems_lever, confirm_lever);
        break;
      case APPROACH_FINAL:
        finalDisplay(current_gear_bitmap);
        break;
    }

  } while (landerDisplay.nextPage());

  delay(100);
}


void processState(
  const bool thrust_lever,
  const bool systems_lever,
  const bool confirm_lever
) {
  switch (approach_state) {
    case APPROACH_INIT:
      Serial.println("case APPROACH_INIT");
      // All levers off
      if (!thrust_lever && !systems_lever && !confirm_lever) {
        approach_state = APPROACH_PREFLIGHT;
      }
      break;

    case APPROACH_PREFLIGHT:
      Serial.println("case APPROACH_PREFLIGHT");
      // All levers on
      if (thrust_lever && systems_lever && confirm_lever) {
        approach_state = APPROACH_FINAL;
      }
      break;

    case APPROACH_FINAL:
      Serial.println("case APPROACH_FINAL");
      const char customKey = controlPad.getKey();
      if (customKey && customKey != last_key) {
        Serial.println(customKey);
        last_key = customKey;
      }

      gear_state = processLandingGear(customKey, gear_state, current_gear_bitmap);
      break;
  }

  current_gear_bitmap += gear_state;

  if (current_gear_bitmap == 0 || current_gear_bitmap == GEAR_BITMAP_COUNT - 1) {
    gear_state = GEAR_IDLE;
  }

  Serial.print("Gear: ");
  Serial.println(current_gear_bitmap);
  Serial.println(gear_state);
}

GEAR_STATE processLandingGear(
  char customKey,
  GEAR_STATE gear_state,
  int current_gear_bitmap
) {
  switch (customKey) {
    case 'A':  // Lower landing gear unless already lowered
      if (current_gear_bitmap != GEAR_BITMAP_COUNT - 1) {
        gear_state = GEAR_LOWERING;
      }
      break;
    case 'B':  // Raise landing gear unless already raised
      if (current_gear_bitmap != 0) {
        gear_state = GEAR_RAISING;
      }
  }

  return gear_state;
}

void preflightDisplay(
  APPROACH_STATE approach_state,
  bool thruster_lever,
  bool systems_lever,
  bool confirm_lever
) {
  landerDisplay.setFontPosTop();

  // Display headers
  byte y_offset = drawString(0, 0, "Exploration Lander");
  drawString(0, y_offset, "Approach Sequence");

  // Set y_offset to point four lines above bottom of display
  y_offset = landerDisplay.getDisplayHeight() - (4 * landerDisplay.getMaxCharHeight());

  // Display statuses
  y_offset = displayLeverSetting(
    "Thrusters: ",
    thruster_lever,
    y_offset
  );

  y_offset = displayLeverSetting(
    "Systems  : ",
    systems_lever,
    y_offset
  );

  y_offset = displayLeverSetting(
    "Confirm  : ",
    confirm_lever,
    y_offset
  );

  // Display countdown state
  drawString(
    0, y_offset,
    (String("Countdown ") + liftoffStateToString(approach_state)).c_str()
  );
}

void finalDisplay(
  int current_gear_bitmap
) {
  // Flash lander bitmap if gear isn't down.
  // Display current gear bitmap as gear lowers.
  // .drawXBMP() displays each bitmap centered in the display based
  // on it's size.

  // Calculate our x and y offsets for our bitmap graphics
  byte x_offset = (landerDisplay.getDisplayWidth() - LANDING_GEAR_BITMAP_WIDTH) / 2;
  byte y_offset = (landerDisplay.getDisplayHeight() - LANDING_GEAR_BITMAP_HEIGHT) / 2;

  landerDisplay.drawXBMP(
    x_offset, y_offset,
    LANDING_GEAR_BITMAP_WIDTH, LANDING_GEAR_BITMAP_HEIGHT,
    SWITCH_BITMAPS[current_gear_bitmap]
  );
}

// Return a different string for each enum state.
String liftoffStateToString(enum APPROACH_STATE approach_state) {
  switch (approach_state) {
    case APPROACH_INIT:
      return ("Init");
    case APPROACH_PREFLIGHT:
      return ("Preflight");
    case APPROACH_FINAL:
      return("Final");
  }

  return "";
}

// Draw a line of text on our OLED display at x, y, returning new y
byte drawString(
  byte x, byte y,
  const char* string
) {
  landerDisplay.drawStr(x, y, string);
  return (y + landerDisplay.getMaxCharHeight());  // return new y_offset on display
}

// Display a lever setting with a title and state.
byte displayLeverSetting(
  const String& leverName,
  bool leverVal,
  byte yOffset
) {
  return drawString(
    0,
    yOffset,
    (leverName + onOff(leverVal)).c_str()
  );
}

// Convert bool to on/off
String onOff(bool val) {
  return val ? "ON" : "OFF";
}
