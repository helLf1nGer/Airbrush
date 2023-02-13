#define buttonPin 3
#define potSW 12
#define directionPin 5
#define stepPin 7
#define enablePin 13
#define potCLK 10
#define potDT 11

#include <Arduino.h>

int buttonState = 0;
int direction = 1;
int rotationDuration = 2;  // Default rotation duration
int steps = 200;  // Number of steps per rotation
int potValue = 0;

void potentAdjustment() {
  int newPotValue = 0;
  for (int i = 0; i < 1023; i++) {
    digitalWrite(potCLK, LOW);
    digitalWrite(potCLK, HIGH);
    newPotValue += digitalRead(potDT);
  }
  int threshold = 500;  // Change threshold
  int change = newPotValue - potValue;
  if (abs(change) > threshold) {
    potValue = newPotValue;
    rotationDuration = map(potValue, 0, 1023, 1, 10);  // Update the rotation duration
    Serial.print("Rotation duration: ");
    Serial.print(rotationDuration);
    Serial.println(" seconds");
    Serial.print("Potentiometer value: ");
    Serial.println(potValue);
  }
  // Check if the potentiometer was clicked
  if (digitalRead(potSW) == LOW) {
    // Change the direction of rotation
    direction = -direction;
    digitalWrite(directionPin, direction > 0 ? HIGH : LOW);
  }
}


void rotateMotor(int duration) {
  digitalWrite(enablePin, LOW);  // Enable the motor
  for (int i = 0; i < duration * steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
  digitalWrite(enablePin, HIGH);  // Disable the motor
}

void pushButton() {
  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
    //potentAdjustment();  // Adjust the potentiometer
    rotateMotor(rotationDuration * direction);  // Rotate the motor
  }
}


void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(directionPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(potSW, INPUT_PULLUP);
  digitalWrite(enablePin, HIGH);  // Disable the motor by default
  Serial.begin(115200);
}

void loop() {
  pushButton();
  potentAdjustment();
  delay(10);
}
