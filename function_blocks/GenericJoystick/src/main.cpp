#include <Arduino.h>

#define JoystickX A0
#define JoystickY A1
#define JoystickSW 0

void setup() {
  // put your setup code here, to run once:

  pinMode(JoystickX, INPUT);
  pinMode(JoystickY, INPUT);
  pinMode(JoystickSW, INPUT_PULLDOWN);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  int raw_joystick_x = analogRead(JoystickX);
  int raw_joystick_y = analogRead(JoystickY);
  boolean val_joystick_sw = digitalReadFast(JoystickSW);

  int val_joystick_x = map(raw_joystick_x, 0, 65000, -5, 5);
  int val_joystick_y = map(raw_joystick_y, 0, 65000, -5, 5);

  Serial.print("X=");
  Serial.print(val_joystick_x);
  Serial.print("   Y=");
  Serial.print(val_joystick_y);
  Serial.print("   SW=");
  Serial.print(val_joystick_sw);
  Serial.println();
}