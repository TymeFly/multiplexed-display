/**
 * This sketch is specifically for programming the EEPROM used in the 8-bit
 * decimal display decoder described in https://youtu.be/dLh1n2dErzE
 */

// EEPROM Definitions 
#define EEPROM_SIZE (8 * 1024)
#define MAX_ADDRESS (EEPROM_SIZE - 1)

// Arduino Pins 
#define SHIFT_DATA  2
#define SHIFT_CLK   3
#define SHIFT_LATCH 4
#define EEPROM_D0   5
#define EEPROM_D7   12
#define WRITE_EN    13

// Values of each data pin 
#define D0  0x01
#define D1  0x02
#define D2  0x04
#define D3  0x08
#define D4  0x10
#define D5  0x20
#define D6  0x40
#define D7  0x80

// Mappings between the LEDs and data pins.
// Change these if you rewire the 7+1 segment display
#define LED_LWR_RIGHT  D0
#define LED_LWR        D1
#define LED_LWR_LEFT   D2
#define LED_DOT        D3
#define LED_MIDDLE     D4
#define LED_TOP_LEFT   D5
#define LED_TOP        D6
#define LED_TOP_RIGHT  D7

// Modes we will support. 
// Change these if you want to map the modes differently
// I want the machine to power up and display 'on' (Text mode, value 0)
#define MODE_TEXT       0
#define MODE_UNSIGNED   1
#define MODE_SIGNED     2
#define MODE_HEX        3
#define MODE_RESERVED1  4
#define MODE_RESERVED2  5
#define MODE_DEBUG      6
#define MODE_OFF        7

// Offset in the EEPROM for the first byte of each mode
#define MODE_TEXT_OFFSET      (MODE_TEXT << 10)
#define MODE_UNSIGNED_OFFSET  (MODE_UNSIGNED << 10)
#define MODE_SIGNED_OFFSET    (MODE_SIGNED << 10)
#define MODE_HEX_OFFSET       (MODE_HEX << 10)
#define MODE_RESERVED1_OFFSET (MODE_RESERVED1 << 10)
#define MODE_RESERVED2_OFFSET (MODE_RESERVED2 << 10)
#define MODE_DEBUG_OFFSET     (MODE_DEBUG << 10)
#define MODE_OFF_OFFSET       (MODE_OFF << 10)

// Offsets within each Mode's data for each digit
#define DIGIT1_OFFSET   0x000
#define DIGIT2_OFFSET   0x100
#define DIGIT3_OFFSET   0x200
#define DIGIT4_OFFSET   0x300
#define BLOCK_SIZE      0x400


// LED combinations for each of the characters
#define CHAR_0        (LED_LWR_RIGHT | LED_LWR        | LED_LWR_LEFT  | LED_TOP_LEFT  | LED_TOP       | LED_TOP_RIGHT)
#define CHAR_1        (LED_LWR_RIGHT | LED_TOP_RIGHT)
#define CHAR_2        (LED_TOP       | LED_TOP_RIGHT  | LED_MIDDLE    | LED_LWR_LEFT  | LED_LWR)
#define CHAR_3        (LED_TOP       | LED_TOP_RIGHT  | LED_MIDDLE    | LED_LWR_RIGHT | LED_LWR)
#define CHAR_4        (LED_TOP_LEFT  | LED_TOP_RIGHT  | LED_MIDDLE    | LED_LWR_RIGHT)
#define CHAR_5        (LED_TOP       | LED_TOP_LEFT   | LED_MIDDLE    | LED_LWR_RIGHT | LED_LWR)
#define CHAR_6        (LED_TOP       | LED_TOP_LEFT   | LED_MIDDLE    | LED_LWR_LEFT  | LED_LWR_RIGHT | LED_LWR)
#define CHAR_7        (LED_TOP       | LED_TOP_RIGHT  | LED_LWR_RIGHT)
#define CHAR_8        (LED_LWR_RIGHT | LED_LWR        | LED_LWR_LEFT  | LED_MIDDLE    | LED_TOP_LEFT  | LED_TOP | LED_TOP_RIGHT)
#define CHAR_9        (LED_LWR_RIGHT | LED_MIDDLE     | LED_TOP_LEFT  | LED_TOP       | LED_TOP_RIGHT)
#define CHAR_A        (LED_LWR_LEFT  | LED_TOP_LEFT   | LED_MIDDLE    | LED_TOP       | LED_TOP_RIGHT | LED_LWR_RIGHT)
#define CHAR_B        (LED_LWR_LEFT  | LED_TOP_LEFT   | LED_MIDDLE    | LED_LWR       | LED_LWR_RIGHT)
#define CHAR_C        (LED_TOP       | LED_TOP_LEFT   | LED_LWR_LEFT  | LED_LWR)
#define CHAR_D        (LED_TOP_RIGHT | LED_LWR_RIGHT  | LED_LWR       | LED_MIDDLE    | LED_LWR_LEFT)
#define CHAR_E        (LED_TOP       | LED_TOP_LEFT   | LED_MIDDLE    | LED_LWR_LEFT  | LED_LWR)
#define CHAR_F        (LED_TOP       | LED_TOP_LEFT   | LED_MIDDLE    | LED_LWR_LEFT)
#define CHAR_G        (LED_LWR_RIGHT | LED_LWR        | LED_MIDDLE    | LED_TOP_LEFT  | LED_TOP       | LED_TOP_RIGHT)
#define CHAR_H        (LED_TOP_LEFT  | LED_LWR_LEFT   | LED_MIDDLE    | LED_LWR_RIGHT)
#define CHAR_I        (LED_LWR_RIGHT)
#define CHAR_L        (LED_TOP_LEFT  | LED_LWR_LEFT   | LED_LWR)
#define CHAR_N        (LED_LWR_LEFT  | LED_MIDDLE     | LED_LWR_RIGHT)
#define CHAR_O        (LED_LWR_RIGHT | LED_LWR        | LED_LWR_LEFT  | LED_MIDDLE)
#define CHAR_P        (LED_LWR_LEFT  | LED_TOP_LEFT   | LED_TOP       | LED_TOP_RIGHT | LED_MIDDLE)
#define CHAR_R        (LED_LWR_LEFT  | LED_MIDDLE)
#define CHAR_S        (LED_TOP       | LED_TOP_LEFT   | LED_MIDDLE    | LED_LWR_RIGHT | LED_LWR)
#define CHAR_T        (LED_TOP_LEFT  | LED_LWR_LEFT   | LED_MIDDLE    | LED_LWR)
#define CHAR_U        (LED_LWR_RIGHT | LED_LWR        | LED_LWR_LEFT)
#define CHAR_Y        (LED_TOP_LEFT  | LED_TOP_RIGHT  | LED_MIDDLE    | LED_LWR_RIGHT | LED_LWR)
#define CHAR_BANG     (LED_TOP_RIGHT | LED_LWR_RIGHT  | LED_DOT)
#define CHAR_MINUS    (LED_MIDDLE)
#define CHAR_BLANK    (0)
#define CHAR_DOT      (LED_DOT)
#define CHAR_QUESTION (LED_TOP       | LED_TOP_RIGHT  | LED_MIDDLE    | LED_LWR_LEFT  | LED_DOT)
#define CHAR_ALL      (LED_LWR_RIGHT | LED_LWR        | LED_LWR_LEFT  | LED_DOT       | LED_MIDDLE  | LED_TOP_LEFT | LED_TOP | LED_TOP_RIGHT)



// Bit patterns for hex digits -  0..9 and letters A..F
byte digits[] = { CHAR_0, CHAR_1, CHAR_2, CHAR_3, CHAR_4, CHAR_5, CHAR_6, CHAR_7, CHAR_8, CHAR_9, CHAR_A, CHAR_B, CHAR_C, CHAR_D, CHAR_E, CHAR_F };

// Predefined string for text mode
byte text[][4] = {
  { CHAR_BLANK,     CHAR_O,         CHAR_N,         CHAR_BLANK    },      //  0: on
  { CHAR_BLANK,     CHAR_O,         CHAR_F,         CHAR_F        },      //  1: off
  { CHAR_BLANK,     CHAR_G,         CHAR_O,         CHAR_BLANK    },      //  2: go
  { CHAR_BLANK,     CHAR_G,         CHAR_O,         CHAR_BANG     },      //  3: go!
  { CHAR_H,         CHAR_A,         CHAR_L,         CHAR_T        },      //  4: halt
  { CHAR_S,         CHAR_T,         CHAR_O,         CHAR_P        },      //  5: stop
  { CHAR_BLANK,     CHAR_E,         CHAR_N,         CHAR_D        },      //  6: end
  { CHAR_BLANK,     CHAR_I,         CHAR_N,         CHAR_C        },      //  7: inc
  { CHAR_BLANK,     CHAR_D,         CHAR_E,         CHAR_C        },      //  8: dec
  { CHAR_BLANK,     CHAR_Y,         CHAR_E,         CHAR_S        },      //  9: yes
  { CHAR_Y,         CHAR_E,         CHAR_S,         CHAR_BANG     },      // 10: yes!
  { CHAR_BLANK,     CHAR_N,         CHAR_O,         CHAR_BLANK,   },      // 11: no
  { CHAR_BLANK,     CHAR_N,         CHAR_O,         CHAR_BANG,   },       // 12: no!
  { CHAR_E,         CHAR_R,         CHAR_R,         CHAR_0        },      // 13: err0
  { CHAR_E,         CHAR_R,         CHAR_R,         CHAR_1        },      // 14: err1
  { CHAR_E,         CHAR_R,         CHAR_R,         CHAR_2        },      // 15: err2
  { CHAR_E,         CHAR_R,         CHAR_R,         CHAR_3        },      // 16: err3
  { CHAR_E,         CHAR_R,         CHAR_R,         CHAR_4        },      // 17: err4
  { CHAR_E,         CHAR_R,         CHAR_R,         CHAR_5        },      // 18: err5
  { CHAR_E,         CHAR_R,         CHAR_R,         CHAR_6        },      // 19: err6
  { CHAR_E,         CHAR_R,         CHAR_R,         CHAR_7        },      // 20: err7
  { CHAR_E,         CHAR_R,         CHAR_R,         CHAR_8        },      // 21: err8
  { CHAR_E,         CHAR_R,         CHAR_R,         CHAR_9        },      // 22: err9
  { CHAR_BLANK,     CHAR_BLANK,     CHAR_BLANK,     CHAR_MINUS    },      // 23: animation 1.1
  { CHAR_BLANK,     CHAR_BLANK,     CHAR_MINUS,     CHAR_BLANK    },      // 24: animation 1.2
  { CHAR_BLANK,     CHAR_MINUS,     CHAR_BLANK,     CHAR_MINUS    },      // 25: animation 1.3
  { CHAR_MINUS,     CHAR_BLANK,     CHAR_BLANK,     CHAR_MINUS    },      // 26: animation 1.4
  { CHAR_QUESTION,  CHAR_BLANK,     CHAR_BLANK,     CHAR_QUESTION },      // 27: animation 2.1
  { CHAR_BLANK,     CHAR_QUESTION,  CHAR_QUESTION,  CHAR_BLANK    },      // 28: animation 2.2
  { CHAR_P,         CHAR_L,         CHAR_A,         CHAR_Y        },      // 29: play
  { CHAR_L,         CHAR_O,         CHAR_O,         CHAR_P        },      // 30: loop
  { CHAR_R,         CHAR_E,         CHAR_D,         CHAR_Y        },      // 31: redy
  { CHAR_R,         CHAR_D,         CHAR_Y,         CHAR_QUESTION },      // 32: rdy?
  { CHAR_BLANK,     CHAR_N,         CHAR_O,         CHAR_P        },      // 33: nop
  { CHAR_QUESTION,  CHAR_QUESTION,  CHAR_QUESTION,  CHAR_QUESTION },      // 34: ????
  { CHAR_BLANK,     CHAR_BLANK,     CHAR_BLANK,     CHAR_BLANK    },      // 35: blank
  { CHAR_N,         CHAR_U,         CHAR_L,         CHAR_L        },      // 36: null
  { CHAR_BLANK,     CHAR_G,         CHAR_E,         CHAR_T        },      // 37: get
  { CHAR_BLANK,     CHAR_S,         CHAR_E,         CHAR_T        },      // 38: set
  { CHAR_BLANK,     CHAR_P,         CHAR_U,         CHAR_T        },      // 39: put
  { CHAR_BLANK,     CHAR_A,         CHAR_D,         CHAR_D        },      // 40: add
  { CHAR_BLANK,     CHAR_I,         CHAR_N,         CHAR_BLANK    },      // 41: in
  { CHAR_BLANK,     CHAR_O,         CHAR_U,         CHAR_T        },      // 42: out
  { CHAR_BLANK,     CHAR_C,         CHAR_A,         CHAR_T        },      // 43: cat
  { CHAR_BLANK,     CHAR_D,         CHAR_O,         CHAR_G        },      // 44: dog
  { CHAR_BLANK,     CHAR_F,         CHAR_L,         CHAR_Y        },      // 45: fly
  { CHAR_BLANK,     CHAR_R,         CHAR_A,         CHAR_T        },      // 46: rat
  { CHAR_BLANK,     CHAR_I,         CHAR_N,         CHAR_T        },      // 47: int
  { CHAR_C,         CHAR_H,         CHAR_A,         CHAR_R        },      // 48: char
  { CHAR_R,         CHAR_A,         CHAR_T,         CHAR_E        },      // 49: rate
  { CHAR_BLANK,     CHAR_T,         CHAR_A,         CHAR_P        },      // 50: tap
  { CHAR_T,         CHAR_A,         CHAR_P,         CHAR_E        },      // 51: tape
  { CHAR_BLANK,     CHAR_H,         CHAR_I,         CHAR_BLANK    },      // 52: hi
  { CHAR_BLANK,     CHAR_H,         CHAR_I,         CHAR_BANG     },      // 53: hi!
  { CHAR_BLANK,     CHAR_T,         CHAR_O,         CHAR_P        },      // 54: top
  { CHAR_BLANK,     CHAR_P,         CHAR_T,         CHAR_R        },      // 55: ptr
  { CHAR_P,         CHAR_T,         CHAR_R,         CHAR_QUESTION },      // 56: ptr?
};

// Predefined string for debug mode
byte debug[][4] = {
  { CHAR_ALL,         CHAR_ALL,         CHAR_ALL,         CHAR_ALL          },  //  0: All on
  { CHAR_ALL,         CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  //  1: Char4 on  
  { CHAR_BLANK,       CHAR_ALL,         CHAR_BLANK,       CHAR_BLANK        },  //  2: Char3 on  
  { CHAR_BLANK,       CHAR_BLANK,       CHAR_ALL,         CHAR_BLANK        },  //  3: Char2 on   
  { CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK,       CHAR_ALL          },  //  4: Char1 on  
  { LED_LWR_RIGHT,    LED_LWR_RIGHT,    LED_LWR_RIGHT,    LED_LWR_RIGHT     },  //  5: LED1 on
  { LED_LWR,          LED_LWR,          LED_LWR,          LED_LWR           },  //  6: LED2 on
  { LED_LWR_LEFT,     LED_LWR_LEFT,     LED_LWR_LEFT,     LED_LWR_LEFT      },  //  7: LED3 on
  { LED_DOT,          LED_DOT,          LED_DOT,          LED_DOT           },  //  8: LED4 on
  { LED_MIDDLE,       LED_MIDDLE,       LED_MIDDLE,       LED_MIDDLE        },  //  9: LED5 on
  { LED_TOP_LEFT,     LED_TOP_LEFT,     LED_TOP_LEFT,     LED_TOP_LEFT      },  // 10: LED6 on
  { LED_TOP,          LED_TOP,          LED_TOP,          LED_TOP           },  // 11: LED7 on
  { LED_TOP_RIGHT,    LED_TOP_RIGHT,    LED_TOP_RIGHT,    LED_TOP_RIGHT     },  // 12: LED8 on
  { CHAR_0,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 13: 0
  { CHAR_1,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 14: 1
  { CHAR_2,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 15: 2
  { CHAR_3,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 16: 3
  { CHAR_4,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 17: 4 
  { CHAR_5,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 18: 5
  { CHAR_6,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 19: 6
  { CHAR_7,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 20: 7
  { CHAR_8,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 21: 8
  { CHAR_9,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 22: 9
  { CHAR_A,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 23: A
  { CHAR_B,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 24: B
  { CHAR_C,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 25: C
  { CHAR_D,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 26: D
  { CHAR_E,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 27: E
  { CHAR_F,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 28: F
  { CHAR_G,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 29: G
  { CHAR_H,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 30: H
  { CHAR_I,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 31: I
  { CHAR_L,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 32: L
  { CHAR_N,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 33: N
  { CHAR_O,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 34: O
  { CHAR_P,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 35: P
  { CHAR_R,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 36: R
  { CHAR_S,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 37: S
  { CHAR_T,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 38: T
  { CHAR_U,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 39: U
  { CHAR_Y,           CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 40: Y
  { CHAR_BANG,        CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 41: !
  { CHAR_MINUS,       CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 42: -
  { CHAR_DOT,         CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 43: .
  { CHAR_QUESTION,    CHAR_BLANK,       CHAR_BLANK,       CHAR_BLANK        },  // 44: ?
};

/*
   Output the address bits and outputEnable signal using shift registers.
*/
void setAddress(int address, bool outputEnable) {
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, (address >> 8) | (outputEnable ? 0x00 : 0x80));
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address);

  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);
}


/*
   Read a byte from the EEPROM at the specified address.
*/
byte readEEPROM(int address) {
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, INPUT);
  }
  setAddress(address, /*outputEnable*/ true);

  byte data = 0;
  for (int pin = EEPROM_D7; pin >= EEPROM_D0; pin -= 1) {
    data = (data << 1) + digitalRead(pin);
  }
  return data;
}


/*
   Write a byte to the EEPROM at the specified address.
*/
void writeEEPROM(int address, byte data) {
  setAddress(address, /*outputEnable*/ false);
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, OUTPUT);
  }

  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    digitalWrite(pin, data & 1);
    data = data >> 1;
  }
  digitalWrite(WRITE_EN, LOW);
  delayMicroseconds(1);
  digitalWrite(WRITE_EN, HIGH);
  delay(10);
}


/*
   Read the contents of the EEPROM and print them to the serial monitor.
*/
void printContents() {
  for (int base = 0; base <= MAX_ADDRESS; base += 16) {
    byte data[16];
    for (int offset = 0; offset <= 15; offset += 1) {
      data[offset] = readEEPROM(base + offset);
    }

    char buf[80];
    sprintf(buf, "%04x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}


/*
   Programmer entry point
*/
void setup() {
  // Setup the output Arduio output pins
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  digitalWrite(WRITE_EN, HIGH);
  pinMode(WRITE_EN, OUTPUT);
  Serial.begin(57600);

  // Write the value for each of the 8 modes 
  writeText();
  writeUnsignedMode();
  writeSignedMode();
  writeHexMode();
  writeReservedMode(1, MODE_RESERVED1_OFFSET);
  writeReservedMode(2, MODE_RESERVED2_OFFSET);
  writeDebug();
  writeOffMode();

  // Read and print out the contents of the EERPROM
  Serial.println("Reading EEPROM");
  printContents();
}


/*
   Write the predefined strings for "Text Mode"
*/
void writeText() {
  writeStrings("Text", text, sizeof(text) / 4, MODE_TEXT_OFFSET);   
}


/*
   Write the predefined strings for "Debug Mode"
*/
void writeDebug() {
  writeStrings("Debug", debug, sizeof(debug) / 4, MODE_DEBUG_OFFSET);  
}


/*
   Helper method to write all the strings in a table to the EEPROM. 
   Used by "Text" and "Debug" modes.
*/
static void writeStrings(char* title, byte strings[][4], int stringCount, int baseAddress) {
  char message[30];
  
  sprintf(message, "%s Mode: %02d strings\n", title, stringCount);
  Serial.print(message);

  sprintf(message, "  Char 1 at 0x%04x\n", (baseAddress | DIGIT1_OFFSET));
  Serial.print(message);
  sprintf(message, "  Char 2 at 0x%04x\n", (baseAddress | DIGIT2_OFFSET));
  Serial.print(message);
  sprintf(message, "  Char 3 at 0x%04x\n", (baseAddress | DIGIT3_OFFSET));
  Serial.print(message);
  sprintf(message, "  Char 4 at 0x%04x\n", (baseAddress | DIGIT4_OFFSET));
  Serial.print(message);

  for (int value = 0; value < stringCount; value += 1) {
    writeEEPROM(baseAddress | DIGIT1_OFFSET | value, strings[value][3]);
    writeEEPROM(baseAddress | DIGIT2_OFFSET | value, strings[value][2]);
    writeEEPROM(baseAddress | DIGIT3_OFFSET | value, strings[value][1]);
    writeEEPROM(baseAddress | DIGIT4_OFFSET | value, strings[value][0]);
  }

  Serial.println("  Clearing empty text space");

  for (int value = stringCount; value <= 255; value += 1) {
    writeEEPROM(baseAddress | DIGIT1_OFFSET | value, CHAR_QUESTION);
    writeEEPROM(baseAddress | DIGIT2_OFFSET | value, CHAR_QUESTION);
    writeEEPROM(baseAddress | DIGIT3_OFFSET | value, CHAR_QUESTION);
    writeEEPROM(baseAddress | DIGIT4_OFFSET | value, CHAR_QUESTION);
  }  
}


/*
   Write the predefined characters for "Off" Mode - no values are displayed for any value
   We will display all dots to assure the user the display is working
*/
void writeOffMode() {
  int offset;
  char message[30];
    
  Serial.println("Off Mode");

  offset = MODE_OFF_OFFSET | DIGIT1_OFFSET;
  sprintf(message, "  Digit 1 at 0x%04x\n", offset);
  Serial.print(message);
  
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(offset + value, CHAR_DOT);
  }
  
  offset = MODE_OFF_OFFSET | DIGIT2_OFFSET;
  sprintf(message, "  Digit 2 at 0x%04x\n", offset);
  Serial.print(message);
  
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(offset + value, CHAR_DOT);
  }
  
  offset = MODE_OFF_OFFSET | DIGIT3_OFFSET;
  sprintf(message, "  Digit 3 at 0x%04x\n", offset);
  Serial.print(message);
  
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(offset + value, CHAR_DOT);
  }

  offset = MODE_OFF_OFFSET | DIGIT4_OFFSET;
  sprintf(message, "  Digit 4 at 0x%04x\n", offset);
  Serial.print(message);
  
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(offset + value, CHAR_DOT);
  }
}


/*
   Write the characters for "unsigned" mode
*/
void writeUnsignedMode() {
  int offset;
  char message[30];
    
  Serial.println("Unsigned Mode");

  offset = MODE_UNSIGNED_OFFSET | DIGIT1_OFFSET;
  sprintf(message, "  Digit 1 at 0x%04x\n", offset);
  Serial.print(message);
  
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(offset + value, digits[value % 10]);
  }
  
  offset = MODE_UNSIGNED_OFFSET | DIGIT2_OFFSET;
  sprintf(message, "  Digit 2 at 0x%04x\n", offset);
  Serial.print(message);
  
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(offset + value, digits[(value / 10) % 10]);
  }
  
  offset = MODE_UNSIGNED_OFFSET | DIGIT3_OFFSET;
  sprintf(message, "  Digit 3 at 0x%04x\n", offset);
  Serial.print(message);
  
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(offset + value, digits[(value / 100) % 10]);
  }

  offset = MODE_UNSIGNED_OFFSET | DIGIT4_OFFSET;
  sprintf(message, "  Digit 4 at 0x%04x\n", offset);
  Serial.print(message);
  
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(offset + value, CHAR_BLANK);
  }
}


/*
   Write the characters for "signed" mode
*/
void writeSignedMode() {
  int offset;
  char message[30];
    
  Serial.println("Signed Mode");

  offset = MODE_SIGNED_OFFSET | DIGIT1_OFFSET;  
  sprintf(message, "  Digit 1 at 0x%04x\n", offset);
  Serial.print(message);
  
  for (int value = -128; value <= 127; value += 1) {
    writeEEPROM((offset + (byte)value), digits[abs(value) % 10]);
  }

  offset = MODE_SIGNED_OFFSET | DIGIT2_OFFSET;
  sprintf(message, "  Digit 2 at 0x%04x\n", offset);
  Serial.print(message);
  
  for (int value = -128; value <= 127; value += 1) {
    writeEEPROM((offset + (byte)value), digits[abs(value / 10) % 10]);
  }
  
  offset = MODE_SIGNED_OFFSET | DIGIT3_OFFSET;
  sprintf(message, "  Digit 3 at 0x%04x\n", offset);
  Serial.print(message);
  
  for (int value = -128; value <= 127; value += 1) {
    writeEEPROM((offset + (byte)value), digits[abs(value / 100) % 10]);
  }
  
  offset = MODE_SIGNED_OFFSET | DIGIT4_OFFSET;
  sprintf(message, "  Digit 4 at 0x%04x\n", offset);
  Serial.print(message);
  
  for (int value = -128; value <= 127; value += 1) {
    writeEEPROM((offset + (byte)value), (value < 0 ? CHAR_MINUS : CHAR_BLANK));
  }
}


/*
   Write the characters for "Hex" mode
   We only need 2 digits, so we'll use the middle two
*/
void writeHexMode() {
  int offset;
  char message[30];
    
  Serial.println("Hex Mode");

  offset = MODE_HEX_OFFSET | DIGIT1_OFFSET;
  sprintf(message, "  Digit 1 at 0x%04x\n", offset);
  Serial.print(message);
  
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(offset + value, CHAR_BLANK);
  }
  
  offset = MODE_HEX_OFFSET | DIGIT2_OFFSET;
  sprintf(message, "  Digit 2 at 0x%04x\n", offset);
  Serial.print(message);
  
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(offset + value, digits[value % 16]);
  }
  
  offset = MODE_HEX_OFFSET | DIGIT3_OFFSET;
  sprintf(message, "  Digit 3 at 0x%04x\n", offset);
  Serial.print(message);
  
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(offset + value, digits[(value / 16) % 16]);
  }

  offset = MODE_HEX_OFFSET | DIGIT4_OFFSET;
  sprintf(message, "  Digit 4 at 0x%04x\n", offset);
  Serial.print(message);
  
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(offset + value, CHAR_BLANK);
  }
}


/*
   Write the characters for a "reserved" mode.
   The computer shouldn't use this mode so we'll just display question marks
*/
void writeReservedMode(int block, int address) {
  char message[30];
    
  sprintf(message, "Reserved Mode %d\n", block);
  Serial.print(message);

  sprintf(message, "  Starts at 0x%04x\n", address);
  Serial.print(message);
  
  for (int offset = 0; offset <= BLOCK_SIZE; offset+= 2) {
    writeEEPROM(address + offset, CHAR_DOT);
    writeEEPROM(address + offset + 1, CHAR_QUESTION);
  }
}


// Required entry point
void loop() {
  // Do nothing
}
