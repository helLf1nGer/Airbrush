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
const unsigned short s3 = 400;
const unsigned short s4 = 800;
const unsigned short s5 = 1600;
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
        counter++;
        while (digitalRead(BUTTON) == LOW) {
          button_end = millis();
          if (button_end - button_start >= 300) {
            rotateMotor();
            counter = 0;
            // return;
          }
        }
        last_push = current_time;
        timer_start = current_time;
      } else if (counter == 1) {
        if (current_time - last_push >= cutoff_time) {
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
        if (current_time - last_push >= cutoff_time / 2) {
          rotation = -rotation;
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
        if (current_time - last_push >= 6000) {
          counter = 0;
          return;
        } else if (digitalRead(BUTTON) == LOW) { // check if button is pressed
          unsigned long press_start = millis();
          while (digitalRead(BUTTON) == LOW) {
            // loop until button is released
          }
          unsigned long press_duration = millis() - press_start;
          if (press_duration < 1000) {
            speed = s1;
          } else if (press_duration < 2000) {
            speed = s2;
          } else if (press_duration < 3000) {
            speed = s3;
          } else if (press_duration < 4000) {
            speed = s4;
          } else {
            speed = s5;
          }
          counter = 0; // reset the counter after setting the speed
          return;
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