#include <Arduino.h>

#define ROTARY_PIN_1 0
#define ROTARY_PIN_2 1

#define BUZZER_PIN 2

volatile boolean b_pin1_first = false;
volatile boolean b_pin2_first = false;
volatile boolean consecutive = false;
volatile boolean updateDisplay = false;
volatile long last_change_time = 0;
volatile int last_change = 0;
volatile int count = 0;
volatile int integrator = 0;

boolean buzzerState = false;

void ISR_RotaryPin1(void);
void ISR_RotaryPin2(void);
void CounterUpdate(int change);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(ROTARY_PIN_1, INPUT);
  pinMode(ROTARY_PIN_2, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);

  attachInterrupt(ROTARY_PIN_1, ISR_RotaryPin1, RISING);
  attachInterrupt(ROTARY_PIN_2, ISR_RotaryPin2, RISING);
}

void loop()
{
  // put your main code here, to run repeatedly:
  long time = micros(); // once to keep consistent timing over the loop

  if (updateDisplay)
  {
    // toggle the buzzer
    buzzerState = !buzzerState;
    digitalWriteFast(BUZZER_PIN, buzzerState);

    // print value
    Serial.println(count);

    updateDisplay = false;
  }

  delay(100);
}

void ISR_RotaryPin1(void)
{
  if (!b_pin2_first)
  {
    b_pin1_first = true;
  }
  else
  {
    CounterUpdate(+1);
    b_pin2_first = false;
  }
}

void ISR_RotaryPin2(void)
{
  if (!b_pin1_first)
  {
    b_pin2_first = true;
  }
  else
  {
    CounterUpdate(-1);
    b_pin1_first = false;
  }
}

void CounterUpdate(int change)
{
  // reset acceleration if direction was reversed
  if (change != last_change)
  {
    integrator = 0;
    last_change = change;
  }

  // increase integrator
  integrator = integrator + 1;

  // make integrator exponential
  int expo = floor(integrator / 10.0);

  // limit exponent
  if (expo >= 5)
  { // not faster than 100k per count. thats fast enough for a value 0 < x < 20M
    expo = 5;
  }

  // increase count
  count = count + change * pow(10, expo);

  // limit count
  if (count >= 20000000)
  {
    count = 20000000;
  }
  else if (count <= 0)
  {
    count = 0;
  }

  // reset change
  change = 0;

  long time = micros();

  // save time for reset of integrator
  last_change_time = time + 200000; // one fith second to reset exponential acceleration

  // reset integrator if time is over
  if (time > last_change_time)
  {
    integrator = 0;
  }

  updateDisplay = true;
}