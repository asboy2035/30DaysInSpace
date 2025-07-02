// Libraries
#include "Arduino.h"

// Classes
#include "LanderTypes.h"
#include "LanderHardware.h"
#include "LanderGame.h"
#include "LanderDisplay.h"

// Game objects
LanderGame game;

void setup() {
  Serial.begin(9600);
  LanderHardware::init();
}

[[noreturn]] void loop() {
  // Read current values of all of our switches as booleans ("on" is true, "off" is false)
  const bool thrust_lever = LanderHardware::getThrustLever();
  const bool systems_lever = LanderHardware::getSystemsLever();
  const bool confirm_lever = LanderHardware::getConfirmLever();

  game.update();

  // Update our lander display (OLED) using firstPage()/nextPage() methods which
  // use a smaller buffer to save memory.  Draw the exact SAME display each time
  // through this loop!
  landerDisplay.firstPage();
  do {
    switch (game.getApproachState()) {
      // Display switch status for INIT and PREFLIGHT states.
      case APPROACH_INIT:
      case APPROACH_PREFLIGHT:
        LanderDisplay::displayPreFlight(
            game.getApproachState(),
            thrust_lever,
            systems_lever,
            confirm_lever
        );
        break;

      case APPROACH_FINAL:
        LanderDisplay::displayFinal(game.getCurrentGearBitmapIndex());
        // FALLTHROUGH to APPROACH_IN_FLIGHT to also display in-flight data

      case APPROACH_IN_FLIGHT:
        LanderDisplay::displayInFlight(
            game.getLanderDistance(),
            game.getLanderSpeed(),
            game.getMotherShipXOffset(),
            game.getMotherShipYOffset()
        );
        break;
    }
  } while (landerDisplay.nextPage());

  LanderHardware::showDistance(game.getLanderDistance());

  // Determines outcome image
  if (game.isGameOver()) {
    LanderHardware::clearDistanceDisplay(); // Show 0 on 7-segment display

    // Calculate elapsed time (in ms) from first thrust.
    const unsigned long elapsed_time = millis() - game.getApproachStartTime();
    const unsigned char* endingBitmap = game.getEndingBitmap();

    LanderDisplay::displayEndingScreen(
        elapsed_time,
        endingBitmap,
        game.getCurrentGearBitmapIndex(),
        game.getLanderDistance(),
        game.getLanderSpeed(),
        game.getMotherShipXOffset(),
        game.getMotherShipYOffset()
    );
  }

  delay(100);  // Delay before next loop
}
