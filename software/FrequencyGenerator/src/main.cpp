#include <Arduino.h>

// ------------------------------------------------------------------------------------------------
// General
// ------------------------------------------------------------------------------------------------
//#define DEBUG_MODE

// settings for the frequency counter
#define MAX_FREQUENCY_VALUE 10000000
#define MIN_FREQUENCY_VALUE 1

// settings for dutycycle
#define MAX_DUTYCYCLE_VALYE 1000
#define MIN_DUTYCYCLE_VALYE 1

// ------------------------------------------------------------------------------------------------
// LCD
// ------------------------------------------------------------------------------------------------
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

// settings
#define LCD_UPDATE_TIME 100000

// lcd object
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// functions
void setupLCD(void);
void printFrequency(int freq);
void printDutyCycle(int dc);

// ------------------------------------------------------------------------------------------------
// Rotary Encoder
// ------------------------------------------------------------------------------------------------
// the pins for the rotary encoder
#define ROT_CK 19
#define ROT_DT 18
#define ROT_SW 17


// functions
void setupRotaryEncoder(void);
void readEncoder(void);
void confirmFrequency(void);
void setFrequency(int frequency);

// variables
volatile int rot_counter = 0;
volatile int rot_previousCounter = 1;
volatile boolean rot_autoset = false;

// ------------------------------------------------------------------------------------------------
// State machine and menus
// ------------------------------------------------------------------------------------------------
enum main_states
{
  state_set_frequency,
  state_set_dutycycle,
  state_reference_output,
  state_options,
  state_menu
} main_state;

enum main_options
{
  state_autoset,
  state_wait_gps,
  state_reset_cold,
  state_reset_warm,
  state_reset_hot,
  state_exit_options
} main_option;

int main_frequency = 2;
int main_dutycycle = 501;
main_states main_menu;

// ------------------------------------------------------------------------------------------------
// MAIN
// ------------------------------------------------------------------------------------------------
void setup() {
  #ifdef DEBUG_MODE
  // debug
  Serial.begin(9600); // usb is always 12 Mbit/s
  #endif

  setupLCD();

  setupRotaryEncoder();

  // enty point for main state machine
  main_state = state_set_frequency;
  main_menu = main_state;
}

void loop() {
  long time = micros(); // once to keep consistent timing over the loop

  switch (main_state)
  {
    case state_set_frequency:
      // only update display if value changed
      if (rot_counter != rot_previousCounter) {

        // replace with acceleration
        main_frequency = main_frequency + (rot_counter - rot_previousCounter);

        if (main_frequency >= MAX_FREQUENCY_VALUE) {
          main_frequency = MAX_FREQUENCY_VALUE;
        } else if (main_frequency <= MIN_FREQUENCY_VALUE) {
          main_frequency = MIN_FREQUENCY_VALUE;
        }

        rot_previousCounter = rot_counter;
        printFrequency(main_frequency);

        if (rot_autoset) {
          //setFrequency(main_frequency);
        }
      }
      break;

    case state_set_dutycycle:
      // only update display if value changed
      if (rot_counter != rot_previousCounter) {

        // replace with acceleration
        main_dutycycle = main_dutycycle + (rot_counter - rot_previousCounter);

        if (main_dutycycle >= MAX_DUTYCYCLE_VALYE) {
          main_dutycycle = MAX_DUTYCYCLE_VALYE;
        } else if (main_dutycycle <= MIN_DUTYCYCLE_VALYE) {
          main_dutycycle = MIN_DUTYCYCLE_VALYE;
        }

        rot_previousCounter = rot_counter;
        printDutyCycle(main_dutycycle);

        if (rot_autoset) {
          //setFrequency(main_frequency);
        }
      }
      break;
    
    case state_reference_output:
      // never update display, text is only put there when entering this menu
      break;
    
    case state_options:
      // only update display if value changed
      if (rot_counter != rot_previousCounter) {
        rot_previousCounter = rot_counter;
        //printOption(rot_counter);
      }
      break;
    
    case state_menu:
      // only update display if value changed
      if (rot_counter != rot_previousCounter) {
        rot_previousCounter = rot_counter;
        //printMenu(rot_counter);
      }
      break;
    
    default:
      break;
  }

  delay(10);
}

// ------------------------------------------------------------------------------------------------
// Functions
// ------------------------------------------------------------------------------------------------
void setupLCD(void) {
  // set up the lcd with width and height
  lcd.begin(16, 2);
}

void setupRotaryEncoder(void) {
  // pin directions
  pinMode(ROT_CK, INPUT);
  pinMode(ROT_DT, INPUT);
  pinMode(ROT_SW, INPUT_PULLUP);

  // interrupts
  attachInterrupt(ROT_CK, readEncoder, CHANGE);
  attachInterrupt(ROT_DT, readEncoder, CHANGE);
  attachInterrupt(ROT_SW, confirmFrequency, FALLING);
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

void printDutyCycle(int dc) {
  // print the top row
  lcd.setCursor(0, 0);
  lcd.print("DutyCycle:");

  // set cursor on second line
  lcd.setCursor(8, 1);

  int dc_temp = dc;

  int tens = 0;
  int ones = 0;

  if (dc_temp >= 10) {
    tens = floor(dc_temp / 10);
  }

  dc_temp = dc_temp - tens * 10;

  ones = dc_temp;

  if (tens != 0) {
    // leading blank spaces
    if (tens >= 100) {
      lcd.print(" ");
    } else if (tens >= 10) {
      lcd.print("  ");
    } else {
      lcd.print("   ");
    }
    lcd.print(tens);
    // tens decimal point
    lcd.print(".");
  } else {
    // leading blank spaces, a zero and a decimal point
    lcd.print("   0.");
  }

  lcd.print(ones);
  lcd.print(" %");
}

void readEncoder()
{
  static uint8_t previousState = 3;
  static boolean tick = false;

  uint8_t newA = digitalReadFast(ROT_CK);
  uint8_t newB = digitalReadFast(ROT_DT);

  uint8_t newState = (newA << 1) | newB;

  if (newState != previousState) {
    if (newState == 3 && previousState == 2 && tick) {
      rot_counter = rot_counter + 1;
      tick = false;
    } else if (newState == 3 && previousState == 1 && tick) {
      rot_counter = rot_counter - 1;
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

void confirmFrequency() {
  static long lastConfirm = 0;
  long time = micros();

  if (time > lastConfirm + 100000) {
    setFrequency(rot_counter);
    #ifdef DEBUG_MODE
    Serial.print("Confirmed: ");
    #endif
    Serial.println(rot_counter);
    lastConfirm = time;
  }
}

void setFrequency(int frequency) {
  #ifdef DEBUG_MODE
  Serial.println("set");
  #endif
}