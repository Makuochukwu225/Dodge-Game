#include <LedControl.h>

#define DIN 11
#define CS 10
#define CLK 13

LedControl lc = LedControl(DIN, CLK, CS, 1);

// Joystick pins
#define VRX A0
#define VRY A1
#define BTN 2  // Joystick button (reset)

int playerX = 3;
int objectX = 0;
int objectY = 0;

bool gameOver = false;
unsigned long lastUpdate = 0;
const int fallInterval = 300; // Object fall speed

// Joystick movement limiter
unsigned long lastMoveTime = 0;
const int moveCooldown = 150; // ms between joystick moves

void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

  pinMode(VRX, INPUT);
  pinMode(VRY, INPUT);
  pinMode(BTN, INPUT_PULLUP); // Use internal pull-up

  randomSeed(analogRead(0));
  resetGame();
}

void loop() {
  if (gameOver) {
    showGameOver();
    if (digitalRead(BTN) == LOW) {
      delay(200); // debounce
      resetGame();
    }
    return;
  }

  handleInput();

  if (millis() - lastUpdate >= fallInterval) {
    lastUpdate = millis();
    lc.clearDisplay(0);

    objectY++;

    if (objectY == 7) {
      if (objectX == playerX) {
        // Caught object — restart it
        objectY = 0;
        objectX = random(0, 8);
      } else {
        // Missed — game over
        gameOver = true;
        return;
      }
    }

    // Draw falling object (corrected orientation)
    if (objectY < 8) {
      lc.setLed(0, objectX, objectY, true);
    }
  }

  // Draw player (bottom row, corrected orientation)
  lc.setLed(0, playerX, 7, true);

  delay(30); // smooth movement rendering
}

void handleInput() {
  int xVal = analogRead(VRX);
  unsigned long now = millis();

  if (now - lastMoveTime >= moveCooldown) {
    if (xVal < 400) {
      playerX = max(0, playerX - 1);
      lastMoveTime = now;
    }
    else if (xVal > 600) {
      playerX = min(7, playerX + 1);
      lastMoveTime = now;
    }
  }
}

void showGameOver() {
  lc.clearDisplay(0);
  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, B11111111); // flash full matrix
  }
}

void resetGame() {
  gameOver = false;
  objectY = 0;
  objectX = random(0, 8);
  playerX = 3;
  lastUpdate = millis();
  lastMoveTime = millis();
  lc.clearDisplay(0);
}
