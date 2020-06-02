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
int last_change = 0;

void loop() {
  // put your main code here, to run repeatedly:
  long time = micros(); // once to keep consistent timing over the loop

  if (change != 0) {
    // reset acceleration if direction was reversed
    if (change != last_change) {
      integrator = 0;
      last_change = change;
    }

    // increase integrator
    integrator = integrator+1;

    // make integrator exponential
    int expo = floor(integrator/10.0);

    // limit exponent
    if (expo >= 5) { // not faster than 100k per count. thats fast enough for a value 0 < x < 20M
      expo = 5;
    }

    // increase count
    count = count+change*pow(10, expo);

    // limit count
    if (count >= 20000000) {
      count = 20000000;
    } else if (count <= 0) {
      count = 0;
    }

    // reset change
    change = 0;

    // save time for reset of integrator
    last_change_time = time + 200000; // one fith second to reset exponential acceleration
  }

  // reset integrator if time is over
  if (time > last_change_time) {
    integrator = 0;
  }

  // print value
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