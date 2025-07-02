//
// Created by ash on 6/15/25.
//

#ifndef LANDER_HARDWARE_H
#define LANDER_HARDWARE_H

#include "Arduino.h"
#include <U8g2lib.h>
#include <TM1637Display.h>
#include <Keypad.h>
#include "LanderConfig.h"
#include "LanderTypes.h"

class LanderHardware {
public:
  // Hardware initialization
  static void init();

  // Display functions
  static void setDisplayBrightness(int brightness);
  static void clearDistanceDisplay();
  static void showDistance(int distance);

  // Input functions
  static bool getThrustLever();
  static bool getSystemsLever();
  static bool getConfirmLever();
  static LANDER_CONTROLS getControlButtonPressed();

  // Random utilities
  static int getRandomDrift();
  static void seedRandom();

private:
  static LANDER_CONTROLS lastKey;
};

// External hardware objects (defined in LanderHardware.cpp)
extern U8G2_SH1106_128X64_NONAME_2_HW_I2C landerDisplay;
extern TM1637Display distanceDisplay;
extern Keypad lander_controls;

#endif // LANDER_HARDWARE_H
