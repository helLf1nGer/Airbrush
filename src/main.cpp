#include <Arduino.h>
// Define the input and output pins for the stepper motor and button
#define PUL 7
#define DIR 6
#define ENA 5
#define BUTTON 3
// Declare variables for the button press, rotation, speed, and timer
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
  // Set the pin modes for the stepper motor and button
  pinMode(PUL, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
// Set the initial digital output values for the stepper motor pins
  digitalWrite(PUL, LOW);
  digitalWrite(DIR, LOW);
  digitalWrite(ENA, HIGH);
  // Initialize the serial communication baud rate to 115200 for debugging
  Serial.begin(115200);

}
// Function to rotate the motor
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
//Check if the button is pressed
void checkButtonPress() {
  static unsigned long lastButtonStateChangeTime = 0;
  static int lastButtonState = HIGH;
  int buttonState = digitalRead(BUTTON);
  unsigned long now = millis();
// Check if the button state has changed
  if (buttonState != lastButtonState) {
    lastButtonStateChangeTime = now;
  }
// Check if the button has been pressed for more than 50ms - debounce the button
  if (now - lastButtonStateChangeTime > 50) {
    if (buttonState == LOW) {
      unsigned long current_time = millis();
      if (counter == 0) {
        button_start = current_time;
        counter++;
        //rotate the motor if the button is pressed for more than 300ms
        while (digitalRead(BUTTON) == LOW) {
          button_end = millis();
          if (button_end - button_start >= 300) {
            rotateMotor();
            counter = 0;
            // return;
          }
        }
        //save the time of the last button press and the time the timer started
        last_push = current_time;
        timer_start = current_time;
        //logic to check if the button is pressed twice or thrice
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
        //if the button is pressed twice, check if the button is pressed again and if not change the direction of rotation
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
        //if the button is pressed thrice, check if the button is pressed again and if not set counter to 0, else set the speed of rotation based on the time the button is pressed
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
  //debugging section
  Serial.println("counter: " + String(counter) + " speed: " + String(speed) +
                 " rotation: " + rotation);
}