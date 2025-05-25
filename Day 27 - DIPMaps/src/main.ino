#include <U8g2lib.h> // Display library for SH1106 OLED
#include "Wire.h" // I2C communication
#include "switch_bitmaps.h" // Bitmap images for switch positions
#include <TM1637Display.h> // 4-digit 7-segment display

// Pins
constexpr byte NUMBER_DISPLAY_DIO = 2;
constexpr byte NUMBER_DISPLAY_CLK = 3;

constexpr byte SWITCH_0 = A2;  // Bit 0
constexpr byte SWITCH_1 = A1;  // Bit 1
constexpr byte SWITCH_2 = A0;  // Bit 2

// Displays
U8G2_SH1106_128X64_NONAME_2_HW_I2C bitmap_display(U8G2_R0, /* reset = */ U8X8_PIN_NONE);
TM1637Display number_display(NUMBER_DISPLAY_CLK, NUMBER_DISPLAY_DIO);

// Bitmap image pointers array (indexed 0–7)
const static unsigned char* SWITCH_BITMAPS[] = {
  SWITCHES_ZERO,
  SWITCHES_ONE,
  SWITCHES_TWO,
  SWITCHES_THREE,
  SWITCHES_FOUR,
  SWITCHES_FIVE,
  SWITCHES_SIX,
  SWITCHES_SEVEN,
};

// Functions
int digitalReadPin(
  uint8_t pin
) {
  return digitalRead(pin) == HIGH ? 1 : 0;
}

// -MARK: Setup
void setup(void) {
  // Initialize 7-segment display
  number_display.setBrightness(7);  // Max brightness (0–7)
  number_display.clear();           // Clear display

  // Set switch pins as inputs
  pinMode(SWITCH_0, INPUT);
  pinMode(SWITCH_1, INPUT);
  pinMode(SWITCH_2, INPUT);

  // Initialize OLED
  bitmap_display.begin();
}

// -MARK: Loop
void loop() {
  // Center bitmap on the screen
  const byte x_offset = (bitmap_display.getDisplayWidth() - BITMAP_WIDTH) / 2;
  const byte y_offset = (bitmap_display.getDisplayHeight() - BITMAP_HEIGHT) / 2;

  // Read 3-bit switch input (values 0–7)
  byte switch_value = 0;
  switch_value |= digitalReadPin(SWITCH_0);
  switch_value |= digitalReadPin(SWITCH_1) << 1;
  switch_value |= digitalReadPin(SWITCH_2) << 2;

  // Show number on 7-segment display
  number_display.showNumberDecEx(switch_value);

  // Show corresponding bitmap on OLED
  bitmap_display.firstPage();
  do {
    bitmap_display.drawXBMP(
      x_offset, y_offset,
      BITMAP_WIDTH, BITMAP_HEIGHT,
      SWITCH_BITMAPS[switch_value]
    );
  } while (bitmap_display.nextPage());

  delay(100);
}
