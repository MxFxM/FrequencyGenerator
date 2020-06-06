#include <Arduino.h>

#define ENC_A 1
#define ENC_B 2

void read_encoder(void);

volatile int counter;
volatile boolean tick;

void setup()
{
  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT);

  Serial.begin(9600);
  Serial.println("Start");

  counter = 0;
  tick = false;

  attachInterrupt(ENC_A, read_encoder, CHANGE);
  attachInterrupt(ENC_B, read_encoder, CHANGE);
}

void loop()
{
  // do some stuff here - the joy of interrupts is that they take care of themselves
  Serial.println(counter);
  delay(100);
}

void read_encoder()
{
  static uint8_t previousState = 3;

  uint8_t newA = digitalReadFast(ENC_A);
  uint8_t newB = digitalReadFast(ENC_B);

  uint8_t newState = (newA << 1) | newB;

  if (newState != previousState) {
    if (newState == 3 && previousState == 2 && tick) {
      counter = counter + 1;
      tick = false;
    } else

    if (newState == 3 && previousState == 1 && tick) {
      counter = counter - 1;
      tick = false;
    } else

    if (newState == 2 && previousState == 0 && !tick) {
      tick = true;
    } else

    if (newState == 1 && previousState == 0 && !tick) {
      tick = true;
    } else

    {
      tick = false;
    }

    previousState = newState;
  }
}