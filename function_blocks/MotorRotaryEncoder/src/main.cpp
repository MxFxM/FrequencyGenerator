#include <Arduino.h>

#define pin1 0
#define pin2 1

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  boolean a = digitalReadFast(pin1);
  boolean b = digitalReadFast(pin2);

  Serial.print("Pin1: ");
  Serial.print(a);
  Serial.print("    Pin2: ");
  Serial.print(b);
  Serial.println();

  delay(10);
}