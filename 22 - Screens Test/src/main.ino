/*
 * 30 Days - Lost in Space
 * Day 22 - Display Panel Details
 *
 * Even our new OLED display is a bit small if we have large amounts of
 * information to display.  But this new display isn't limited to JUST
 * text!  It is a 128x64 pixel display and we can turn any of them on/off
 * to display whatever we like, including graphics.
 *
 * Today we'll run a "readiness check" on our new display to test out its
 * full capabilities.
 *
 * Learn more at https://learn.inventr.io/adventure
 *
 * Alex Eschenauer
 * David Schmidt
 * Greg Lyzenga
 */

// -MARK: Consts, setup
#include "Arduino.h"
#include "Wire.h"
 
/*
 * Extensive documentation for this library can be found at https://github.com/olikraus/u8g2
 * for those wanting to dive deeper, but we will explain all of the functions
 * used in these lessons.
 */
#include <U8g2lib.h>  // Include file for the U8g2 library.
 
// Construct our lander_display handle using the same constructor from Day 21
U8G2_SH1106_128X64_NONAME_F_HW_I2C lander_display(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
 
typedef struct Point {
  byte x;
  byte y;
} Point;

typedef struct Circle {
  Point center;
  byte radius;
};

typedef struct Box {
  Point topLeft;
  byte width;
  byte height;
};

void setup(void) {
  lander_display.begin();
}
 
/*
 * For our main loop() we will cycle through each test page, displaying
 * 8 frames for each test (which allows animation on each page).
 *
 * Rather than use two variables, one to track the page and one to track
 * the frame, we will demonstrate the use of the "bitshift" operators (">>"
 * and "<<") in conjunction with the bitwise and" to use a single variable.
 *
 * While this method is less clear, it is sometimes used when memory is very
 * tight since it uses less data storage.
 */
void loop(void) { // -MARK: Loop
  const byte TEST_PAGE_COUNT = 13;
  for (unsigned int display_frame = 0; display_frame < (TEST_PAGE_COUNT * 8); display_frame++) {
    lander_display.clearBuffer();  // clear the internal memory
 
    // setCursor() will set the x/y location of the upper left bit of each string
    lander_display.setFont(u8g_font_6x10);
    lander_display.setFontRefHeightExtendedText();
    lander_display.setFontPosTop();
 
    // y_offset is current Y coordinate below all previously drawn content
    byte y_offset = 0;    // start at top of display
 
    // Title line for our display displayed on each screen
    drawCenteredString(0, 0, "Exploration Lander");  // Center title at top of display
 
    // To leave the title above each display we add the maximum character height of
    // our current font to the current y_offset.
    y_offset += lander_display.getMaxCharHeight();  // maximum height of current font
 
    // If you'd like to just view a single page just uncomment the next two lines and
    // set the page desired in the first line:

    // const byte DISPLAY_PAGE = 12;
    // display_frame = (display_frame & 7) | (DISPLAY_PAGE << 3);
 
    // Now display the appropriate page for our current display_frame.
    // Since each page is displayed 8 times we shift the display_frame right
    // by 3 bits, which is a FAST way of dividing it by 8.
    switch (display_frame >> 3) {
      // The frame number for each page is contained in the rightmost 3
      // bits of the current display_frame.  So here we remove the page
      // number by using a bitwise AND to remove all but the last three bits.
      case 0:
        display_test_ready(y_offset, display_frame & 0b00000111);
        break;
        
      case 1:
        display_test_box_frame(y_offset, display_frame & 0b00000111);
        break;
        
      case 2:
        display_test_circles(y_offset, display_frame & 0b00000111);
        break;
        
      case 3:
        display_test_r_frame(y_offset, display_frame & 0b00000111);
        break;
        
      case 4:
        display_test_string(y_offset, display_frame & 0b00000111);
        break;
        
      case 5:
        display_test_line(y_offset, display_frame & 0b00000111);
        break;
        
      case 6:
        display_test_triangle(y_offset, display_frame & 0b00000111);
        break;
        
      case 7:
        display_test_ascii_1(y_offset);
        break;
        
      case 8:
        display_test_ascii_2(y_offset);
        break;
        
      case 9:
        display_test_extra_page(y_offset, display_frame & 0b00000111);
        break;
        
      case 10:
        display_test_bitmap_modes(y_offset, display_frame & 0b00000111, false);
        break;
        
      case 11:
        display_test_bitmap_modes(y_offset, display_frame & 0b00000111, true);
        break;
        
      case 12:
        display_test_bitmap_overlay(y_offset, display_frame & 0b00000111);
        break;

      default: 
        break;
    }
    lander_display.sendBuffer();
    delay(100);
  }
}
 
// Use the .drawStr() method to draw a string in the display centered
// horizontally between the given X coordinate and the maximum X.
// Y coordinate is unchanged and text is displayed relative to the
// current font positioning mode (Top, Center, Bottom).
byte drawCenteredString(byte x, byte y, char *string) {
  byte centered_x = x + ((lander_display.getDisplayWidth() - x) - lander_display.getStrWidth(string)) / 2;
  lander_display.drawStr(centered_x, y, string);
}
 
const byte LANDER_HEIGHT = 25;    // height of our lander image, in bits
const byte LANDER_WIDTH = 20;     // width of our lander image, in bits
 
// Draw an image of our lander drawn with frames and triangles
// at location x_location, y_location (relative to the upper left corner).
void display_lander(byte x_location, byte y_location) {
  // Ship top
  lander_display.drawFrame(x_location + 7, y_location, 6, 5);

  // Ship center
  lander_display.drawFrame(x_location + 5, y_location + 4, 10, 20);

  // Left pod
  lander_display.drawFrame(x_location, y_location + 6, 6, 16);

  // Right pod
  lander_display.drawFrame(x_location + 14, y_location + 6, 6, 16);

  // Left nozzle
  lander_display.drawTriangle(
    x_location + 2, y_location + 21,
    x_location, y_location + 25,
    x_location + 4, y_location + 25
  );

  // Right nozzle
  lander_display.drawTriangle(
    x_location + 18, y_location + 21,
    x_location + 15, y_location + 25,
    x_location + 20, y_location + 25
  );
}
 
/////////////////////////////////////////////////////////////////////
// -MARK: 0: Begin message
// Page 0: Display lander drawing and blinking begin message
void display_test_ready(byte y_offset, byte frame) {
  // Y value halfway between top and bottom of drawable area
  const byte Y_CENTER = y_offset + ((lander_display.getDisplayHeight() - y_offset) / 2);
 
  // Since our lander drawing's location is set using the upper-left
  // coordinates we will subtract half our drawing's height to center
  // it vertically.
  //
  // We will also shift it right by adding a "padding" amount to the
  // lander's X coordinate.
  const byte LANDER_PADDING = LANDER_WIDTH;   // shift lander right by its width
  const byte LANDER_Y_CENTER = Y_CENTER - (LANDER_HEIGHT / 2);
  display_lander(LANDER_PADDING, LANDER_Y_CENTER);
 
  // Blink the "ready" message by only displaying it every other frame
  // by extracting the rightmost bit of our frame number and only displaying
  // the message if the bit is 0 (frames 0, 2, 4, 6).
  if ((frame & 0b00000001) == 0) {
    // Determine remaining space to the right of our lander for our message.
    // The lander image extends from its starting X position plus its width.
    const byte X_OFFSET = LANDER_PADDING + LANDER_WIDTH;
 
    // get the height of each text line since it's used multiple times below
    byte text_height = lander_display.getMaxCharHeight();
 
    lander_display.setFontPosCenter();  // Y coordinate relative to center of font height
    // Display line 1 on display, one line above center
    drawCenteredString(X_OFFSET, Y_CENTER - text_height, "Begin");
 
    // Display line 2, vertically centered in space below title
    drawCenteredString(X_OFFSET, Y_CENTER, "Hardware");
 
    // Display line 3 on display, one line below center
    drawCenteredString(X_OFFSET, Y_CENTER + text_height, "Test");
  }
}
 
///////////////////////////////////////////////////////////////////
// -MARK: 1: Boxes
// Page 1: Display filled and hollow boxes with one moving every frame.
 
// Here we define constants for our box size and coordinates.  This is
// done so they can be changed in one place, and it also makes it clear
// what the MEANINGS of each number are in the code below.  It changes
// code like:
// .drawBox(5,10,20,10);
// to:
// .drawBox(BOX1_X_OFFSET, y_offset, BOX1_WIDTH, BOX1_HEIGHT);



const byte BOX1_WIDTH = 20;
const byte BOX1_HEIGHT = 10;
const byte BOX1_X_OFFSET = 5;
const byte BOX2_WIDTH = BOX1_WIDTH * 1.5;
const byte BOX2_HEIGHT = BOX1_HEIGHT * .8;
const byte BOX2_X_OFFSET = BOX1_X_OFFSET * 2;
const byte BOX2_Y_OFFSET = BOX1_HEIGHT / 2;
 
void display_test_box_frame(byte y_offset, byte frame) {
  // Display "drawBox" label
  drawCenteredString(0, y_offset, "drawBox");
  
  // Move down by font height to position for the boxes
  y_offset += lander_display.getMaxCharHeight();

  // Draw a filled box at the specified coordinates and dimensions
  lander_display.drawBox(
    BOX1_X_OFFSET, 
    y_offset, 
    BOX1_WIDTH, 
    BOX1_HEIGHT
  );

  // Draw a second filled box, offset to the right, moving each frame
  lander_display.drawBox(
    BOX2_X_OFFSET + frame, 
    y_offset + BOX2_Y_OFFSET,
    BOX2_WIDTH, 
    BOX2_HEIGHT
  );

  // Move down below the boxes for the next set of drawings
  y_offset += BOX2_Y_OFFSET + BOX2_HEIGHT;

  // Display "drawFrame" label
  drawCenteredString(0, y_offset, "drawFrame");

  // Move down by font height again for the frame boxes
  y_offset += lander_display.getMaxCharHeight();

  // Draw box frame at the specified coordinates and dimensions
  lander_display.drawFrame(
    BOX1_X_OFFSET, 
    y_offset, 
    BOX1_WIDTH, 
    BOX1_HEIGHT
  );

  // Draw second box frame, offset and moving right each frame
  lander_display.drawFrame(
    BOX2_X_OFFSET + frame, 
    y_offset + BOX2_Y_OFFSET, 
    BOX2_WIDTH, 
    BOX2_HEIGHT
  );
}
 
/////////////////////////////////////////////////////////////////////
// -MARK: 2: Circles
// Page 2: Display filled and hollow circles with one moving every frame.
 
const Circle circle1 = {
  { 8, 8 },
  8
};

const Circle circle2 = {
  { 1, 2 },
  circle1.radius - 1
};

const byte CIRCLE1_DIAMETER = (circle1.radius * 2) + 1;   // Diameter is (radius * 2) + 1
const byte CIRCLE1_X_OFFSET = circle1.radius;
const byte CIRCLE2_X_OFFSET = CIRCLE1_DIAMETER + circle2.radius;

void display_test_circles(byte y_offset, byte frame) {
  // Drawing discs
  drawCenteredString(0, y_offset, "drawDisc");
  y_offset += lander_display.getMaxCharHeight();  // offset down by font height

  // Center disc at x, y with radius.
  lander_display.drawDisc(
    circle1.center.x, 
    y_offset + circle1.radius, 
    circle1.radius
  );

  // Draw second (moving) disc
  lander_display.drawDisc(
    CIRCLE2_X_OFFSET + frame,
    y_offset + circle2.radius, 
    circle2.radius
  );
 

  // Drawing circles
  drawCenteredString(0, y_offset, "drawCircle");
  y_offset += CIRCLE1_DIAMETER;
  y_offset += lander_display.getMaxCharHeight();  // offset down by font height
  // Draw hollow circle at x, y with radius
  lander_display.drawCircle(
    CIRCLE1_X_OFFSET, 
    y_offset + circle1.radius, 
    circle1.radius
  );

  // Draw second (moving) hollow circle
  lander_display.drawCircle(
    CIRCLE2_X_OFFSET + frame, 
    y_offset + circle2.radius, 
    circle2.radius
  );
}
 
/////////////////////////////////////////////////////////////////////
// -MARK: 3: Rounded rects
// Page 3: Display filled and hollow boxes with rounded corners
 
const byte RBOX1_WIDTH = 40;
const byte RBOX1_HEIGHT = 30;
const byte RBOX1_X_OFFSET = 5;
const byte RBOX2_WIDTH = 25;
const byte RBOX2_HEIGHT = 40;
const byte RBOX2_X_OFFSET = RBOX1_X_OFFSET + RBOX1_WIDTH + RBOX1_X_OFFSET;
 
void display_test_r_frame(byte y_offset, byte frame) {
  drawCenteredString(0, y_offset, "drawRFrame/Box");
  y_offset += lander_display.getMaxCharHeight();  // offset down by font height
 
  lander_display.drawRFrame(RBOX1_X_OFFSET, y_offset, RBOX1_WIDTH, RBOX1_HEIGHT, frame + 1);
  lander_display.drawRBox(RBOX2_X_OFFSET, y_offset, RBOX2_WIDTH, RBOX2_HEIGHT, frame + 1);
}
 
/////////////////////////////////////////////////////////////////////
// -MARK: 4: Texts
// Page 4: Display text strings, drawing them in different directions
 
const byte STRING_X_OFFSET = 30;
 
void display_test_string(byte y_offset, byte frame) {
  byte y_center = y_offset + (lander_display.getDisplayHeight() - y_offset) / 2;
 
  lander_display.setFontDirection(0);   // left to right
  lander_display.drawStr(STRING_X_OFFSET + frame, y_center, " 0");
 
  lander_display.setFontDirection(1);   // up to down
  lander_display.drawStr(STRING_X_OFFSET, y_center + frame, " 90");
 
  lander_display.setFontDirection(2);   // right to left (and upside down)
  lander_display.drawStr(STRING_X_OFFSET - frame, y_center, " 180");
 
  lander_display.setFontDirection(3);   // down to up
  lander_display.drawStr(STRING_X_OFFSET, y_center - frame, " 270");
 
  lander_display.setFontDirection(0);   // Restore normal left to right direction
}
 
/////////////////////////////////////////////////////////////////////
// -MARK: 5: Lines
// Page 5: Display lines at different angles
 
const byte LINE_X_OFFSET = 7;
const byte LINE_Y_MAX = 55;
 
void display_test_line(byte y_offset, byte frame) {
  drawCenteredString(0, y_offset, "drawLine");
  y_offset += lander_display.getMaxCharHeight();  // offset down by font height
 
  // Draw lines from x1/y1 to x2/y2, moving x1 each frame.
  lander_display.drawLine(
    LINE_X_OFFSET + frame, 
    y_offset,
    40, 
    LINE_Y_MAX
  );

  lander_display.drawLine(
    LINE_X_OFFSET + frame * 2, 
    y_offset,
    60, 
    LINE_Y_MAX
  );

  lander_display.drawLine(
    LINE_X_OFFSET + frame * 3, 
    y_offset,
    80, 
    LINE_Y_MAX
  );

  lander_display.drawLine(
    LINE_X_OFFSET + frame * 4, 
    y_offset,
    100, 
    LINE_Y_MAX
  );
}
 
/////////////////////////////////////////////////////////////////////
// -MARK: 6: Triangles
// Page 6: Display triangles that separate each frame
 
void display_test_triangle(byte y_offset, byte frame) {
  drawCenteredString(0, y_offset, "drawTriangle");
 
  y_offset += lander_display.getMaxCharHeight();  // offset down by font height
  lander_display.drawTriangle(
    14, y_offset + 7,
    45, y_offset + 20,
    10, y_offset + 30
  );
  
  lander_display.drawTriangle(
    14 + frame, y_offset + 7 - frame,
    45 + frame, y_offset + 20 - frame,
    57 + frame, y_offset + 0 - frame
  );
  
  lander_display.drawTriangle(
    57 + frame * 2, y_offset + 0,
    45 + frame * 2, y_offset + 20,
    96 + frame * 2, y_offset + 43
  );
  
  lander_display.drawTriangle(
    10 + frame, y_offset + 30 + frame,
    45 + frame, y_offset + 20 + frame,
    96 + frame, y_offset + 43 + frame
  );
}
 
/////////////////////////////////////////////////////////////////////
// -MARK: 7: ASCII pt.1
// Page 7: Display characters from first ASCII page
 
const byte FIRST_PRINTABLE_CHARACTER = 32;  // ASCII character 32 is first printable character
void display_test_ascii_1(byte y_offset) {
  drawCenteredString(0, y_offset, "ASCII page 1");
  y_offset += lander_display.getMaxCharHeight();  // offset down by font height
 
  // For more characters visible you can uncomment the following line for a smaller font
  // lander_display.setFont(u8g2_font_spleen5x8_mf);
  byte character_width = lander_display.getMaxCharWidth();
  byte column_count = lander_display.getDisplayWidth() / character_width;
  byte row_count = (lander_display.getDisplayHeight() - y_offset) / lander_display.getMaxCharHeight();
  
  for (byte row = 0; row < row_count; row++) {
    for (byte column = 0; column < column_count; column++) {
      char character_string[2];  // space for character plus null terminator
      byte character_number = FIRST_PRINTABLE_CHARACTER + (row * column_count) + column;
      snprintf(character_string, sizeof(character_string), "%c", character_number);
      lander_display.drawStr(
        column * character_width,
        y_offset + (row * lander_display.getMaxCharHeight()),
        character_string
      );
    }
  }
}
 
/////////////////////////////////////////////////////////////////////
// -MARK: 8: ASCII pt.2
// Page 8: Display characters from second ASCII page
 
void display_test_ascii_2(byte y_offset) {
  drawCenteredString(0, y_offset, "ASCII page 2");
  y_offset += lander_display.getMaxCharHeight();  // offset down by font height
 
  // For more characters visible you can uncomment the following line for a smaller font
  // lander_display.setFont(u8g2_font_spleen5x8_mf);
  byte character_width = lander_display.getMaxCharWidth();
  byte column_count = lander_display.getDisplayWidth() / character_width;
  byte row_count = (lander_display.getDisplayHeight() - y_offset) / lander_display.getMaxCharHeight();
 
  for (byte row = 0; row < row_count; row++) {
    for (byte column = 0; column < column_count; column++) {
      char character_string[2];  // space for character plus null terminator
      byte character_number = FIRST_PRINTABLE_CHARACTER + 128 + (row * column_count) + column;
      snprintf(character_string, sizeof(character_string), "%c", character_number);
      lander_display.drawStr(
        column * character_width,
        y_offset + (row * lander_display.getMaxCharHeight()),
        character_string
      );
    }
  }
}
 
/////////////////////////////////////////////////////////////////////
// -MARK: 9: Unicode symbols
// Page 9: Display characters from Unicode font
 
void display_test_extra_page(byte y_offset, byte frame) {
  drawCenteredString(0, y_offset, "Unicode");
  y_offset += lander_display.getMaxCharHeight();  // offset down by font height
 
  lander_display.setFont(u8g2_font_unifont_t_symbols);
  lander_display.setFontPosTop();
  lander_display.drawUTF8(0, y_offset, "☀ ☁");

  switch (frame) {
    case 0:
    case 1:
    case 2:
    case 3:
      lander_display.drawUTF8(frame * 3, y_offset + 20, "☂");
      break;
    case 4:
    case 5:
    case 6:
    case 7:
      lander_display.drawUTF8(frame * 3, y_offset + 20, "☔");
      break;
  }
}
 
const byte CROSS_WIDTH = 24;
const byte CROSS_HEIGHT = 24;
static const unsigned char cross_bits[] U8X8_PROGMEM = {
  0x00, 0x18, 0x00, 0x00, 0x24, 0x00, 0x00, 0x24, 0x00, 0x00, 0x42, 0x00,
  0x00, 0x42, 0x00, 0x00, 0x42, 0x00, 0x00, 0x81, 0x00, 0x00, 0x81, 0x00,
  0xC0, 0x00, 0x03, 0x38, 0x3C, 0x1C, 0x06, 0x42, 0x60, 0x01, 0x42, 0x80,
  0x01, 0x42, 0x80, 0x06, 0x42, 0x60, 0x38, 0x3C, 0x1C, 0xC0, 0x00, 0x03,
  0x00, 0x81, 0x00, 0x00, 0x81, 0x00, 0x00, 0x42, 0x00, 0x00, 0x42, 0x00,
  0x00, 0x42, 0x00, 0x00, 0x24, 0x00, 0x00, 0x24, 0x00, 0x00, 0x18, 0x00, };
 
const byte CROSS_FILL_WIDTH = 24;
const byte CROSS_FILL_HEIGHT = 24;
static const unsigned char cross_fill_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x18, 0x64, 0x00, 0x26,
  0x84, 0x00, 0x21, 0x08, 0x81, 0x10, 0x08, 0x42, 0x10, 0x10, 0x3C, 0x08,
  0x20, 0x00, 0x04, 0x40, 0x00, 0x02, 0x80, 0x00, 0x01, 0x80, 0x18, 0x01,
  0x80, 0x18, 0x01, 0x80, 0x00, 0x01, 0x40, 0x00, 0x02, 0x20, 0x00, 0x04,
  0x10, 0x3C, 0x08, 0x08, 0x42, 0x10, 0x08, 0x81, 0x10, 0x84, 0x00, 0x21,
  0x64, 0x00, 0x26, 0x18, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };
 
const byte CROSS_BLOCK_WIDTH = 14;
const byte CROSS_BLOCK_HEIGHT = 14;
static const unsigned char cross_block_bits[] U8X8_PROGMEM = {
  0xFF, 0x3F, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20,
  0xC1, 0x20, 0xC1, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20,
  0x01, 0x20, 0xFF, 0x3F, };
 
/////////////////////////////////////////////////////////////////////
// -MARK: 10/11: Bitmap Blending
// Pages 10 and 11: Display bitmap modes (non-transparent / transparent)
 
void display_test_bitmap_modes(byte y_offset, byte frame, bool transparent) {
  if (!transparent) {
    lander_display.setBitmapMode(false /* solid */);
    drawCenteredString(0, y_offset, "Solid bitmap");
  } else {
    lander_display.setBitmapMode(true /* transparent*/);
    drawCenteredString(0, y_offset, "Transparent bitmap");
  }
  
  y_offset += lander_display.getMaxCharHeight();  // offset down by font height
  
  const byte frame_size = CROSS_HEIGHT + 4;
  lander_display.drawBox(0, y_offset, frame_size * 5, frame_size / 2);
  
  lander_display.setDrawColor(0);  // Black
  lander_display.drawXBMP(
    frame_size * 0.5, y_offset + (frame_size / 2) - (CROSS_HEIGHT / 2),
    CROSS_WIDTH, CROSS_HEIGHT, cross_bits
  );
  
  lander_display.setDrawColor(1);  // White
  lander_display.drawXBMP(
    frame_size * 2, y_offset + (frame_size / 2) - (CROSS_HEIGHT / 2),
    CROSS_WIDTH, CROSS_HEIGHT, cross_bits
  );
  
  lander_display.setDrawColor(2);  // XOR
  lander_display.drawXBMP(
    frame_size * 3.5, y_offset + (frame_size / 2) - (CROSS_HEIGHT / 2),
    CROSS_WIDTH, CROSS_HEIGHT, cross_bits
  );
  
  lander_display.setDrawColor(1);  // restore default color
  
  lander_display.drawStr(frame_size * 0.5, y_offset + frame_size, "Black");
  lander_display.drawStr(frame_size * 2, y_offset + frame_size, "White");
  lander_display.drawStr(frame_size * 3.5, y_offset + frame_size, "XOR");
  
  if (frame == 7) {
    delay(1000);
  }
}
 
/////////////////////////////////////////////////////////////////////
// -MARK: 12: Bitmap overlay
// Pages 12: Display bitmap overlay mode
 
void display_test_bitmap_overlay(byte y_offset, byte frame) {
  byte frame_size = CROSS_FILL_HEIGHT + 4;
  byte frame_padding = 5;

  // Display the title for the overlay test
  drawCenteredString(0, y_offset, "Bitmap overlay");

  // Offset down by font height
  y_offset += lander_display.getMaxCharHeight();

  // Draw solid frame and bitmap
  lander_display.setBitmapMode(false /* solid */);
  lander_display.drawFrame(0, y_offset, frame_size, frame_size);
  lander_display.drawXBMP(
    (frame_size / 2) - (CROSS_FILL_WIDTH / 2),
    y_offset + (frame_size / 2) - (CROSS_FILL_HEIGHT / 2),
    CROSS_WIDTH, CROSS_HEIGHT, cross_bits
  );

  // If frame condition is met, draw the block cross
  if (frame & 4) {
    lander_display.drawXBMP(
      (frame_size / 2) - (CROSS_BLOCK_WIDTH / 2),
      y_offset + (frame_size / 2) - (CROSS_BLOCK_HEIGHT / 2),
      CROSS_BLOCK_WIDTH, CROSS_BLOCK_HEIGHT, cross_block_bits
    );
  }

  // Draw transparent frame and bitmap
  lander_display.setBitmapMode(true /* transparent */);
  lander_display.drawFrame(
    frame_size + frame_padding, y_offset, frame_size, frame_size
  );
  lander_display.drawXBMP(
    frame_size + frame_padding + (frame_size / 2) - (CROSS_FILL_WIDTH / 2),
    y_offset + (frame_size / 2) - (CROSS_FILL_HEIGHT / 2),
    CROSS_WIDTH, CROSS_HEIGHT, cross_bits
  );

  // If frame condition is met, draw the block cross on the transparent frame
  if (frame & 4) {
    lander_display.drawXBMP(
      frame_size + frame_padding + (frame_size / 2) - (CROSS_BLOCK_WIDTH / 2),
      y_offset + (frame_size / 2) - (CROSS_BLOCK_HEIGHT / 2),
      CROSS_BLOCK_WIDTH, CROSS_BLOCK_HEIGHT, cross_block_bits
    );
  }

  // Label indicating solid or transparent bitmap mode
  lander_display.drawStr(0, y_offset + frame_size, "Solid / transparent");

  // Delay if frame is 7
  if (frame == 7) {
    delay(1000);
  }
}