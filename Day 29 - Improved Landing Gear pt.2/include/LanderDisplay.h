//
// Created by ash on 6/15/25.
//

#ifndef LANDER_DISPLAY_H
#define LANDER_DISPLAY_H

#include "Arduino.h"
#include "LanderTypes.h"

class LanderDisplay {
public:
  // Display management
  static void displayPreFlight(
      APPROACH_STATE approach_state,
      bool thrusterLever,
      bool systemsLever,
      bool confirmLever
  );

  static void displayInFlight(
      int lander_distance,
      int lander_speed,
      int mother_ship_x_offset,
      int mother_ship_y_offset
  );

  static void displayFinal(
      int current_gear_bitmap_index
  );

  static void displayEndingScreen(
      unsigned long elapsed_time,
      const unsigned char* endingBitmap,
      int current_gear_bitmap_index,
      int lander_distance,
      int lander_speed,
      int mother_ship_x_offset,
      int mother_ship_y_offset
  );

private:
  // Helper functions
  static byte drawString(byte x, byte y, const char* string);
  static byte displayLeverSetting(const String& leverName, bool leverVal, byte yOffset);
  static String onOff(bool val);
  static String liftoffStateToString(APPROACH_STATE approach_state);
};

#endif // LANDER_DISPLAY_H
