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
volatile int count = 0;
volatile int integrator = 0;

void loop() {
  // put your main code here, to run repeatedly:

  integrator += change;
  if (abs(integrator) > 10) {
    change = change*10;
  }
  if (abs(integrator) > 20) {
    change = change*10;
  }
  if (abs(integrator) > 30) {
    change = change*10;
  }
  if (abs(integrator) > 40) {
    change = change*10;
  }
  count = count+change;
  change = 0;
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