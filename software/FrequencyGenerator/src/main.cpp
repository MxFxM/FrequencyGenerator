#include <Arduino.h>

// library for the lcd
// the library uses delays but for my application that does not matter
// i might remove the delays later tho
#include <LiquidCrystal.h>

// the pins for the lcd
#define LCD_RS 12
#define LCD_EN 11
#define LCD_D4 5
#define LCD_D5 4
#define LCD_D6 3
#define LCD_D7 2

// some other values used for the display
#define LCD_UPDATE_TIME 100000

// create lcd object
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// the pins for the rotary encoder
#define ROT_DT 0
#define ROT_CK 1
#define ROT_SW 2

void printFrequency(int freq);

void setup() {
  // set up the lcd with width and height
  lcd.begin(16, 2);
}

void loop() {
  long time = micros(); // once to keep consistent timing over the loop
}

void printFrequency(int freq) {
  // print the top row
  lcd.setCursor(0, 0);
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
