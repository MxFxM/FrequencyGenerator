#include <Arduino.h>

// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void printFrequency(int freq);

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Frequency:");
}

void loop() {

  int fake_freq = micros();

  fake_freq = fake_freq % 20000000; // limit to 20 Mhz

  printFrequency(fake_freq);

  delay(500);
}

void printFrequency(int freq) {
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
  } // there is no else, since this cant be 0

  // lcd.print(freq);
  lcd.print(" Hz");
}