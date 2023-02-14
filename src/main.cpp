#include <Arduino.h>

#include <Arduino.h>

#define PUL 7
#define DIR 6
#define ENA 5
#define BUTTON 3

unsigned long button_start;
unsigned long button_end;
unsigned short pulseRev = 3200;
unsigned short speed = 200;

void setup() {
  pinMode(PUL, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  digitalWrite(PUL, LOW);
  digitalWrite(DIR, LOW);
  digitalWrite(ENA, HIGH);
}

void rotateMotor() {
  digitalWrite(ENA, LOW);
  for (int i = 0; i < pulseRev; i++) {
    digitalWrite(PUL, HIGH);
    digitalWrite(PUL, LOW);
    delayMicroseconds(speed);
  }
  digitalWrite(ENA, HIGH);
  }

void checkLongPress() {
  button_start = millis();

  // Wait until the pushbutton is released
  while (digitalRead(BUTTON) == LOW) {
    // Check if the pushbutton has been pushed for more than 300 milliseconds
    button_end = millis();
    if (button_end - button_start >= 300) {
      // Invoke the rotateMotor function
      rotateMotor();
    }
  }
}

void loop() {
  // Check if the pushbutton is being pushed
  if (digitalRead(BUTTON) == LOW) {
    checkLongPress();
  }
}
