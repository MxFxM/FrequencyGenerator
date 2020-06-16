#include <Arduino.h>

// ------------------------------------------------------------------------------------------------
// General
// ------------------------------------------------------------------------------------------------
//#define DEBUG_MODE

// settings for push button debounce
#define BUTTON_DEBOUNCE_MICROS 10000
#define BUTTON_LONG_PRESS_MICROS 500000

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
void printMenu(void);
void printOptions(void);

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
void pressEncoder(void);
void setFrequency(int frequency);

// variables
volatile int rot_counter = 0;
volatile int rot_previousCounter = 1;
volatile long rot_pressTime = 0;
volatile boolean rot_long_press = false;
volatile boolean rot_short_press = false;

// ------------------------------------------------------------------------------------------------
// LEDs
// ------------------------------------------------------------------------------------------------
#define LED_YELLOW 16
#define LED_GREEN 15
#define LED_BLUE 14

// ------------------------------------------------------------------------------------------------
// GPS module
// ------------------------------------------------------------------------------------------------
#define GPS Serial5

// some predefined messages (wrong crc)
uint8_t message_coldstart[] = {0xB5, 0x62, 0x06, 0x04, 0x04, 0x00, 0xFF, 0xFF, 0x02, 0x00, 0x0E, 0x61};
uint8_t message_warmstart[] = {0xB5, 0x62, 0x06, 0x04, 0x04, 0x00, 0xFF, 0xFF, 0x02, 0x00, 0x0E, 0x61};
uint8_t message_hotstart[]  = {0xB5, 0x62, 0x06, 0x04, 0x04, 0x00, 0xFF, 0xFF, 0x02, 0x00, 0x0E, 0x61};

// functions
void calcCRC(uint8_t* message);
void configureTimepulse(uint32_t freq, uint32_t freqLock, double pulseRatio, double pulseRatioLock);
void setupGPS(void);
void readGPS(void);

uint8_t gps_inputBuffer[100];
uint8_t gps_inputBufferIndex = 0;

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
  state_reset_hot
} main_option;

int main_frequency = 1;
int main_dutycycle = 500;
main_states main_menu;

boolean option_autoset = false;
boolean option_waitgps = true;

boolean main_reference = false;

boolean error = false;
boolean locked = false;
boolean enabled = false;

void setFrequency(boolean reference);

// ------------------------------------------------------------------------------------------------
// MAIN
// ------------------------------------------------------------------------------------------------
void setup() {
  #ifdef DEBUG_MODE
  // debug
  Serial.begin(9600); // usb is always 12 Mbit/s

  while (Serial.available() <= 0) {}
  #endif

  setupLCD();

  setupRotaryEncoder();

  setupGPS();

  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  // enty point for main state machine
  main_state = state_menu;
  main_menu = state_set_frequency;
}

void loop() {
  long time = micros(); // once to keep consistent timing over the loop

  readGPS();

  digitalWriteFast(LED_YELLOW, error);
  digitalWriteFast(LED_GREEN, locked);
  enabled = locked || !option_waitgps;
  digitalWriteFast(LED_BLUE, enabled);

  main_states next_state = main_state;

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

        if (option_autoset) {
          setFrequency(false);
        }
      } else if (rot_long_press) {
        main_menu = state_set_frequency;
        next_state = state_menu;
        printMenu();
        rot_long_press = false;
      } else if (rot_short_press) {
        if (!option_autoset) {
          setFrequency(false);
        }
        rot_short_press = false;
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

        if (option_autoset) {
          setFrequency(false);
        }
      } else if (rot_long_press) {
        main_menu = state_set_dutycycle;
        next_state = state_menu;
        printMenu();
        rot_long_press = false;
      } else if (rot_short_press) {
        if (!option_autoset) {
          setFrequency(false);
        }
        rot_short_press = false;
      }
      break;
    
    case state_reference_output:
      // never update display, text is only put there when entering this menu
      if (rot_long_press) {
        main_menu = state_reference_output;
        next_state = state_menu;
        printMenu();
        rot_long_press = false;
      } else if (rot_short_press) {
        // does nothing here
        rot_short_press = false;
      }
      break;
    
    case state_options:
      // only update display if value changed
      if (rot_counter != rot_previousCounter) {

        boolean up = false;
        main_options next_option;
        if (rot_counter - rot_previousCounter > 0) {
          up = true;
        }

        switch (main_option)
        {
        case state_autoset:
          if (up) {next_option = state_wait_gps;}
          else    {next_option = state_autoset;}
          break;

        case state_wait_gps:
          if (up) {next_option = state_reset_cold;}
          else    {next_option = state_autoset;}
          break;
        
        case state_reset_cold:
          if (up) {next_option = state_reset_warm;}
          else    {next_option = state_wait_gps;}
          break;
        
        case state_reset_warm:
          if (up) {next_option = state_reset_hot;}
          else    {next_option = state_reset_cold;}
          break;

        case state_reset_hot:
          if (up) {next_option = state_reset_hot;}
          else    {next_option = state_reset_warm;}
          break;
        
        default:
          next_option = state_autoset;
          break;
        }

        main_option = next_option;
        rot_previousCounter = rot_counter;

        printOptions();

      } else if (rot_short_press) {
        switch (main_option)
        {
        case state_autoset:
          option_autoset = !option_autoset;
          printOptions();
          break;

        case state_wait_gps:
          option_waitgps = !option_waitgps;
          printOptions();
          setFrequency(main_reference);
          break;
        
        case state_reset_cold:
          // send reset to gps module
          calcCRC(&message_coldstart[0]);
          GPS.write(message_coldstart, sizeof(message_coldstart));
          delay(200);
          setFrequency(main_reference);
          break;
        
        case state_reset_warm:
          // send reset to gps module
          calcCRC(&message_warmstart[0]);
          GPS.write(message_warmstart, sizeof(message_warmstart));
          delay(200);
          setFrequency(main_reference);
          break;

        case state_reset_hot:
          // send reset to gps module
          calcCRC(&message_hotstart[0]);
          GPS.write(message_hotstart, sizeof(message_hotstart));
          delay(200);
          setFrequency(main_reference);
          break;
        
        default:
          break;
        }
        rot_short_press = false;
      } else if (rot_long_press) {
        main_menu = state_options;
        next_state = state_menu;
        printMenu();
        rot_long_press = false;
      }
      break;
    
    case state_menu:
      // only update display if value changed
      if (rot_counter != rot_previousCounter) {

        boolean up = false;
        main_states next_menu;
        if (rot_counter - rot_previousCounter > 0) {
          up = true;
        }

        switch (main_menu)
        {
        case state_set_frequency:
          if (up) {next_menu = state_set_dutycycle;}
          else    {next_menu = state_set_frequency;}
          break;

        case state_set_dutycycle:
          if (up) {next_menu = state_reference_output;}
          else    {next_menu = state_set_frequency;}
          break;
        
        case state_reference_output:
          if (up) {next_menu = state_options;}
          else    {next_menu = state_set_dutycycle;}
          break;
        
        case state_options:
          if (up) {next_menu = state_options;}
          else    {next_menu = state_reference_output;}
          break;
        
        default:
          next_menu = state_set_frequency;
          break;
        }

        main_menu = next_menu;
        rot_previousCounter = rot_counter;

        printMenu();

        //printMenu(rot_counter);
      } else if (rot_short_press) {
        switch (main_menu)
        {
        case state_set_frequency:
          next_state = state_set_frequency;
          printFrequency(main_frequency);
          main_reference = false;
          break;

        case state_set_dutycycle:
          next_state = state_set_dutycycle;
          printDutyCycle(main_dutycycle);
          main_reference = false;
          break;
        
        case state_reference_output:
          next_state = state_reference_output;
          lcd.setCursor(0, 0);
          lcd.print("Reference Output");
          lcd.setCursor(0, 1);
          lcd.print("10 MHz    50.0 %");
          setFrequency(true);
          main_reference = true;
          break;
        
        case state_options:
          next_state = state_options;
          printOptions();
          break;
        
        default:
          break;
        }
        rot_short_press = false;
      }
      break;
    
    default:
      break;
  }

  main_state = next_state;

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
  attachInterrupt(ROT_SW, pressEncoder, CHANGE);
}

void setupGPS(void) {
  GPS.begin(9600);
  delay(300);
  calcCRC(&message_coldstart[0]);
  GPS.write(message_coldstart, sizeof(message_coldstart));
  delay(200);
  configureTimepulse(1, 1, 0.0, 50.0);
}

void printFrequency(int freq) {
  // print the top row
  lcd.setCursor(0, 0);
  lcd.print("Frequency:      ");

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
  lcd.print("DutyCycle:      ");

  // set cursor on second line
  lcd.setCursor(0, 1);

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
      lcd.print("         ");
    } else if (tens >= 10) {
      lcd.print("          ");
    } else {
      lcd.print("           ");
    }
    lcd.print(tens);
    // tens decimal point
    lcd.print(".");
  } else {
    // leading blank spaces, a zero and a decimal point
    lcd.print("           0.");
  }

  lcd.print(ones);
  lcd.print(" %");
}

void printMenu(void) {
  lcd.setCursor(0, 0);
  lcd.print("Main Menu:      ");

  lcd.setCursor(0, 1);
  switch (main_menu)
  {
  case state_set_frequency:
    lcd.print("   set frequency");
    break;

  case state_set_dutycycle:
    lcd.print("   set dutycycle");
    break;
  
  case state_reference_output:
    lcd.print("reference output");
    break;
  
  case state_options:
    lcd.print("         options");
    break;
  
  default:
    break;
  }
}

void printOptions(void) {
  lcd.setCursor(0, 0);
  lcd.print("Options:        ");

  lcd.setCursor(0, 1);
  switch (main_option)
  {
  case state_autoset:
    lcd.print("autoset");
    if (option_autoset) {
      lcd.print("       on");
    } else {
      lcd.print("      off");
    }
    break;
    
  case state_wait_gps:
    lcd.print("wait for GPS");
    if (option_waitgps) {
      lcd.print("  on");
    } else {
      lcd.print(" off");
    }
    break;
  
  case state_reset_cold:
    lcd.print("reset GPS   cold");
    break;
  
  case state_reset_warm:
    lcd.print("reset GPS   warm");
    break;
  
  case state_reset_hot:
    lcd.print("reset GPS    hot");
    break;
  
  default:
    break;
  }
}

void readEncoder(void)
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

void pressEncoder(void) {
  static long lastChange = 0;

  long time = micros();

  if (time > lastChange + BUTTON_DEBOUNCE_MICROS) {
    if (!digitalReadFast(ROT_SW)) { // pressed
      rot_pressTime = time;
      lastChange = time;
    } else { // released
      if (time > rot_pressTime + BUTTON_LONG_PRESS_MICROS) {
        rot_long_press = true;
        rot_short_press = false;
        lastChange = time;
      } else {
        rot_long_press = false;
        rot_short_press = true;
        lastChange = time;
      }
    }
  }
}

void calcCRC(uint8_t* message) {
  uint8_t ck_a = 0;
  uint8_t ck_b = 0;

  uint16_t length = 8;

  // advance 2 adresses to skip header
  message++;
  message++;

  for (uint16_t i = 2; i < length - 2; i++) {
    if (i == 4) {
      length = length + *message;
    }
    if (i == 5) {
      length = length + (*message << 8);
    }
    // calculate the checksum
    ck_a = ck_a + *message;
    ck_b = ck_b + ck_a;

    message++; // advance pointer to next byte
  }

  // add crc to message
  *message = ck_a;
  message++;
  *message = ck_b;
}

void configureTimepulse(uint32_t freq, uint32_t freqLock, double pulseRatio, double pulseRatioLock) {
  double factor = powl(2.0, -32.0);
  pulseRatio = 1 - pulseRatio;
  uint32_t dc = (pulseRatio / factor);
  pulseRatioLock = 1 - pulseRatioLock;
  uint32_t dcLock = (pulseRatioLock / factor);

  uint8_t message[40];
  message[0] =  0xB5; // header
  message[1] =  0x62; // header
  message[2] =  0x06; // class
  message[3] =  0x31; // id
  message[4] =  0x20; // length
  message[5] =  0x00; // length
  message[6] =  0x00; // time pulse selection
  message[7] =  0x01; // version
  message[8] =  0x00; // reserved
  message[9] =  0x00; // reserved
  message[10] = 0x32; // antenna cable delay (here fixed)
  message[11] = 0x00; // antenna cable delay
  message[12] = 0x00; // rf group delay (here fixed)
  message[13] = 0x00; // rf group delay
  message[14] = (freq >>  0) & 0xFF; // frequency
  message[15] = (freq >>  8) & 0xFF; // frequency
  message[16] = (freq >> 16) & 0xFF; // frequency
  message[17] = (freq >> 24) & 0xFF; // frequency
  message[18] = (freqLock >>  0) & 0xFF; // frequency on lock
  message[19] = (freqLock >>  8) & 0xFF; // frequency on lock
  message[20] = (freqLock >> 16) & 0xFF; // frequency on lock
  message[21] = (freqLock >> 24) & 0xFF; // frequency on lock
  message[22] = (dc >>  0) & 0xFF; // dutycycle
  message[23] = (dc >>  8) & 0xFF; // dutycycle
  message[24] = (dc >> 16) & 0xFF; // dutycycle
  message[25] = (dc >> 24) & 0xFF; // dutycycle
  message[26] = (dcLock >>  0) & 0xFF; // dutycycle on lock
  message[27] = (dcLock >>  8) & 0xFF; // dutycycle on lock
  message[28] = (dcLock >> 16) & 0xFF; // dutycycle on lock
  message[29] = (dcLock >> 24) & 0xFF; // dutycycle on lock
  message[30] = 0x00; // user configured delay
  message[31] = 0x00; // user configured delay
  message[32] = 0x00; // user configured delay
  message[33] = 0x00; // user configured delay
  message[34] = 0xEF; // flags
  message[35] = 0x00; // flags
  message[36] = 0x00; // flags
  message[37] = 0x00; // flags
  message[38] = 0x00; // crc will be included after calculation
  message[39] = 0x00; // crc will be included after calculation

  calcCRC(&message[0]);
  GPS.write(message, sizeof(message));
}

void setFrequency(boolean reference) {
  int fn;
  int fl;
  double dn;
  double dl;

  if (reference) {
    fn = 10000000;
    dn = 50.0;
  } else {
    fn = main_frequency;
    dn = double(main_dutycycle/1000.0);
  }

  fl = fn;
  dl = dn;

  if (option_waitgps) {
    fn = 1;
    dn = 0.0;
  }

  configureTimepulse(fn, fl, dn, dl);
}

void readGPS(void) {
  uint8_t newBytes = GPS.available();

  for (uint8_t newByte = 0; newByte < newBytes; newByte++) {
    gps_inputBuffer[gps_inputBufferIndex] = GPS.read();
    #ifdef DEBUG_MODE
    Serial.write(gps_inputBuffer[gps_inputBufferIndex]);
    #endif
    gps_inputBufferIndex++;

    if (gps_inputBuffer[0] == 0x24) {
      // correct start byte
      if (gps_inputBuffer[gps_inputBufferIndex-1] == 0x0A && gps_inputBuffer[gps_inputBufferIndex-2] == 0x0D) {
        // carriage return and linefeed = complete message
        if (gps_inputBuffer[3] == 0x52 && gps_inputBuffer[4] == 0x4D && gps_inputBuffer[5] == 0x43) {
          // the correct RMC message
          uint8_t validityByteIndex = 0;
          uint8_t commaCounter = 0;
          for (uint8_t index = 6; index < gps_inputBufferIndex; index++) {
            if (gps_inputBuffer[index] == 0x2C) {
              commaCounter++;
              if (commaCounter >= 2) {
                validityByteIndex = index + 1;
                break;
              }
            }
          }
          if (gps_inputBuffer[validityByteIndex] == 0x41) {
            locked = true;
          } else {
            locked = false;
          }
        }
        gps_inputBufferIndex = 0;
      }
    } else {
      // wrong start byte, clear buffer
      gps_inputBufferIndex = 0;
    }
  }
}