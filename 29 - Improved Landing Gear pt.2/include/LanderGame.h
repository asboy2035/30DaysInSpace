//
// Created by ash on 6/15/25.
//

#ifndef LANDER_GAME_H
#define LANDER_GAME_H

#include "LanderTypes.h"

class LanderGame {
public:
  // Constructor
  LanderGame();

  // Main game loop
  void update();

  // Game state getters
  APPROACH_STATE getApproachState() const { return approach_state; }
  GEAR_STATE getGearState() const { return gear_state; }
  int getLanderDistance() const { return lander_distance; }
  int getLanderSpeed() const { return lander_speed; }
  int getMotherShipXOffset() const { return mother_ship_x_offset; }
  int getMotherShipYOffset() const { return mother_ship_y_offset; }
  int getCurrentGearBitmapIndex() const { return current_gear_bitmap_index; }
  unsigned long getApproachStartTime() const { return approachStartTime; }

  // Game state checkers
  bool isGameOver() const { return lander_distance <= 0; }
  const unsigned char* getEndingBitmap() const;
  unsigned long getElapsedTime() const;

private:
  // Game state variables
  APPROACH_STATE approach_state;
  GEAR_STATE gear_state;

  unsigned long approachStartTime;
  int current_gear_bitmap_index;

  int lander_distance;
  int lander_speed;
  int mother_ship_x_offset;
  int mother_ship_y_offset;

  // State processing functions
  void processApproachInit();
  void processApproachPreflight();
  void processApproachInFlight();
  static void processApproachFinal();

  void processInflightState();
  bool processSpeedState(LANDER_CONTROLS action);
  bool processGearState(LANDER_CONTROLS action);
  void processSteeringState(LANDER_CONTROLS action);
  static int getRandomDrift();

  void updateGearAnimation();
  void updateMotherShipDrift();
  void updateDistance();
};

#endif // LANDER_GAME_H
