// Libs
#include <Arduino.h>
#include <U8g2lib.h>
#include <BasicEncoder.h>

// Pins
// Interrupts are supported on pins 2 and 3.
constexpr byte ScrollerClkPin = 2;
constexpr byte ScrollerDtPin = 3;
constexpr byte ScrollerSwPin = 4;

// Hardware
U8G2_SH1106_128X64_NONAME_2_HW_I2C statusDisplay(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
BasicEncoder scroller(ScrollerClkPin, ScrollerDtPin);

// Types
struct Status {
  const char* title;
  const char* description;
};

// Consts
const Status Statuses[] = {
  {
    "Do not Disturb",
    "Don't disturb me, I'm solving world hunger."
  },
  {
    "Deep Focus",
    "Please don’t — I’m thinking in 4D."
  },
  {
    "Coffee Break",
    "Getting caffeinated. Back soon!"
  },
  {
    "Out for a Walk",
    "Stretching legs + brain."
  },
  {
    "Vibing",
    "Headphones ON, world OFF."
  },
  {
    "In a Meeting",
    "Probably nodding and smiling."
  },
  {
    "Lurking",
    "I'm here... just watching."
  },
  {
    "Creative Mode",
    "Color palettes and pixels await."
  },
  {
    "Debugging",
    "Sacrificing sleep to fix semicolons."
  },
  {
    "Nap Time",
    "Gone to recharge. Literally."
  },
  {
    "Gaming",
    "Saving the world... virtually."
  },
  {
    "Studying",
    "Brain.exe is loading..."
  },
  {
    "AFK",
    "Not dead, just away from keyboard."
  },
  {
    "Writing",
    "Crafting characters or variables."
  },
  {
    "Idle",
    "Still here, just not doing much."
  },
  {
    "Cleaning",
    "IRL garbage collection."
  },
  {
    "Brainstorming",
    "May contain lightning bolts."
  },
  {
    "No Talkie",
    "Social battery: 0%"
  },
  {
    "Existential Mode",
    "What even is code, bro?"
  },
  {
    "Self Care",
    "Face mask on. Zen mode engaged."
  }
};

constexpr int NumTotalStatuses = sizeof(Statuses) / sizeof(Statuses[0]);
constexpr int NumVisibleLines = 5;

// Global Variables
int prevScrollPos = 0;
int scrollIndex = 0;
int cursorIndex = 0;
int isDisplayingStatus = false;

// Functions
void drawMessages();
void displayStatus(Status status);

bool isButtonPressed() {
  if (digitalRead(ScrollerSwPin) == LOW) {
    delay(50); // Basic debounce
    if (digitalRead(ScrollerSwPin) == LOW) { // Ensure button is still pressed
      return true;
    }
  }
  return false;
}

void updateScroller() {
  scroller.service();
}

void drawDisplay() {
  // statusDisplay.clearBuffer();
  // statusDisplay.setDrawColor(1);

  if (isDisplayingStatus) {
    displayStatus(Statuses[scrollIndex + cursorIndex]);
  } else {
    drawMessages();
  }
}

void drawMessages() {
  statusDisplay.firstPage();
  do {
    const int fontHeight = statusDisplay.getMaxCharHeight(); // should be 8 for 5x8 font

    for (int i = 0; i < NumVisibleLines; i++) {
      constexpr int lineHeight = 12;
      const int statusIndex = scrollIndex + i;
      if (statusIndex >= NumTotalStatuses) break;

      const int y = (i * lineHeight) + 2;

      // Draw highlight for selected item (top visible item)
      if (i == cursorIndex) {
        constexpr int boxPadding = 1;
        const int boxHeight = fontHeight + (boxPadding * 2);
        const int boxWidth = statusDisplay.getDisplayWidth(); // full width
        statusDisplay.drawRBox(
          0, y - boxPadding,
          boxWidth, boxHeight,
          4 // radius
        );
        // Invert text color for better contrast
        statusDisplay.setDrawColor(0); // Invert text inside highlight
        statusDisplay.drawStr(2, y, Statuses[statusIndex].title);
        statusDisplay.setDrawColor(1); // Reset to normal
      } else {
        statusDisplay.drawStr(2, y, Statuses[statusIndex].title);
      }
    }
  } while (statusDisplay.nextPage());
}

void displayStatus(const Status status) {
  statusDisplay.firstPage();
  do {
    constexpr int boxPadding = 2;
    int y = 0;

    // Set and measure title font
    statusDisplay.setFont(u8g2_font_6x12_tr); // Bigger font for title
    const int titleFontHeight = statusDisplay.getMaxCharHeight();
    const int titleBoxHeight = titleFontHeight + (boxPadding * 2);

    // Draw rounded rectangle behind title
    statusDisplay.drawRBox(
      0, y,
      statusDisplay.getDisplayWidth(), titleBoxHeight,
      4 // Corner radius
    );

    // Draw title (with inverted color for contrast inside box)
    statusDisplay.setDrawColor(0);
    statusDisplay.drawStr(2, y + boxPadding, status.title);
    statusDisplay.setDrawColor(1);

    // Set font for description
    statusDisplay.setFont(u8g2_font_6x10_tr); // Smaller font for description
    y += titleBoxHeight + 4; // Add some spacing

    // Draw description
    statusDisplay.drawStr(2, y, status.description);

  } while (statusDisplay.nextPage());
}

void setup() {
  Serial.begin(9600);

  // Initialize Hardware
  statusDisplay.begin();
  statusDisplay.setFont(u8g2_font_6x10_tr);
  statusDisplay.setFontRefHeightText();
  statusDisplay.setFontPosTop();
  statusDisplay.setDrawColor(1);

  scroller.begin();
  drawMessages();

  pinMode(ScrollerSwPin, INPUT_PULLUP);

  // Interrupts
  attachInterrupt(
    digitalPinToInterrupt(ScrollerClkPin),
    updateScroller, CHANGE
  );
  attachInterrupt(
    digitalPinToInterrupt(ScrollerDtPin),
    updateScroller, CHANGE
  );
}

void loop() {
  drawDisplay();

  if (scroller.get_change()) {
    const int newScrollerPos = scroller.get_count();
    const int delta = newScrollerPos - prevScrollPos;

    if (delta > 0) {
      // Scrolling down
      if (cursorIndex < NumVisibleLines - 1 &&
          scrollIndex + cursorIndex + 1 < NumTotalStatuses) {
        cursorIndex++;
      } else if (scrollIndex + NumVisibleLines < NumTotalStatuses) {
        scrollIndex++;
      }
    } else if (delta < 0) {
      // Scrolling up
      if (cursorIndex > 0) {
        cursorIndex--;
      } else if (scrollIndex > 0) {
        scrollIndex--;
      }
    }

    prevScrollPos = newScrollerPos;
  }

  if (isButtonPressed()) {
    Serial.println("Button pressed!");
    isDisplayingStatus = !isDisplayingStatus;
  }
}
