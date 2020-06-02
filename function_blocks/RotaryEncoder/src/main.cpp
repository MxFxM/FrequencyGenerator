#include <Arduino.h>

#define RotaryEncoderPin1 0
#define RotaryEncoderPin2 1

void isr_rotary_pin1(void);
void isr_rotary_pin2(void);

void setup() {
  // put your setup code here, to run once:

  pinMode(RotaryEncoderPin1, INPUT_PULLUP);
  pinMode(RotaryEncoderPin2, INPUT_PULLUP);

  Serial.begin(9600); // usb is always 12 Mbit/s

  attachInterrupt(RotaryEncoderPin1, isr_rotary_pin1, FALLING);
  attachInterrupt(RotaryEncoderPin2, isr_rotary_pin2, FALLING);
}

volatile boolean first = first;
int rotary_change = 0;
uint16_t counter = 0;

void loop() {
  // put your main code here, to run repeatedly:
  delay(10);
  if (rotary_change != 0) {
    counter = counter + rotary_change;
    rotary_change = 0;
    Serial.println(counter);
  }
}

void isr_rotary_pin1(void) {
  //
  if (first) { // read slow as some sort of debounce
    rotary_change = 1;
  } else {
    rotary_change = -1;
    first = true;
  }
}

void isr_rotary_pin2(void) {
  //
  first = false;
}