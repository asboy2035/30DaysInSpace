// -MARK: Consts, structs
#include <Arduino.h>
#include <stdlib.h>
#include <Wire.h>
#include <U8g2lib.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

typedef struct Point {
  byte x;
  byte y;
};

typedef struct Circle {
  Point center;
  byte radius;
};

typedef struct Box {
  Point topLeft;
  byte width;
  byte height;
};

// -MARK: Funcs
void drawDisc(
  U8G2_SH1106_128X64_NONAME_F_HW_I2C& display,
  Circle circle
) {
  display.drawDisc(
    circle.center.x,
    circle.center.y,
    circle.radius
  );
}

// void bounceCircle(
//   U8G2_SH1106_128X64_NONAME_F_HW_I2C& display,
//   Circle circle,
//   int steps,
//   int stepSize,
//   int stepDelay
// ) {
//   for (int i = 0; i <= steps; i++) {
//     circle.center.y -= stepSize;
//     drawDisc(display, circle);
//     display.sendBuffer();

//     delay(stepDelay + steps/(i+1));
//     display.clearBuffer();
//   }

//   for (int i = 0; i <= steps; i++) {
//     circle.center.y += stepSize;
//     drawDisc(display, circle);
//     display.sendBuffer();

//     int offset = stepDelay + steps - i*3;
//     if (offset <= 0) {
//       offset = 10;
//     }
//     Serial.println(offset);
//     delay(offset);
//     display.clearBuffer();
//   }
// }


float fallDown(
  Circle circle,
  int maxBounces,
  float gravity,
  int stepDelay
) {
  float velocity = 0;
  float position = circle.center.y;
  float floor = position;
  float timeStep = 0.75;  // Approximate time per frame
  
  Serial.println("Before for loop");
  for (int bounce = 0; bounce < maxBounces; bounce++) {
    // Update velocity with acceleration (gravity)
    // v = u + at
    velocity += gravity * timeStep;
    Serial.println("Velocity: " + String(velocity));
    
    // Update position with velocity
    // s = s0 + 0.5at(squared)
    position += 0.5 * velocity * timeStep;
    Serial.println("Pos: " + String(position));
    
    // Check for collision with the floor
    if (position + circle.radius >= 60) {
      break;
    }

    // Update circle's position
    circle.center.y = (int)position;
    Serial.println(circle.center.y);

    // Draw and update display
    display.clearBuffer();
    drawDisc(display, circle);
    display.sendBuffer();
    delay(stepDelay);
  }
  
  return velocity;
}

void bounceUp(
  Circle circle,
  int maxBounces,
  float gravity,
  float startingPosition,
  float velocity,
  int stepDelay
) {
  // float velocity = 0;
  float position = startingPosition;
  float timeStep = 0.75;  // Approximate time per frame

  Serial.println("Before for loop");
  for (int bounce = 0; bounce < maxBounces; bounce++) {
    // Update velocity with acceleration (gravity)
    // v = u + at
    velocity -= gravity * timeStep;

    if (velocity <= 0) {
      break;
    }

    Serial.println("Velocity: " + String(velocity));
    
    // Update position with velocity
    // s = s0 + 0.5at(squared)
    position -= 0.5 * velocity * timeStep;
    Serial.println("Pos: " + String(position));
    
    // Check for collision with the ceiling
    if (position - circle.radius <= 0) {
      break;
    }

    // Update circle's position
    circle.center.y = (int)position;
    Serial.println(circle.center.y);

    // Draw and update display
    display.clearBuffer();
    drawDisc(display, circle);
    display.sendBuffer();
    delay(stepDelay);
  }    
}

void bounceCircle(
  U8G2_SH1106_128X64_NONAME_F_HW_I2C& display,
  Circle circle,
  int maxBounces,
  float gravity,
  float energyLoss,
  int stepDelay
) {  
  Serial.println("Circle Y 1: " + String(circle.center.y));

  float finalVelocity = fallDown(circle, maxBounces, gravity, stepDelay);
  bounceUp(circle, maxBounces, gravity, 55.0, finalVelocity, stepDelay);
  
  Serial.println("Circle Y 2: " + String(circle.center.y));
}
// -MARK: Main
void setup() {
  display.begin();
  Serial.begin(9600);
}

void loop() {
  static Circle circle1 = {
    { 64, 10 },
    5
  };

  // drawDisc(display, circle1);
  bounceCircle(display, circle1, 100, 0.8, 0.7, 30);
  delay(250);
}
