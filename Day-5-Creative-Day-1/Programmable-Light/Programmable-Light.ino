// Light pin constants
const byte LIGHT_RED = 10;
const byte LIGHT_WHITE = 11;
const byte LIGHT_BLUE = 12;

// Switch pin constants
const byte PIN_0 = 2;
const byte PIN_1 = 3;
const byte PIN_2 = 4;

// Sequence Constants
const int POLICE_LIGHTS = 3;
const int TRAFFIC_LIGHTS = 4;

void setup() {
  // Light Pins
  pinMode(LIGHT_RED, OUTPUT);    // pin controlling the cabin lights
  pinMode(LIGHT_WHITE, OUTPUT);  // pin controlling the storage lights
  pinMode(LIGHT_BLUE, OUTPUT);  // pin controlling the exterior lights

  // Switch pins
  pinMode(PIN_0, INPUT);    // pin connected to switch 1 (cabin lights)
  pinMode(PIN_1, INPUT);  // pin connected to switch 2 (storage lights)
  pinMode(PIN_2, INPUT);  // pin connected to switch 3 (exterior lights)
}

// Functions ↓

// Utilities
void controlLight(int light, int pin) {
  if (pin == HIGH) {
    digitalWrite(light, HIGH);  // Switch is ON, turn on our lander's light
  } else {
    digitalWrite(light, LOW);  // Switch is OFF, turn off lander's light
  }
}
int readDIP() {

  // Reading the DIP switch as binary
  int bit0 = digitalRead(PIN_2);
  int bit1 = digitalRead(PIN_1);
  int bit2 = digitalRead(PIN_0);


  // Turn on light
  // controlLight(LIGHT_RED, bit0);
  // controlLight(LIGHT_WHITE, bit1);
  // controlLight(LIGHT_BLUE, bit2);

  // Converting binary to decimal
  int number = 1 * bit0 + 2 * bit1 + 4 * bit2;
  return number;
}

void allOff() {
  digitalWrite(LIGHT_BLUE, LOW);
  digitalWrite(LIGHT_WHITE, LOW);
  digitalWrite(LIGHT_RED, LOW);
}

void doubleFlash(
  byte pin,
  unsigned long delayMs
) {
  digitalWrite(pin, HIGH);
  delay(50);
  digitalWrite(pin, LOW);
  delay(50);
  digitalWrite(pin, HIGH);
  delay(50);
  digitalWrite(pin, LOW);
  delay(delayMs);
}

void flash(
  byte pin,
  unsigned long delayMs
) {
  digitalWrite(pin, HIGH);
  delay(delayMs);
  digitalWrite(pin, LOW);
}

// Sequences
void policeLight() {
  doubleFlash(LIGHT_RED, 100);
  doubleFlash(LIGHT_BLUE, 100);
}

void trafficLight() {
  flash(LIGHT_RED, 5000);
  flash(LIGHT_WHITE, 30);
  flash(LIGHT_BLUE, 5000);
  flash(LIGHT_WHITE, 1000);
}

void sequenceChooser(int sequence) {
  allOff();
  switch(sequence) {
    case 0:
      digitalWrite(LIGHT_RED, HIGH);
      break;

    case 1: 
      digitalWrite(LIGHT_WHITE, HIGH);
      break;

    case 2:
      digitalWrite(LIGHT_BLUE, HIGH);
      break;

    case POLICE_LIGHTS:
      policeLight();
      break;

    case TRAFFIC_LIGHTS:
      trafficLight();
      break;

    default:
      allOff();
  }
}

void loop() {
  int sequence = readDIP();
  sequenceChooser(sequence);
}