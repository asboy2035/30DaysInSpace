// -MARK: Definitions
#include "Arduino.h"
#include <TM1637Display.h> // 7-segment
#include <U8g2lib.h>  // Display

#define numberOfMinutes(_milliseconds_) (((_milliseconds_ + 999) / 1000) / 60)
#define numberOfSeconds(_milliseconds_) (((_milliseconds_ + 999) / 1000) % 60)

// Pin connections for our 4 digit counter
#define COUNTER_DISPLAY_CLK_PIN 5
#define COUNTER_DISPLAY_DIO_PIN 4

// Construct counter_display handle.
TM1637Display counter_display(COUNTER_DISPLAY_CLK_PIN, COUNTER_DISPLAY_DIO_PIN);
// Different display call uses smaller 256-byte buffer: (_F_) -> (_2_)
U8G2_SH1106_128X64_NONAME_2_HW_I2C lander_display(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

constexpr byte LANDER_HEIGHT = 25;  // height of our lander image, in bits
constexpr byte LANDER_WIDTH = 20;   // width of our lander image, in bits

// Define 7 Segment display values to spell out "dOnE"
constexpr uint8_t DONE[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,          // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,  // O
  SEG_C | SEG_E | SEG_G,                          // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G           // E
};

constexpr unsigned long COUNTDOWN_MILLISECONDS = (70UL * 1000UL);


// -MARK: Function hoisting
byte drawString(
  byte x, byte y,
  const char *string
);

void displayLander(
  byte x_location,
  byte y_location
);

void displayCounter(unsigned long milliseconds);


// -MARK: Setup
void setup() {
  Serial.begin(9600);
  Serial.println("CountdownMS: " + String(COUNTDOWN_MILLISECONDS));

  // Configure counter display
  counter_display.setBrightness(7); // Set max brightness (0..7)
  counter_display.clear();

  // Configure OLED display
  lander_display.begin();
  lander_display.setFont(u8g2_font_6x10_tr);
  lander_display.setFontRefHeightText();
  lander_display.setFontPosTop();

  /*
   * Display starting status on our lander's OLED display.
   *
   * The U8g2 library for Arduino uses a technique called "page buffering" to draw on
   * displays connected to microcontrollers with limited RAM. Here's a short explanation
   * of how it works:
   *
   * firstPage(): This function starts the drawing routine. It initializes the buffer with
   *              the first "page" or segment of the display.
   * nextPage(): After drawing the first page, this function is called in a loop. Each call
   *             loads the next segment into the buffer and allows you to draw additional
   *             content. This process repeats until the entire display content is drawn over
   *             multiple pages.
   * Looping: The combination of firstPage() and nextPage() is typically used inside a loop.
   *          The loop continues until nextPage() returns false, indicating that all pages
   *          have been drawn.
   *
   * By drawing the display in segments, the library conserves RAM, making it feasible to use
   * graphics displays with microcontrollers that have limited memory resources.
   */
  lander_display.firstPage();
  do {
    const byte y_offset = drawString(0, 0, "Exploration Lander");
    drawString(0, y_offset, "Liftoff Sequence");

    // Status on bottom line of OLED display
    drawString(
      0,
      lander_display.getDisplayHeight() - lander_display.getMaxCharHeight(),
      "Countdown Active"
    );
    // Draw a picture of our lander int bottom right corner
    displayLander(
      lander_display.getDisplayWidth() - LANDER_WIDTH,
      lander_display.getDisplayHeight() - LANDER_HEIGHT
    );
  } while (lander_display.nextPage());

  // blink the countdown on our timer before beginning the countdown
  for (int i = 0; i < 4; i++) {
    counter_display.clear();
    delay(200);
    displayCounter(COUNTDOWN_MILLISECONDS);
    delay(200);
  }
  Serial.println("Countdown started..: ");
}

// -MARK: Loop
[[noreturn]] void loop() {
  static unsigned long timeRemaining = COUNTDOWN_MILLISECONDS;
  static unsigned long countdown_start_time = millis();

  displayCounter(timeRemaining);  // Display minutes:seconds on counter display

  // Display ending values
  if (timeRemaining == 0) {
    Serial.println("Done!!");
    counter_display.setSegments(DONE);

    // Update OLED display with ending screen using firstPage()/nextPage()
    lander_display.firstPage();
    do {
      // Each time we display a line of text on our display the y_offset
      // is updated to point to the next available point for drawing.

      // Display first two lines
      byte y_offset = drawString(0, 0, "Exploration Lander");
      drawString(0, y_offset, "Liftoff ABORTED");

      // Set y_offset to point four lines above bottom of display
      y_offset = lander_display.getDisplayHeight() - (4 * lander_display.getMaxCharHeight());

      // Display last four lines
      y_offset = drawString(0, y_offset, "Thrusters: OFF");
      y_offset = drawString(0, y_offset, "Systems: OFF");
      y_offset = drawString(0, y_offset, "Confirm: OFF");
      drawString(0, y_offset, "Countdown ABORT");

      // Draw a picture of our lander in bottom right corner
      displayLander(
        lander_display.getDisplayWidth() - LANDER_WIDTH,
        lander_display.getDisplayHeight() - LANDER_HEIGHT
      );
    } while (lander_display.nextPage());

    // Stop here by looping forever.
    while (true);
  }

  // Update our remaining time by subtracting the start time from current
  // execution time (in milliseconds).  If our elapsed time is greater
  // than our countdown time then set remaining time to 0.
  const unsigned long elapsed_time = millis() - countdown_start_time;
  if (elapsed_time < COUNTDOWN_MILLISECONDS) {
    timeRemaining = COUNTDOWN_MILLISECONDS - elapsed_time;
  } else {
    timeRemaining = 0;
  }
}

// -MARK: Functions
// Display milliseconds as minutes:seconds (MM:SS)
void displayCounter(const unsigned long milliseconds) {
  // To display the countdown in mm:ss format, separate the parts
  const byte minutes = numberOfMinutes(milliseconds);
  const byte seconds = numberOfSeconds(milliseconds);

  Serial.print("ms: ");
  Serial.print(milliseconds);
  Serial.print(" | m/s: ");
  Serial.println(String(minutes) + "/" + String(seconds));

  // Display the minutes in the first two places, with colon
  counter_display.showNumberDecEx(minutes, 0b01000000, true, 2, 0);
  // This displays the seconds in the last two places
  counter_display.showNumberDecEx(seconds, 0, true, 2, 2);
}

// Draw test on our lander display at x, y, returning new y
// value that is immediately below the new line of text.
byte drawString(
  const byte x, const byte y,
  const char *string
) {
  lander_display.drawStr(x, y, string);
  return (y + lander_display.getMaxCharHeight());  // return new y_offset on display
}

// -MARK: Display Lander
// Draw an image of our lander drawn with frames and triangles
// at location x_location, y_location (relative to the upper left corner).
void displayLander(
  const byte x_location,
  const byte y_location
) {
  // Ship top
  lander_display.drawFrame(
    x_location + 7,
    y_location,
    6,
    5
  );

  // Ship center
  lander_display.drawFrame(
    x_location + 5,
    y_location + 4,
    10,
    20
  );

  // Left pod
  lander_display.drawFrame(
    x_location,
    y_location + 6,
    6,
    16
  );

  // Right pod
  lander_display.drawFrame(
    x_location + 14,
    y_location + 6,
    6,
    16
  );

  // Left nozzle
  lander_display.drawTriangle(
    x_location + 2,
    y_location + 21,
    x_location,
    y_location + 25,
    x_location + 4,
    y_location + 25
  );

  // Right nozzle
  lander_display.drawTriangle(
    x_location + 18,
    y_location + 21,
    x_location + 15,
    y_location + 25,
    x_location + 20,
    y_location + 25
  );
}
