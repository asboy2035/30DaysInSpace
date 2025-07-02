//
// Created by ash on 6/15/25.
//

#include "LanderDisplay.h"
#include "LanderHardware.h"
#include "LanderConfig.h"
#include "radarArrows.h"
#include "smallLandingGearBitmaps.h"
#include "endingBitmaps.h"

// Gear bitmaps array
const static unsigned char* GEAR_BITMAPS[] = {
  LANDING_GEAR_1,  // Gear up
  LANDING_GEAR_2,  // Gear lowering
  LANDING_GEAR_3,  // Gear lowering
  LANDING_GEAR_4,  // Gear down
};

constexpr int GEAR_BITMAP_COUNT = sizeof(GEAR_BITMAPS) / sizeof(GEAR_BITMAPS[0]);

void LanderDisplay::displayPreFlight(
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

void LanderDisplay::displayInFlight(
    const int lander_distance,
    const int lander_speed,
    const int mother_ship_x_offset,
    const int mother_ship_y_offset
) {
    // Mother ship initially appears as a single dot, but expands into a rectangle
    // as we get closer.  Scaled based on the maximum width, from 1 to MAX.
    constexpr unsigned int SEGMENT_SIZE = INITIAL_DISTANCE / (MAX_MOTHER_SHIP_WIDTH - 1);
    const byte segment_number = lander_distance / SEGMENT_SIZE;

    // subtract segment number from width/height to get visible width (minimum 1)
    const int mother_ship_width = MAX_MOTHER_SHIP_WIDTH - segment_number;
    int mother_ship_height = MAX_MOTHER_SHIP_HEIGHT - segment_number;

    if (mother_ship_height < 1) {
        mother_ship_height = 1;  // Always at least 1 pixel high
    }

    // coordinates of the center of our radar display
    const byte RADAR_CENTER_X = (landerDisplay.getDisplayWidth() / 2 / 2);  // center of left half
    const byte RADAR_CENTER_Y = (landerDisplay.getDisplayHeight() / 2);     // Vertical center

    // Display bitmaps with 0 bits set to transparent.
    landerDisplay.setBitmapMode(1);

    // Draw radar display circle and center pointer dot.
    landerDisplay.drawCircle(RADAR_CENTER_X, RADAR_CENTER_Y, RADAR_RADIUS);
    landerDisplay.drawPixel(RADAR_CENTER_X, RADAR_CENTER_Y);

    // Draw directional arrows based on drift
    if (mother_ship_x_offset < -DRIFT_BEFORE_ARROW_X) {  // Left of center
        if (mother_ship_y_offset < -DRIFT_BEFORE_ARROW_Y) {
            landerDisplay.drawXBMP(9, 9, ARROW_SIZE_X, ARROW_SIZE_Y, ARROW_UP_LEFT);
        } else if (mother_ship_y_offset > DRIFT_BEFORE_ARROW_Y) {
            landerDisplay.drawXBMP(8, 45, ARROW_SIZE_X, ARROW_SIZE_Y, ARROW_DOWN_LEFT);
        } else {
            landerDisplay.drawXBMP(1, 27, ARROW_SIZE_X, ARROW_SIZE_Y, ARROW_LEFT);
        }
    } else if (mother_ship_x_offset > DRIFT_BEFORE_ARROW_X) {  // Right of center
        if (mother_ship_y_offset < -DRIFT_BEFORE_ARROW_Y) {
            landerDisplay.drawXBMP(45, 8, ARROW_SIZE_X, ARROW_SIZE_Y, ARROW_UP_RIGHT);
        } else if (mother_ship_y_offset > DRIFT_BEFORE_ARROW_Y) {
            landerDisplay.drawXBMP(45, 45, ARROW_SIZE_X, ARROW_SIZE_Y, ARROW_DOWN_RIGHT);
        } else {
            landerDisplay.drawXBMP(53, 27, ARROW_SIZE_X, ARROW_SIZE_Y, ARROW_RIGHT);
        }
    } else {  // close to center in X direction
        if (mother_ship_y_offset < -DRIFT_BEFORE_ARROW_Y) {
            landerDisplay.drawXBMP(27, 1, ARROW_SIZE_X, ARROW_SIZE_Y, ARROW_UP);
        } else if (mother_ship_y_offset > DRIFT_BEFORE_ARROW_Y) {
            landerDisplay.drawXBMP(27, 53, ARROW_SIZE_X, ARROW_SIZE_Y, ARROW_DOWN);
        }
    }

    // Display speed in upper right
    char buffer[9];
    sprintf(buffer, "SPD: %2d", lander_speed);
    const u8g2_uint_t width = landerDisplay.getStrWidth(buffer);
    landerDisplay.drawStr(landerDisplay.getDisplayWidth() - width, 0, buffer);

    // Draw the mother ship as a rectangle
    const byte x_offset = RADAR_CENTER_X + mother_ship_x_offset - (mother_ship_width / 2);
    const byte y_offset = RADAR_CENTER_Y + mother_ship_y_offset - (mother_ship_height / 2);
    landerDisplay.drawFrame(x_offset, y_offset, mother_ship_width, mother_ship_height);
}

void LanderDisplay::displayFinal(
    const int current_gear_bitmap_index
) {
    constexpr int gear_down_index = GEAR_BITMAP_COUNT - 1;

    // Coordinates for gear status text
    byte x_offset = (landerDisplay.getDisplayWidth() / 2) + 11;
    byte y_offset = landerDisplay.getMaxCharHeight() * 2;

    if (current_gear_bitmap_index == 0) {
        landerDisplay.drawStr(x_offset, y_offset, "Drop gear");
    } else if (current_gear_bitmap_index < gear_down_index) {
        landerDisplay.drawStr(x_offset, y_offset, "Lowering");
    } else {
        landerDisplay.drawStr(x_offset, y_offset, "Gear OK");
    }

    // Calculate position for gear bitmap
    x_offset = (landerDisplay.getDisplayWidth() / 2);
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

[[noreturn]] void LanderDisplay::displayEndingScreen(
    unsigned long elapsed_time,
    const unsigned char* endingBitmap,
    int current_gear_bitmap_index,
    int lander_distance,
    int lander_speed,
    int mother_ship_x_offset,
    int mother_ship_y_offset
) {
    char buffer[20];
    sprintf(buffer, "%4lu.%03lu Sec", elapsed_time / 1000, elapsed_time % 1000);

    // Alternate between splash screen with time and final radar view
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

// Helper functions
byte LanderDisplay::drawString(
    const byte x, const byte y,
    const char* string
) {
    landerDisplay.drawStr(x, y, string);
    return (y + landerDisplay.getMaxCharHeight());
}

byte LanderDisplay::displayLeverSetting(
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

String LanderDisplay::onOff(const bool val) {
    return val ? "ON" : "OFF";
}

String LanderDisplay::liftoffStateToString(
    const APPROACH_STATE approach_state
) {
    switch (approach_state) {
        case APPROACH_INIT:
            return ("Init");
        case APPROACH_PREFLIGHT:
            return ("Preflight");
        default:
            return ("");
    }
}
