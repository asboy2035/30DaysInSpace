//
// Created by ash on 6/15/25.
//

#include "LanderHardware.h"

// Hardware object definitions
U8G2_SH1106_128X64_NONAME_2_HW_I2C landerDisplay(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
TM1637Display distanceDisplay(DISTANCE_DISPLAY_CLK, DISTANCE_DISPLAY_DIO);

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

// Static member initialization
LANDER_CONTROLS LanderHardware::lastKey = UNUSED;

void LanderHardware::init() {
    // Configure OLED display
    landerDisplay.begin();
    landerDisplay.setFont(u8g2_font_6x10_tr);
    landerDisplay.setFontRefHeightText();
    landerDisplay.setFontPosTop();

    // Configure counter display
    distanceDisplay.setBrightness(7);
    distanceDisplay.clear();

    // Configure DIP switch pins
    pinMode(CONFIRM_LEVER, INPUT);
    pinMode(SYSTEMS_LEVER, INPUT);
    pinMode(THRUST_LEVER, INPUT);
}

void LanderHardware::setDisplayBrightness(int brightness) {
    distanceDisplay.setBrightness(brightness);
}

void LanderHardware::clearDistanceDisplay() {
    distanceDisplay.clear();
}

void LanderHardware::showDistance(int distance) {
    distanceDisplay.showNumberDec(distance);
}

bool LanderHardware::getThrustLever() {
    return digitalRead(THRUST_LEVER);
}

bool LanderHardware::getSystemsLever() {
    return digitalRead(SYSTEMS_LEVER);
}

bool LanderHardware::getConfirmLever() {
    return digitalRead(CONFIRM_LEVER);
}

LANDER_CONTROLS LanderHardware::getControlButtonPressed() {
    char current_key = lander_controls.getKey();

    if (current_key != NO_KEY) {
        lastKey = static_cast<LANDER_CONTROLS>(current_key);
    }

    // If the button is released reset to NO_KEY
    if (lander_controls.getState() == RELEASED) {
        lastKey = UNUSED;
        current_key = NO_KEY;
    } else {
        current_key = static_cast<char>(lastKey);
    }

    return static_cast<LANDER_CONTROLS>(current_key);
}

int LanderHardware::getRandomDrift() {
    int drift = static_cast<int>(random(-1, DRIFT_CONTROL));

    if (drift > 1) {
        drift = 0;
    }

    return drift;
}

void LanderHardware::seedRandom() {
    randomSeed(analogRead(A3));
}
