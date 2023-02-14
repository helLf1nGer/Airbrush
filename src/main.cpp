#include <Arduino.h>

#define PUL 7
#define DIR 6
#define ENA 5
#define BUTTON 3

unsigned long button_start;
unsigned long button_end;
unsigned short pulseRev = 3200;
unsigned short speed = 200;
const unsigned short s1 = 100;
const unsigned short s2 = 200;
const unsigned short s3 = 300;
const unsigned short s4 = 400;
const unsigned short s5 = 500;
unsigned int counter = 0;
unsigned int rotation = 1;
unsigned long last_push = 0;
unsigned long timer_start = 0;
unsigned short cutoff_time = 1000;

void setup() {
  pinMode(PUL, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  digitalWrite(PUL, LOW);
  digitalWrite(DIR, LOW);
  digitalWrite(ENA, HIGH);
  Serial.begin(115200);
}

void rotateMotor() {
  digitalWrite(ENA, LOW);
  digitalWrite(DIR, rotation == 1 ? LOW : HIGH);
  for (int i = 0; i < pulseRev; i++) {
    digitalWrite(PUL, HIGH);
    digitalWrite(PUL, LOW);
    delayMicroseconds(speed);
  }
  digitalWrite(ENA, HIGH);
}

void checkLongPress() {
  button_start = millis();
  while (digitalRead(BUTTON) == LOW) {
    button_end = millis();
    if (button_end - button_start >= 300) {
      rotateMotor();
    }
  }
}

void checkButtonPress() {
  static unsigned long lastButtonStateChangeTime = 0;
  static int lastButtonState = HIGH;
  int buttonState = digitalRead(BUTTON);
  unsigned long now = millis();

  if (buttonState != lastButtonState) {
    lastButtonStateChangeTime = now;
  }

  if (now - lastButtonStateChangeTime > 50) {
    if (buttonState == LOW) {
      unsigned long current_time = millis();
      if (counter == 0) {
        button_start = current_time;
        while (digitalRead(BUTTON) == LOW) {
          button_end = millis();
          if (button_end - button_start >= 300) {
            rotateMotor();
            // return;
            counter = 0;
          }
        }
        counter++;
        last_push = current_time;
        timer_start = current_time;
      } else if (counter == 1) {
        if (current_time - last_push >= cutoff_time) {
          // rotateMotor();
          counter = 0;
          return;
        } else {
          button_start = current_time;
          while (digitalRead(BUTTON) == LOW) {
            button_end = millis();
            if (button_end - button_start >= 300) {
              rotateMotor();
              counter = 0;
              // return;
            }
          }
          counter++;
          last_push = current_time;
          timer_start = current_time;
        }
      } else if (counter == 2) {
        if (current_time - last_push >= cutoff_time) {
          rotation = -rotation;
          // rotateMotor();
          counter = 0;
          return;
        } else {
          button_start = current_time;
          while (digitalRead(BUTTON) == LOW) {
            button_end = millis();
            if (button_end - button_start >= 300) {
              rotateMotor();
              counter = 0;
              // return;
            }
          }
          counter++;
          last_push = current_time;
          timer_start = current_time;
        }
      } else if (counter == 3) {
        if (current_time - last_push >= cutoff_time) {
          counter = 0;
          return;
        } else {
          unsigned long timer_elapsed = current_time - timer_start;
          if (timer_elapsed >= 6000) {
            counter = 0;
            return;
          } else {
            if (timer_elapsed >= 50 && timer_elapsed < 1000) {
              speed = s1;
            } else if (timer_elapsed >= 1000 && timer_elapsed < 2000) {
              speed = s2;
            } else if (timer_elapsed >= 2000 && timer_elapsed < 3000) {
              speed = s3;
            } else if (timer_elapsed >= 3000 && timer_elapsed < 4000) {
              speed = s4;
            } else if (timer_elapsed >= 4000 && timer_elapsed < 6000) {
              speed = s5;
            }
          }
        }
      }
    }
  }

  lastButtonState = buttonState;
}

void loop() {
  checkButtonPress();
  // checkLongPress();
  Serial.println("counter: " + String(counter) + " speed: " + String(speed) +
                 " rotation: " + rotation);
}
