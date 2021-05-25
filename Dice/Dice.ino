#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL),
// On a D1 Mini:             D2(SDA), D1(SCL)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// constants won't change. They're used here to set pin numbers:
#define ROLL_BUTTON_PIN  0 //D16    // the number of the pushbutton pin
#define DIE_PIN         15 //D8     // the pin used to change the dice 
#define TILT_INPUT_PIN   2 //D4     // connected to a tilt sensor to detect when the dice is rocked or tossed
#define led1      12 //D6     // not used yet, init to ON
#define led2      14 //D5     // not used yet, init to ON

#define LOGO_HEIGHT 43 //64
#define LOGO_WIDTH 128 //82
static const unsigned char PROGMEM logo_bmp[] =
{ //output from LCDAssistant, byte orientation: Horizontal
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0xFF,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFF,0xFF,0xFF,0xF8,0xFF,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xF8,0xFF,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xFF,0x00,0x00,0x07,0xF8,0xFF,0xFF,0x9F,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x40,0x01,0xFF,0x00,0x00,0x07,0xF8,0xFF,0xFF,0x9F,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x38,0x39,0xFF,0xFF,0xFF,0xFF,0xF8,0xFF,0xFF,0x9F,0xFF,0xFF,0xFF,0xFF,0xF1,0xFC,0x03,0x39,0xFF,0xFF,0xFF,0xFF,0xF8,0xFF,0xFF,0x9F,0xFF,0xFF,0xFF,0xFF,0xF1,0xFE,0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xF8,0xFF,0xFF,0x9F,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xF8,0xFF,0xFF,0x9F,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xF8,0xFF,0xFF,0x9F,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xF8,0xFF,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFF,0xFF,0xFF,0xF8,0xFF,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xF8,0xFF,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x1C,0x03,0x0F,0xFF,0x00,0xFC,0x1F,0xFF,0xE0,0x1C,0x00,0xFE,0x00,0xC0,0x06,0x60,0x1C,0x03,0x0F,0xFF,0x03,0xCF,0x1F,0xFF,0xE0,0x1C,0x00,0xFF,0xE0,0x60,0x0E,0x60,0x1C,0x03,0x0C,0x00,0x06,0x01,0x00,0x30,0x00,0x3C,0x00,0xC0,0x70,0x30,0x0C,0x60,0x34,0x06,0x0C,0x00,0x06,0x00,0x00,0x30,0x00,0x36,0x00,0xC0,0x30,0x30,0x18,0x30,0x36,0x06,0x0C,0x00,0x0C,0x00,0x00,0x30,0x00,0x26,0x00,0xC0,0x18,0x18,0x30,0x30,0x26,0x06,0x0C,0x00,0x0C,0x00,0x00,0x30,0x00,0x63,0x00,0xC0,0x18,0x0C,0x30,0x30,0x22,0x06,0x0C,0x00,0x0E,0x00,0x00,0x30,0x00,0x63,0x00,0xC0,0x30,0x0C,0x60,0x30,0x62,0x0C,0x0C,0x00,0x07,0x00,0x00,0x30,0x00,0xC1,0x00,0xC0,0x30,0x06,0xC0,0x18,0x63,0x0C,0x0F,0xFF,0x03,0xE0,0x00,0x30,0x00,0xC1,0x80,0xFF,0xC0,0x03,0xC0,0x18,0x43,0x0C,0x0F,0xFF,0x01,0xFE,0x00,0x30,0x00,0x81,0x80,0xFF,0xC0,0x03,0x80,0x18,0x41,0x0C,0x0C,0x00,0x00,0x0F,0x00,0x30,0x01,0x80,0x80,0xC0,0x30,0x01,0x80,0x18,0xC1,0x18,0x0C,0x00,0x00,0x03,0x80,0x30,0x01,0x80,0xC0,0xC0,0x18,0x01,0x80,0x0C,0xC1,0x98,0x0C,0x00,0x00,0x01,0x80,0x30,0x03,0x00,0xC0,0xC0,0x18,0x01,0x80,0x0C,0x81,0x98,0x0C,0x00,0x00,0x01,0x80,0x30,0x03,0xFF,0xE0,0xC0,0x18,0x01,0x80,0x0C,0x80,0x98,0x0C,0x00,0x00,0x01,0x80,0x30,0x03,0x00,0x60,0xC0,0x18,0x01,0x80,0x0D,0x80,0x90,0x0C,0x00,0x00,0x01,0x80,0x30,0x06,0x00,0x20,0xC0,0x18,0x01,0x80,0x07,0x80,0xF0,0x0C,0x00,0x08,0x03,0x80,0x30,0x06,0x00,0x30,0xC0,0x38,0x01,0x80,0x07,0x00,0xF0,0x0F,0xFF,0x0F,0x07,0x00,0x30,0x0C,0x00,0x30,0xFF,0xF0,0x01,0x80,0x07,0x00,0x70,0x0F,0xFF,0x03,0xFC,0x00,0x30,0x0C,0x00,0x18,0xFF,0xC0,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

//variables
int curDie = 6; //Variable to select which die to use
int die[] = {2, 4, 6, 8, 10, 12, 20, 7};

#define BUTTON_HOLD_DELAY_IN_MILLIS_UNTIL_HELD_ACTION 1000

//button routines
enum
{
   PROCESSED,
   NOT_PROCESSED
};

enum
{
  IS_NOT_PRESSED,
  IS_PRESSED,
  WAS_RELEASED,
  IS_HELD,
};

enum
{
  ACTIVE_HIGH,
  ACTIVE_LOW,
  ANALOG_THRESHOLD,
};

enum
{
  ROLL_BUTTON       = 0, //order of array
  DIE_SELECT_BUTTON = 1,
  TILT_INPUT        = 2,
  NUMBER_OF_BUTTONS = 3, //keep at end of list
};

//TBD, convert this to a struct def
byte button_state[NUMBER_OF_BUTTONS]               = { IS_NOT_PRESSED,  IS_NOT_PRESSED, IS_NOT_PRESSED };
byte button_pins [NUMBER_OF_BUTTONS]               = { ROLL_BUTTON_PIN, DIE_PIN,        TILT_INPUT_PIN };
byte button_types[NUMBER_OF_BUTTONS]               = { ACTIVE_LOW,      ACTIVE_HIGH,    ACTIVE_HIGH};
long last_interaction_timestamp[NUMBER_OF_BUTTONS] = { 0,               0,              0};
byte last_mode_button_state[NUMBER_OF_BUTTONS]     = { NOT_PROCESSED,   NOT_PROCESSED,  NOT_PROCESSED};

void setup() 
{
  //setup output to serial monitor
  Serial.begin(115200);

  Serial.println(F("Digital Dice v1.0"));

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Display driver failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the display buffer
  display.clearDisplay();

  //attempt to fix the RNG
  randomSeed(analogRead(A0));

  // initialize the button pins as an input:
  pinMode(ROLL_BUTTON_PIN, INPUT);
  pinMode(DIE_PIN,         INPUT);
  
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);

  display.fillScreen(BLACK); // erase the whole display
  display.drawBitmap((SCREEN_WIDTH - LOGO_WIDTH) / 2, (SCREEN_HEIGHT - LOGO_HEIGHT) / 2, logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(2000);
  display.fillScreen(BLACK);
  drawCurDie();
  drawDie();
  display.display();
  Serial.println(F("Startup Complete."));
}

void loop() 
{
  poll_input_signals();
  process_button_presses();
  
} //end loop

void process_roll_request()
{
  display.fillScreen(BLACK); // erase the whole display
  display.setTextColor(WHITE);
  display.setTextSize(3);
  int roll;
  int roll2 = 0;
  
  if(die[curDie] != 7)
  {
    roll = random(1, die[curDie]+1); // store the random number
  }
  else 
  {
    roll = random(1, die[curDie]); // store the random number
    roll2 = random(1, die[curDie]);
  }
  
  
  if (roll < 10) 
  {
      // single character number
        switch(die[curDie])
        {
          case 2:
            display.setCursor(57, 29); 
            break;
          case 4:
          display.setCursor(57, 29); 
            break;
          case 6:
            draw6(roll);
            break;
          case 7:
            draw7(roll, roll2);
            break;
          case 8:
            display.setCursor(57, 32); 
            break;
          case 10:
            display.setCursor(57, 32); 
            break;
          case 12:
            display.setCursor(57, 32); 
            break;
          case 20:
            display.setCursor(57, 22);
            break;
        }
      if((die[curDie] != 6) && (die[curDie] != 7)) 
      {
        display.println(roll); // write the roll
      }
      drawDie(); // draw the outline
      drawCurDie();
  }
  else 
  {
      // dual character number
        switch(die[curDie])
        {
          case 10:
            display.setCursor(47, 32); 
            break;
          case 12:
            display.setCursor(47, 32); 
            break;
          case 20:
            display.setCursor(47, 22);
            break;
        }
      
      display.println(roll); // write the roll 
      drawDie(); // draw the outline
      drawCurDie();
  }
  
  display.display(); // write to display
  delay(100);

}

void process_die_change_request()
{
  curDie++;
  if (curDie > 7)
  {
    curDie=0;
  }
  display.fillScreen(BLACK);
  drawCurDie();
  drawDie();
  display.display();
}

void drawDie() 
{

  switch(die[curDie])
  {
    case 2:
      display.drawCircle(SCREEN_WIDTH/2, 40, 23, SSD1306_WHITE);
      break;
    case 4:
      display.drawTriangle(40, 63, 88, 63, 64, 16, SSD1306_WHITE);
      break;
    case 6:
      display.drawRoundRect(40, 16, 48, 48, 5, WHITE);
      break;
    case 7:
      display.drawRoundRect(10, 16, 48, 48, 5, WHITE);
      display.drawRoundRect(70, 16, 48, 48, 5, WHITE);
      break;
    case 8:
      display.drawTriangle(40, 60, 88, 60, 64, 16, SSD1306_WHITE);
      display.drawTriangle(40, 60, 88, 60, 64, 80, SSD1306_WHITE);
      display.drawTriangle(40, 60, 64, 16, 40, 36, SSD1306_WHITE);
      display.drawTriangle(88, 60, 64, 16, 88, 36, SSD1306_WHITE);
      break;
    case 10:
      static const unsigned char PROGMEM d10[] =
      {
        0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xf8,0x1f,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0xf8,0x0f,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0xf0,0x0f,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0xe0,0x07,0xbe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7d,0xe0,0x03,0xdf,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xf3,0xc0,0x03,0xcf,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xe7,0xc0,0x01,0xe7,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xc7,0x80,0x00,0xf3,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x8f,0x00,0x00,0xf1,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x0f,0x00,0x00,0x78,0x7c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x1e,0x00,0x00,0x7c,0x3e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7c,0x3c,0x00,0x00,0x3c,0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x3c,0x00,0x00,0x1e,0x0f,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xe0,0x78,0x00,0x00,0x1f,0x07,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xc0,0xf0,0x00,0x00,0x0f,0x03,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x80,0xf0,0x00,0x00,0x07,0x81,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x01,0xe0,0x00,0x00,0x07,0x80,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x03,0xe0,0x00,0x00,0x03,0xc0,0x3e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7c,0x03,0xc0,0x00,0x00,0x01,0xe0,0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf8,0x07,0x80,0x00,0x00,0x01,0xe0,0x0f,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xe0,0x0f,0x80,0x00,0x00,0x00,0xf0,0x07,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xc0,0x0f,0x00,0x00,0x00,0x00,0x78,0x03,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x80,0x1e,0x00,0x00,0x00,0x00,0x78,0x01,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x00,0x1e,0x00,0x00,0x00,0x00,0x3c,0x00,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x00,0x3c,0x00,0x00,0x00,0x00,0x3e,0x00,0x3e,0x00,0x00,0x00,0x00,0x00,0x00,0x7c,0x00,0x78,0x00,0x00,0x00,0x00,0x1e,0x00,0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0xf8,0x00,0x78,0x00,0x00,0x00,0x00,0x0f,0x00,0x0f,0x80,0x00,0x00,0x00,0x00,0x03,0xe0,0x00,0xf0,0x00,0x00,0x00,0x00,0x0f,0x80,0x07,0xc0,0x00,0x00,0x00,0x00,0x07,0xc0,0x01,0xe0,0x00,0x00,0x00,0x00,0x07,0x80,0x03,0xe0,0x00,0x00,0x00,0x00,0x0f,0x80,0x01,0xe0,0x00,0x00,0x00,0x00,0x03,0xc0,0x01,0xf0,0x00,0x00,0x00,0x00,0x1f,0x00,0x03,0xc0,0x00,0x00,0x00,0x00,0x03,0xc0,0x00,0xf8,0x00,0x00,0x00,0x00,0x3e,0x00,0x07,0xc0,0x00,0x00,0x00,0x00,0x01,0xe0,0x00,0x3e,0x00,0x00,0x00,0x00,0x7c,0x00,0x07,0x80,0x00,0x00,0x00,0x00,0x00,0xf0,0x00,0x1f,0x00,0x00,0x00,0x00,0xf8,0x00,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x00,0x0f,0x80,0x00,0x00,0x01,0xe0,0x00,0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0x78,0x00,0x07,0xc0,0x00,0x00,0x07,0xc0,0x00,0x1e,0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x00,0x03,0xe0,0x00,0x00,0x0f,0x80,0x00,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x00,0x01,0xf0,0x00,0x00,0x1f,0x00,0x00,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0x1e,0x00,0x00,0xf8,0x00,0x00,0x3e,0x00,0x00,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x00,0x00,0x3e,0x00,0x00,0x7c,0x00,0x00,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x00,0x00,0x1f,0x00,0x00,0xf8,0x00,0x00,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x80,0x00,0x0f,0x80,0x01,0xf0,0x00,0x01,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xc0,0x00,0x07,0xc0,0x07,0xc0,0x00,0x1f,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xfc,0x00,0x03,0xe0,0x0f,0x80,0x01,0xff,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0xff,0xc0,0x01,0xf0,0x1f,0x00,0x3f,0xff,0xff,0xc0,0x00,0x00,0x00,0x00,0x03,0xff,0x7f,0xfe,0x00,0xf8,0x3e,0x07,0xff,0xe0,0x0f,0xfc,0x00,0x00,0x00,0x00,0x1f,0xf8,0x07,0xff,0xe0,0x3c,0x7c,0x7f,0xfe,0x00,0x01,0xff,0x80,0x00,0x00,0x00,0xff,0xc0,0x00,0x3f,0xff,0x1f,0xff,0xff,0xc0,0x00,0x00,0x3f,0xf0,0x00,0x00,0x0f,0xfc,0x00,0x00,0x03,0xff,0xff
      };
      display.drawBitmap(0, 16, d10, 128, 48, 1);
      break;
    case 12:
      display.drawLine(45, 63, 83, 63, SSD1306_WHITE);
      display.drawLine(45, 63, 35, 36, SSD1306_WHITE);
      display.drawLine(35, 36, 64, 16, SSD1306_WHITE);
      display.drawLine(64, 16, 93, 36, SSD1306_WHITE);
      display.drawLine(93, 36, 83, 63, SSD1306_WHITE);
      break;
    case 20:
      static const unsigned char PROGMEM d20[] =
      {
        0x38,0x00,0x3f,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0xfc,0x00,0x1c,0x38,0x00,0x0f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0x00,0x1c,0x38,0x00,0x03,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xc0,0x00,0x1c,0x38,0x00,0x03,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0xc0,0x00,0x1c,0x38,0x00,0x03,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0xc0,0x00,0x1c,0x38,0x00,0x07,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0xe0,0x00,0x1c,0x38,0x00,0x07,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0xe0,0x00,0x1c,0x38,0x00,0x0f,0x1c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0xf0,0x00,0x1c,0x38,0x00,0x0e,0x1e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x78,0x70,0x00,0x1c,0x38,0x00,0x1e,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x78,0x00,0x1c,0x38,0x00,0x1c,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xe0,0x38,0x00,0x1c,0x38,0x00,0x3c,0x07,0x80,0x00,0x00,0x00,0x00,0x00,0x01,0xe0,0x3c,0x00,0x1c,0x38,0x00,0x38,0x03,0xc0,0x00,0x00,0x00,0x00,0x00,0x03,0xc0,0x1c,0x00,0x1c,0x38,0x00,0x38,0x01,0xc0,0x00,0x00,0x00,0x00,0x00,0x03,0x80,0x1e,0x00,0x1c,0x38,0x00,0x78,0x01,0xe0,0x00,0x00,0x00,0x00,0x00,0x07,0x80,0x0e,0x00,0x1c,0x38,0x00,0x70,0x00,0xf0,0x00,0x00,0x00,0x00,0x00,0x0f,0x00,0x0f,0x00,0x1c,0x38,0x00,0xf0,0x00,0x70,0x00,0x00,0x00,0x00,0x00,0x0e,0x00,0x07,0x00,0x1c,0x38,0x00,0xe0,0x00,0x78,0x00,0x00,0x00,0x00,0x00,0x1e,0x00,0x07,0x80,0x1c,0x38,0x01,0xe0,0x00,0x3c,0x00,0x00,0x00,0x00,0x00,0x3c,0x00,0x03,0x80,0x1c,0x38,0x01,0xc0,0x00,0x1c,0x00,0x00,0x00,0x00,0x00,0x38,0x00,0x03,0x80,0x1c,0x38,0x03,0xc0,0x00,0x1e,0x00,0x00,0x00,0x00,0x00,0x78,0x00,0x03,0xc0,0x1c,0x38,0x03,0x80,0x00,0x0f,0x00,0x00,0x00,0x00,0x00,0xf0,0x00,0x01,0xc0,0x1c,0x38,0x07,0x80,0x00,0x07,0x00,0x00,0x00,0x00,0x01,0xe0,0x00,0x01,0xe0,0x1c,0x38,0x07,0x00,0x00,0x07,0x80,0x00,0x00,0x00,0x01,0xc0,0x00,0x00,0xe0,0x1c,0x38,0x0f,0x00,0x00,0x03,0xc0,0x00,0x00,0x00,0x03,0xc0,0x00,0x00,0xf0,0x1c,0x38,0x0e,0x00,0x00,0x03,0xc0,0x00,0x00,0x00,0x07,0x80,0x00,0x00,0x70,0x1c,0x38,0x1e,0x00,0x00,0x01,0xe0,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x78,0x1c,0x38,0x1c,0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x0f,0x00,0x00,0x00,0x38,0x1c,0x38,0x3c,0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x1e,0x00,0x00,0x00,0x3c,0x1c,0x38,0x38,0x00,0x00,0x00,0x78,0x00,0x00,0x00,0x1c,0x00,0x00,0x00,0x1c,0x1c,0x38,0x78,0x00,0x00,0x00,0x3c,0x00,0x00,0x00,0x3c,0x00,0x00,0x00,0x1e,0x1c,0x38,0x70,0x00,0x00,0x00,0x3c,0x00,0x00,0x00,0x78,0x00,0x00,0x00,0x0e,0x1c,0x38,0xf0,0x00,0x00,0x00,0x1e,0x00,0x00,0x00,0x70,0x00,0x00,0x00,0x0f,0x1c,0x38,0xe0,0x00,0x00,0x00,0x0f,0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x07,0x1c,0x39,0xe0,0x00,0x00,0x00,0x0f,0x00,0x00,0x01,0xe0,0x00,0x00,0x00,0x07,0x9c,0x39,0xc0,0x00,0x00,0x00,0x07,0x80,0x00,0x01,0xc0,0x00,0x00,0x00,0x03,0x9c,0x39,0xc0,0x00,0x00,0x00,0x03,0xc0,0x00,0x03,0xc0,0x00,0x00,0x00,0x03,0xdc,0x3b,0xc0,0x00,0x00,0x00,0x03,0xc0,0x00,0x07,0x80,0x00,0x00,0x00,0x01,0xdc,0x3b,0x80,0x00,0x00,0x00,0x01,0xe0,0x00,0x0f,0x00,0x00,0x00,0x00,0x01,0xfc,0x3f,0x80,0x00,0x00,0x00,0x00,0xe0,0x00,0x0e,0x00,0x00,0x00,0x00,0x00,0xfc,0x3f,0x00,0x00,0x00,0x00,0x00,0xf0,0x00,0x1e,0x00,0x00,0x00,0x00,0x00,0xfc,0x3f,0x00,0x00,0x00,0x00,0x00,0x78,0x00,0x3c,0x00,0x00,0x00,0x00,0x00,0x7c,0x3e,0x00,0x00,0x00,0x00,0x00,0x38,0x00,0x38,0x00,0x00,0x00,0x00,0x00,0x7c,0x3e,0x00,0x00,0x00,0x00,0x00,0x3c,0x00,0x78,0x00,0x00,0x00,0x00,0x00,0x7c,0x3c,0x00,0x00,0x00,0x00,0x00,0x1e,0x00,0xf0,0x00,0x00,0x00,0x00,0x00,0x3c,0x3c,0x00,0x00,0x00,0x00,0x00,0x0e,0x00,0xe0,0x00,0x00,0x00,0x00,0x00,0x3c,0x3f,0xe0,0x00,0x00,0x00,0x00,0x0f,0x01,0xe0,0x00,0x00,0x00,0x00,0x07,0xfc,0x3f,0xff,0xc0,0x00,0x00,0x00,0x07,0x83,0xc0,0x00,0x00,0x00,0x07,0xff,0xfc
      };
      display.drawBitmap(4, 16, d20, 120, 48, 1);
      break;
  }

    // display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
} //end drawDie

void drawCurDie()
{
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(2,0);
  if(die[curDie] == 2)
  {
    display.print("Coin:");
  } else 
  {
    display.print("Die:"); 
  }
  if( die[curDie] != 7)
  {
    display.print(die[curDie]);
  }
  else
  {
    display.print("6x2");
  }
  
  display.setTextSize(1);
  display.println(" sides");
} //end drawCurDie

void draw6(int roll)
{
  switch(roll)
  {
    case 1:
      display.fillCircle(display.width() / 2, 40, 5, SSD1306_WHITE);
      break;
    case 2:
      display.fillCircle(display.width() / 2-15, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15, 25, 5, SSD1306_WHITE);
      break;
    case 3:
      display.fillCircle(display.width() / 2, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2-15, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15, 25, 5, SSD1306_WHITE);
      break;
    case 4:
      display.fillCircle(display.width() / 2-15, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2-15, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15, 55, 5, SSD1306_WHITE);
      break;
    case 5:
      display.fillCircle(display.width() / 2, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2-15, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2-15, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15, 55, 5, SSD1306_WHITE);
      break;
    case 6:
      display.fillCircle(display.width() / 2-15, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2-15, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2-15, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15, 55, 5, SSD1306_WHITE);
      break;
  }//end roll
}// end of draw6

void draw7(int roll, int roll2)
{
  switch(roll)
  {
    case 1:
      display.fillCircle(display.width() / 2-30, 40, 5, SSD1306_WHITE);
      break;
    case 2:
      display.fillCircle(display.width() / 2-15-30, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15-30, 25, 5, SSD1306_WHITE);
      break;
    case 3:
      display.fillCircle(display.width() / 2-30, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2-15-30, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15-30, 25, 5, SSD1306_WHITE);
      break;
    case 4:
      display.fillCircle(display.width() / 2-15-30, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15-30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2-15-30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15-30, 55, 5, SSD1306_WHITE);
      break;
    case 5:
      display.fillCircle(display.width() / 2-30, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2-15-30, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15-30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2-15-30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15-30, 55, 5, SSD1306_WHITE);
      break;
    case 6:
      display.fillCircle(display.width() / 2-15-30, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15-30, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2-15-30, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15-30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2-15-30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15-30, 55, 5, SSD1306_WHITE);
      break;
  }//end roll

    switch(roll2)
    {
    case 1:
      display.fillCircle(display.width() / 2+30, 40, 5, SSD1306_WHITE);
      break;
    case 2:
      display.fillCircle(display.width() / 2-15+30, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15+30, 25, 5, SSD1306_WHITE);
      break;
    case 3:
      display.fillCircle(display.width() / 2+30, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2-15+30, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15+30, 25, 5, SSD1306_WHITE);
      break;
    case 4:
      display.fillCircle(display.width() / 2-15+30, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15+30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2-15+30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15+30, 55, 5, SSD1306_WHITE);
      break;
    case 5:
      display.fillCircle(display.width() / 2+30, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2-15+30, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15+30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2-15+30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15+30, 55, 5, SSD1306_WHITE);
      break;
    case 6:
      display.fillCircle(display.width() / 2-15+30, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15+30, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2-15+30, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15+30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2-15+30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2+15+30, 55, 5, SSD1306_WHITE);
      break;
  }//end roll
}// end draw7

//--------------------------------------
//          Button Subroutines         |
//--------------------------------------

void process_button_presses()
{
  for(byte i=0; i < NUMBER_OF_BUTTONS; i++)
  {
    if(button_state[i] == WAS_RELEASED)
    {
        //CLICKED ACTION

        //Note: action taken on WAS_RELEASED instead of IS_PRESSED to prevent extra clicks during button holds

        last_interaction_timestamp[i] = millis();

        if(last_mode_button_state[i] == NOT_PROCESSED)
        {
            //one action per press / hold
            last_mode_button_state[i] = PROCESSED;

            //button was clicked
            switch(i)
            {
              case ROLL_BUTTON:
              case TILT_INPUT:
                process_roll_request();
                break;
              case DIE_SELECT_BUTTON:
                process_die_change_request();
                break;
              default:
                Serial.print("Unknown button press: ");
                Serial.println(i);
                break;
            }
        }
    }
    else if ( ( button_state[i] == IS_HELD ) &&
              ( last_interaction_timestamp[i] + BUTTON_HOLD_DELAY_IN_MILLIS_UNTIL_HELD_ACTION < millis() ) )
    {
        //HELD ACTION

        last_interaction_timestamp[i] = millis();

        if(last_mode_button_state[i] == NOT_PROCESSED)
        {
            //one action per press / hold
            last_mode_button_state[i] = PROCESSED;

            //button was held
            switch(i)
            {
              case ROLL_BUTTON:
              case TILT_INPUT:
                break;
              case DIE_SELECT_BUTTON:
                //TBD: enable or disable sound
                break;
              default:
                Serial.print("Unknown button press: ");
                Serial.println(i);
                break;
            }
        }
    }
    else if (button_state[i] == IS_NOT_PRESSED)
    {
        //reset timers
        last_interaction_timestamp[i] = millis();
        last_mode_button_state[i] = NOT_PROCESSED;
    }

  }// end for loop
}

void poll_input_signals()
{
  static byte last_button_read   [NUMBER_OF_BUTTONS] = {false, false};
  static byte current_button_read[NUMBER_OF_BUTTONS] = {false, false};

    for(byte i=0; i < NUMBER_OF_BUTTONS; i++)
    {
        if( button_types[i] == ACTIVE_HIGH )
        {
            /* read digital active high beam signal */
            current_button_read[i] = digitalRead(button_pins[i]);
        }
        else if( button_types[i] == ANALOG_THRESHOLD )
        {
            /* read analog input, active high */
            //current_button_read[i] = get_analog_signal_as_bool(i);
        }
        else // button_types[i] == ACTIVE_LOW
        {
            /* read digital active low button signal */
            current_button_read[i] = !digitalRead(button_pins[i]);
        }
        
        if (!last_button_read[i] && current_button_read[i])
        {
            //button was just pressed
            button_state[i] = IS_PRESSED;
            Serial.print("Button ");
            Serial.print(i);
            Serial.println(" was pressed.");
        }
        else if  (last_button_read[i] && current_button_read[i])
        {
            button_state[i] = IS_HELD;
            Serial.print("Button ");
            Serial.print(i);
            Serial.println(" is held.");
        }
        else if (last_button_read[i] && !current_button_read[i])
        {
            //button was just released
            button_state[i] = WAS_RELEASED;
            Serial.print("Button ");
            Serial.print(i);
            Serial.println(" was released.");
        }
        else
        {
            button_state[i] = IS_NOT_PRESSED;
        }

        last_button_read[i] = current_button_read[i];
    }
}