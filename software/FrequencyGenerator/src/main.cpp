#include <Arduino.h>

// include the library code:
#include <LiquidCrystal.h>
// the library uses delays but for my application that does not matter

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);




#define pin1 0
#define pin2 1

#define pinBuzzer 7

void pinInterrupt(void);


void printFrequency(int freq);

void setup() {
  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);

  pinMode(pinBuzzer, OUTPUT);

  attachInterrupt(pin1, pinInterrupt, CHANGE);
}

volatile int change = 0;
int count = 1;
boolean consecutive = false;
int integrator = 0;
long last_change_time = 0;
int last_change = 0;
boolean buzzerState = false;

long last_display_update = 0;

void loop() {
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
    } else if (count <= 1) {
      count = 1;
    }

    // reset change
    change = 0;

    // save time for reset of integrator
    last_change_time = time + 500000; // one half second to reset exponential acceleration

    // toggle the buzzer
    buzzerState = !buzzerState;
    digitalWriteFast(pinBuzzer, buzzerState);
  }

  // reset integrator if time is over
  if (time > last_change_time) {
    integrator = 0;
  }

  if (time > last_display_update) {
    // print value
    printFrequency(count);

    last_display_update = time + 200000;
    last_change_time = last_change_time + 200000; // more time since display update takes a while
  }

  delay(10);
}

void printFrequency(int freq) {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Frequency:");

  // set cursor on second line
  lcd.setCursor(0, 1);

  int freq_temp = freq;

  int millions = 0;
  int thousands = 0;
  int ones = 0;

  if (freq_temp >= 1000000) {
    millions = floor(freq_temp / 1000000);
  }

  freq_temp = freq_temp - millions * 1000000;

  if (freq_temp >= 1000) {
    thousands = floor(freq_temp / 1000);
  }

  freq_temp = freq_temp - thousands * 1000;

  ones = freq_temp;


  if (millions != 0) {
    // leading blank spaces
    if (millions >= 10) { // no more than 20M anyway
      lcd.print("   ");
    } else {
      lcd.print("    ");
    }
    // millions
    lcd.print(millions);
    // millions decimal point
    lcd.print(".");
  } else {
    // leading blank spaces, empty millions and no decimal point
    lcd.print("      ");
  }

  if (thousands != 0) {
    // leading blank spaces, if millions is 0, otherwise leading zeroes
    if (millions == 0) {
      if (thousands < 100) {
        lcd.print(" ");
      }
      if (thousands < 10) {
        lcd.print(" ");
      }
    } else {
      if (thousands < 100) {
        lcd.print("0");
      }
      if (thousands < 10) {
        lcd.print("0");
      }
    }
    // thousands
    lcd.print(thousands);
    // thousands decimal point
    lcd.print(".");
  } else {
    if (millions != 0) {
      lcd.print("000.");
    } else {
      lcd.print("    ");
    }
  }

  if (ones != 0) {
    // leading blank spaces, if millions and thousands are 0, otherwise leading zeroes
    if (millions == 0 && thousands == 0) {
      if (ones < 100) {
        lcd.print(" ");
      }
      if (ones < 10) {
        lcd.print(" ");
      }
    } else {
      if (ones < 100) {
        lcd.print("0");
      }
      if (ones < 10) {
        lcd.print("0");
      }
    }
    // ones
    lcd.print(ones);
  } else {
    if (millions != 0 || thousands != 0) {
      lcd.print("000");
    }
  }

  // lcd.print(freq);
  lcd.print(" Hz");
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