//
// Created by ash on 6/15/25.
//

#ifndef LANDER_TYPES_H
#define LANDER_TYPES_H

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

// Here are the states our code will run through to perform the actual
// approach flight and landing in the mother ship bay.
enum APPROACH_STATE {
  APPROACH_INIT,       // Ensure all switches are off to begin
  APPROACH_PREFLIGHT,  // Wait for all switches to be enabled
  APPROACH_IN_FLIGHT,  // Begin to approach mother ship
  APPROACH_FINAL       // Lower landing gear!
};

#endif // LANDER_TYPES_H
