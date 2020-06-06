#include <Arduino.h>

#define ENC_A 1
#define ENC_B 2
#define ENC_Btn 0

#define MAX_COUNTER_VALUE 10000000
#define MIN_COUNTER_VALUE 1

void read_encoder(void);
void confirm_frequency(void);

volatile int counter = 1;
volatile int previousCounter = 0;

void setup()
{
  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT);
  pinMode(ENC_Btn, INPUT_PULLUP);

  Serial.begin(9600);

  attachInterrupt(ENC_A, read_encoder, CHANGE);
  attachInterrupt(ENC_B, read_encoder, CHANGE);
  attachInterrupt(ENC_Btn, confirm_frequency, FALLING);
}

void loop()
{
  if (counter != previousCounter) {
    previousCounter = counter;
    Serial.println(counter);
  }
}

void read_encoder()
{
  static uint8_t previousState = 3;
  static boolean tick = false;

  uint8_t newA = digitalReadFast(ENC_A);
  uint8_t newB = digitalReadFast(ENC_B);

  uint8_t newState = (newA << 1) | newB;

  if (newState != previousState) {
    if (newState == 3 && previousState == 2 && tick) {
      counter = counter + 1;
      if (counter >= MAX_COUNTER_VALUE) {
        counter = MAX_COUNTER_VALUE;
      }
      tick = false;
    } else if (newState == 3 && previousState == 1 && tick) {
      counter = counter - 1;
      if (counter <= MIN_COUNTER_VALUE) {
        counter = MIN_COUNTER_VALUE;
      }
      tick = false;
    } else if (newState == 2 && previousState == 0 && !tick) {
      tick = true;
    } else if (newState == 1 && previousState == 0 && !tick) {
      tick = true;
    } else {
      tick = false;
    }

    previousState = newState;
  }
}

void confirm_frequency() {
  static long lastConfirm = 0;
  long time = micros();

  if (time > lastConfirm + 100000) {
    Serial.print("Confirmed: ");
    Serial.println(counter);
    lastConfirm = time;
  }
}