//
// Created by ash on 6/15/25.
//

#ifndef LANDER_CONFIG_H
#define LANDER_CONFIG_H

#include "Arduino.h"

// Pins
constexpr byte DISTANCE_DISPLAY_DIO = 4;
constexpr byte DISTANCE_DISPLAY_CLK = 5;

constexpr byte CONFIRM_LEVER = A2;
constexpr byte SYSTEMS_LEVER = A1;
constexpr byte THRUST_LEVER = A0;

// Control Matrix Configuration
constexpr byte CONTROL_ROW_COUNT = 4;
constexpr byte CONTROL_COLUMN_COUNT = 4;

constexpr byte COLUMN_PINS[CONTROL_COLUMN_COUNT] = { 10, 11, 12, 13 };
constexpr byte ROW_PINS[4] = {9, 8, 7, 6};

// Game Constants
constexpr int INITIAL_DISTANCE = 1476;  // Distance to mother ship
constexpr byte MAX_MOTHER_SHIP_WIDTH = 21;
constexpr byte MAX_MOTHER_SHIP_HEIGHT = 15;
constexpr byte DRIFT_CONTROL = 3;       // Must be > 1.  Higher numbers slow drift rate.

// Display Constants
constexpr byte RADAR_RADIUS = 25;
constexpr int DRIFT_BEFORE_ARROW_X = 2;
constexpr int DRIFT_BEFORE_ARROW_Y = 2;

#endif // LANDER_CONFIG_H
