#include <Arduino.h>

#define pin1 0
#define pin2 1

void pinInterrupt(void);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);

  attachInterrupt(pin1, pinInterrupt, CHANGE);
}

volatile int change = 0;
int count = 0;
boolean consecutive = false;
int integrator = 0;
long last_change_time = 0;

void loop() {
  // put your main code here, to run repeatedly:
  long time = micros(); // once to keep consistent timing over the loop

  if (change != 0) {
    integrator = integrator+1;
    int expo = floor(integrator/10.0);
    count = count+change*pow(10, expo);
    change = 0;
    last_change_time = micros() + 100000; // one tenth second to reset exponential acceleration
  }

  if (time > last_change_time) {
    integrator = 0;
  }
  Serial.println(count);

  delay(10);
}

void pinInterrupt(void) {
  boolean a = digitalReadFast(pin1);
  boolean b = digitalReadFast(pin2);

  if (a == b) {
    change = -1;
  } else {
    change = +1;
  }
}