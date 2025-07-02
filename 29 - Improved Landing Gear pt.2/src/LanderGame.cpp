//
// Created by ash on 6/15/25.
//

//
// Created by ash on 6/15/25.
//

#include "LanderGame.h"
#include "LanderHardware.h"
#include "LanderConfig.h"
#include "endingBitmaps.h"

constexpr int GEAR_BITMAP_COUNT = 4;  // Number of gear animation frames

LanderGame::LanderGame() :
    approach_state(APPROACH_INIT),
    gear_state(GEAR_IDLE),
    approachStartTime(0),
    current_gear_bitmap_index(0),
    lander_distance(INITIAL_DISTANCE),
    lander_speed(0),
    mother_ship_x_offset(0),
    mother_ship_y_offset(0)
{
}

void LanderGame::update() {
    // Read current values of all switches
    const bool thrust_lever = LanderHardware::getThrustLever();
    const bool systems_lever = LanderHardware::getSystemsLever();
    const bool confirm_lever = LanderHardware::getConfirmLever();

    // Primary control state machine
    switch (approach_state) {
        case APPROACH_INIT:
            processApproachInit();
            break;

        case APPROACH_PREFLIGHT:
            processApproachPreflight();
            break;

        case APPROACH_FINAL:
            processApproachFinal();
            [[fallthrough]];  // Intentional fallthrough

        case APPROACH_IN_FLIGHT:
            processApproachInFlight();
            break;
    }

    updateGearAnimation();
    updateMotherShipDrift();
    updateDistance();
}

void LanderGame::processApproachInit() {
    const bool thrust_lever = LanderHardware::getThrustLever();
    const bool systems_lever = LanderHardware::getSystemsLever();
    const bool confirm_lever = LanderHardware::getConfirmLever();

    // All levers off
    if (!thrust_lever && !systems_lever && !confirm_lever) {
        approach_state = APPROACH_PREFLIGHT;
    }
}

void LanderGame::processApproachPreflight() {
    const bool thrust_lever = LanderHardware::getThrustLever();
    const bool systems_lever = LanderHardware::getSystemsLever();
    const bool confirm_lever = LanderHardware::getConfirmLever();

    // All levers on
    if (thrust_lever && systems_lever && confirm_lever) {
        approach_state = APPROACH_IN_FLIGHT;
    }
}

void LanderGame::processApproachInFlight() {
    processInflightState();

    // Prepare for landing on final approach
    if (lander_distance < (INITIAL_DISTANCE / 10)) {
        approach_state = APPROACH_FINAL;
    }
}

void LanderGame::processApproachFinal() {
    // Near to mothership, lower gear
    // Process gear control in the inflight state processing
}

void LanderGame::processInflightState() {
    const auto currentKey = LanderHardware::getControlButtonPressed();
    bool actionCompleted = processSpeedState(currentKey);

    if (actionCompleted) {
        return;
    }

    actionCompleted = processGearState(currentKey);

    if (actionCompleted) {
        return;
    }

    processSteeringState(currentKey);
}

bool LanderGame::processSpeedState(const LANDER_CONTROLS action) {
    bool actionCompleted = false;

    switch (action) {
        case RAISE_SPEED:
            actionCompleted = true;
            lander_speed++;
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

        default:
            break;
    }

    return actionCompleted;
}

bool LanderGame::processGearState(const LANDER_CONTROLS action) {
    bool actionCompleted = false;

    switch (action) {
        case LOWER_GEAR: {
            actionCompleted = true;
            // Lower landing gear unless already lowered
            if (approach_state == APPROACH_FINAL) {  // Only works on final approach
                if (current_gear_bitmap_index != GEAR_BITMAP_COUNT - 1) {
                    gear_state = GEAR_LOWERING;
                }
            }
            break;
        }

        case RAISE_GEAR: {
            actionCompleted = true;
            // Raise landing gear unless already raised
            if (current_gear_bitmap_index != 0) {
                gear_state = GEAR_RAISING;
            }
            break;
        }

        default:
            break;
    }

    return actionCompleted;
}

void LanderGame::processSteeringState(const LANDER_CONTROLS action) {
    switch (action) {
        case STEER_UP:
            mother_ship_y_offset++;
            break;
        case STEER_DOWN:
            mother_ship_y_offset--;
            break;
        case STEER_LEFT:
            mother_ship_x_offset++;
            break;
        case STEER_RIGHT:
            mother_ship_x_offset--;
            break;
        case STEER_UP_RIGHT:
            mother_ship_x_offset--;
            mother_ship_y_offset++;
            break;
        case STEER_UP_LEFT:
            mother_ship_x_offset++;
            mother_ship_y_offset++;
            break;
        case STEER_DOWN_RIGHT:
            mother_ship_x_offset--;
            mother_ship_y_offset--;
            break;
        case STEER_DOWN_LEFT:
            mother_ship_x_offset++;
            mother_ship_y_offset--;
            break;
        default:
            break;
    }
}

int LanderGame::getRandomDrift() {
    constexpr byte DRIFT_CONTROL = 3;  // Must be > 1. Higher numbers slow drift rate.
    int drift = static_cast<int>(random(-1, DRIFT_CONTROL));

    if (drift > 1) {  // Values over 1 are changed to 0
        drift = 0;
    }

    return drift;
}

void LanderGame::updateGearAnimation() {
    // Because we specified our gear states as 0, 1 or -1 we can change bitmaps by
    // simply adding the gear state to our current gear bitmap index.
    current_gear_bitmap_index += gear_state;

    // If the gear animation has completed (index is either 0 or the index of our last bitmap)
    // then we change the gear state to IDLE to complete animation.
    if (current_gear_bitmap_index == 0 || current_gear_bitmap_index == GEAR_BITMAP_COUNT - 1) {
        gear_state = GEAR_IDLE;
    }
}

void LanderGame::updateMotherShipDrift() {
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
}

void LanderGame::updateDistance() {
    lander_distance -= lander_speed;  // Adjust distance by current speed
}

const unsigned char* LanderGame::getEndingBitmap() const {
    constexpr byte MAX_MOTHER_SHIP_WIDTH = 21;
    constexpr byte MAX_MOTHER_SHIP_HEIGHT = 15;

    const bool missedMotherShip = !(
        abs(mother_ship_x_offset) < ((MAX_MOTHER_SHIP_WIDTH + 1) / 2) &&
        abs(mother_ship_y_offset) < ((MAX_MOTHER_SHIP_HEIGHT + 1) / 2)
    );

    if (missedMotherShip) {
        // Missed the mother ship. No fuel for another try. Bye!
        return ENDING_BITMAP_MISSED_MOTHER_SHIP;
    }

    // Check speed to see if we were slow enough.
    if (lander_speed > 2) {
        // Max safe landing speed is 2
        // Speed is too fast! Lander AND mother ship destroyed. (Ouch!)
        return ENDING_BITMAP_TOO_FAST;
    }

    // Did we remember to lower the landing gear?
    if (current_gear_bitmap_index == GEAR_BITMAP_COUNT - 1) {
        // Gear is down! Success.
        return ENDING_BITMAP_SUCCESS;
    }

    // Gear is up; damage to lander, but we survived.
    return ENDING_BITMAP_NO_GEAR;
}

unsigned long LanderGame::getElapsedTime() const {
    return millis() - approachStartTime;
}
