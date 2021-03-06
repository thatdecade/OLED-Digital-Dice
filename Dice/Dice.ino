
/* Electronic Dice:

    Author: Dustin Westaby
    Date: June 10, 2021

    Sources:
     - 2017 Joe Coburn (joecoburn) https://www.makeuseof.com/tag/roll-style-diy-electronic-d20/
     - 2018 Tomas Carlsson (TomasC62) https://www.instructables.com/Arduino-Oled-Dice/
     - 2021 Fernando Hernandez (Dsk001) https://www.prusaprinters.org/prints/66933-electronic-dice
     - 2021 Josh Segatto (alienslacker / alieneila) https://twitter.com/alienslacker/status/1398511306546765828

    Update the logo_bmp with your own image using LCDAssistant

    If you are not using a battery, comment out #define USE_BATTERY
    If you are not using a neopixel strip, comment out #define USE_NEOPIXELS

    Usage:
    Press ROLL button to roll dice.
    Shake upside down to roll dice (tilt sensors).
    Hold ROLL button to disable tilt sensors.
    Press DICE button to change dice.
    Hold DICE button to check battery info.
    
    LEDs are connected to D0 so they are forced off during Deep Sleep

*/


/* ************************************************* */

/* These are some top level options without touching main code */

// OPTIONS
//#define USE_SPI_OLED //comment or uncomment out the right communication
#define USE_IIC_OLED

#define USE_BATTERY //comment out to disable
#define MAX_BATTERY_VOLTAGE 4.2 //Use 4.5 for a 130k series resistor, use 4.2 for a 100k series resistor

#define NUMBER_OF_TILT_SENSORS 1 //up to 2, set to 0 to disable

#define NUMBER_OF_NEOPIXELS 0 //set to 0 to disable
#define TYPE_OF_NEOPIXELS (NEO_GRB + NEO_KHZ800)

#define ENABLE_MAGIC_8BALL

//#define ANIMATE_ROLLS

// TIMING
#define HOW_FAST_BUTTONS_AND_SCREEN_ARE_PROCESSED_IN_MS    100 // 100 = 10 Hz

#define DEEP_SLEEP_AFTER_THIS_MANY_MS_OF_INACTIVITY     900000 // 900,000 = 15 * 60 * 1000 = 15 mins

#define BUTTON_HOLD_DELAY_IN_MILLIS_UNTIL_HELD_ACTION     2000 // 2 seconds

#define RESERVE_POWER_PERCENT_FOR_SELF_SHUTDOWN 25 //percent

/* ************************************************* */

#include "Arduino.h"
#include <ESP8266WiFi.h>
#ifdef USE_IIC_OLED
#include <Wire.h>
#else
#include <SPI.h>
#endif
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#if (NUMBER_OF_NEOPIXELS > 0)
#include <Adafruit_NeoPixel.h>
#endif

//Not all pins are interchangable, refer to https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/

//These are the new pin outs
#ifdef USE_IIC_OLED
#define DISPLAY_SCL_PIN      5 // D1 - I2C
#define DISPLAY_SDA_PIN      4 // D2 - I2C
#define ROLL_BUTTON_PIN      0 // D3 - Active Low Input
#define DIE_BUTTON_PIN      15 // D8 - Active High Input
#define TILT_SENSOR_1_PIN   14 // D5 - Active High Interrupt
#define NEOPIXEL_DATA_PIN   13 // D7 - Data Output
#define TILT_SENSOR_2_PIN    3 // RX - Active High Interrupt
#define BATTERY_MONITOR_PIN A0 // A0 - Analog Input, Connect to battery through 130k resistor
#define BUTTON_LED_PIN      16 // D0 - Active High Output
#else //USE_SPI_OLED
#define OLED_MOSI           D7 // Wemos to D1 on OLED
#define OLED_CLK            D5 // Wemos to D0 on OLED 
#define OLED_CS             D2 // Wemos to CS on OLED
#define OLED_DC             D3 // Wemos to DC on OLED
#define OLED_RESET          D4 // Wemos to RES OLED
#define ROLL_BUTTON_PIN     D1 // Active Low Input
#define DIE_BUTTON_PIN      D8 // Active High Input
#define TILT_SENSOR_1_PIN    3 // RX, Active High Interrupt
#define BATTERY_MONITOR_PIN A0 // Analog Input, Connect to battery through 130k or 100k resistor
#define BUTTON_LED_PIN      D0 // Active High Output
//D6 = MISO
//TX = GPIO1
#endif

//screen config
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL),
// On a D1 Mini:             D2(SDA), D1(SCL)
#ifdef USE_IIC_OLED
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#else //USE_SPI_OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
#endif

#define LOGO_HEIGHT 43 //64
#define LOGO_WIDTH 128 //82
static const unsigned char PROGMEM logo_bmp[] =
{ //output from LCDAssistant, byte orientation: Horizontal
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0xFF,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFF,0xFF,0xFF,0xF8,0xFF,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xF8,0xFF,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xFF,0x00,0x00,0x07,0xF8,0xFF,0xFF,0x9F,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x40,0x01,0xFF,0x00,0x00,0x07,0xF8,0xFF,0xFF,0x9F,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x38,0x39,0xFF,0xFF,0xFF,0xFF,0xF8,0xFF,0xFF,0x9F,0xFF,0xFF,0xFF,0xFF,0xF1,0xFC,0x03,0x39,0xFF,0xFF,0xFF,0xFF,0xF8,0xFF,0xFF,0x9F,0xFF,0xFF,0xFF,0xFF,0xF1,0xFE,0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xF8,0xFF,0xFF,0x9F,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xF8,0xFF,0xFF,0x9F,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xF8,0xFF,0xFF,0x9F,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xF8,0xFF,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFF,0xFF,0xFF,0xF8,0xFF,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xF8,0xFF,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x1C,0x03,0x0F,0xFF,0x00,0xFC,0x1F,0xFF,0xE0,0x1C,0x00,0xFE,0x00,0xC0,0x06,0x60,0x1C,0x03,0x0F,0xFF,0x03,0xCF,0x1F,0xFF,0xE0,0x1C,0x00,0xFF,0xE0,0x60,0x0E,0x60,0x1C,0x03,0x0C,0x00,0x06,0x01,0x00,0x30,0x00,0x3C,0x00,0xC0,0x70,0x30,0x0C,0x60,0x34,0x06,0x0C,0x00,0x06,0x00,0x00,0x30,0x00,0x36,0x00,0xC0,0x30,0x30,0x18,0x30,0x36,0x06,0x0C,0x00,0x0C,0x00,0x00,0x30,0x00,0x26,0x00,0xC0,0x18,0x18,0x30,0x30,0x26,0x06,0x0C,0x00,0x0C,0x00,0x00,0x30,0x00,0x63,0x00,0xC0,0x18,0x0C,0x30,0x30,0x22,0x06,0x0C,0x00,0x0E,0x00,0x00,0x30,0x00,0x63,0x00,0xC0,0x30,0x0C,0x60,0x30,0x62,0x0C,0x0C,0x00,0x07,0x00,0x00,0x30,0x00,0xC1,0x00,0xC0,0x30,0x06,0xC0,0x18,0x63,0x0C,0x0F,0xFF,0x03,0xE0,0x00,0x30,0x00,0xC1,0x80,0xFF,0xC0,0x03,0xC0,0x18,0x43,0x0C,0x0F,0xFF,0x01,0xFE,0x00,0x30,0x00,0x81,0x80,0xFF,0xC0,0x03,0x80,0x18,0x41,0x0C,0x0C,0x00,0x00,0x0F,0x00,0x30,0x01,0x80,0x80,0xC0,0x30,0x01,0x80,0x18,0xC1,0x18,0x0C,0x00,0x00,0x03,0x80,0x30,0x01,0x80,0xC0,0xC0,0x18,0x01,0x80,0x0C,0xC1,0x98,0x0C,0x00,0x00,0x01,0x80,0x30,0x03,0x00,0xC0,0xC0,0x18,0x01,0x80,0x0C,0x81,0x98,0x0C,0x00,0x00,0x01,0x80,0x30,0x03,0xFF,0xE0,0xC0,0x18,0x01,0x80,0x0C,0x80,0x98,0x0C,0x00,0x00,0x01,0x80,0x30,0x03,0x00,0x60,0xC0,0x18,0x01,0x80,0x0D,0x80,0x90,0x0C,0x00,0x00,0x01,0x80,0x30,0x06,0x00,0x20,0xC0,0x18,0x01,0x80,0x07,0x80,0xF0,0x0C,0x00,0x08,0x03,0x80,0x30,0x06,0x00,0x30,0xC0,0x38,0x01,0x80,0x07,0x00,0xF0,0x0F,0xFF,0x0F,0x07,0x00,0x30,0x0C,0x00,0x30,0xFF,0xF0,0x01,0x80,0x07,0x00,0x70,0x0F,0xFF,0x03,0xFC,0x00,0x30,0x0C,0x00,0x18,0xFF,0xC0,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

#if (NUMBER_OF_NEOPIXELS > 0)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMBER_OF_NEOPIXELS, NEOPIXEL_DATA_PIN, TYPE_OF_NEOPIXELS);
int started = 0;
#endif

//variables
int curDie = 6; //Variable to select which die to use
#ifdef ENABLE_MAGIC_8BALL
int die[] = {2, 4, 6, 8, 10, 12, 64, 20, 7}; // 7 = 2D6, 64 = Magic 8 ball.
int numDie = 9;
char *magic8Answers[] = {
  "    It is certain", 
  "  It is decidedly so", 
  "    Without a doubt", 
  "    Yes definitely", 
  "  You may rely on it",
  "   As I see it, yes", 
  "      Most likely", 
  "     Outlook good", 
  "          Yes", 
  "   Signs point to yes", 
  "    Reply hazy, try          again", 
  "    Ask again later", 
  "  Better not tell you         now", 
  "   Cannot predict now", 
  "  Concentrate and ask        again", 
  "   Don't count on it", 
  "    My reply is no", 
  "   My sources say no", 
  "  Outlook not so good", 
  "     Very doubtful"
  };
int numMagic8Answers = 20;
#else
int die[] = {2, 4, 6, 8, 10, 12, 20, 7};
int numDie = 8;
#endif

#ifdef ANIMATE_ROLLS
int animationTicker = 0;
#endif
/* ***************** */
//button routines
typedef enum switch_status { PROCESSED, NOT_PROCESSED, };
typedef enum switch_state  { IS_NOT_PRESSED, IS_PRESSED, WAS_RELEASED, IS_HELD, };
typedef enum input_type    { ACTIVE_HIGH, ACTIVE_LOW, ANALOG_THRESHOLD, ACTIVE_HIGH_NO_PULLUP, };
typedef enum switch_type   { PUSHBUTTON, TILT_SENSOR, };

enum button_link
{
  /* order of button_info list */
  ROLL_BUTTON = 0,
  DIE_SELECT_BUTTON,
#if NUMBER_OF_TILT_SENSORS > 0
  TILT_INPUT_1,
#endif
#if (NUMBER_OF_TILT_SENSORS > 1) && defined(USE_IIC_OLED)
  TILT_INPUT_2,
#endif
  /* keep at end of list */
  NUMBER_OF_BUTTONS,
};

typedef struct
{
  byte          pin;
  input_type    in_type;
  switch_state  state;
  switch_status status;
  long          last_interaction_timestamp;
  byte          last_button_read;
  byte          current_button_read;
  switch_type   sw_type;
} button_struct;

button_struct button_info[NUMBER_OF_BUTTONS] =
{
  { /* button_info[ROLL_BUTTON] */
    /* pin       = */ ROLL_BUTTON_PIN,
    /* in_type   = */ ACTIVE_LOW,
    /* state     = */ IS_NOT_PRESSED,
    /* status    = */ NOT_PROCESSED,
    /* timestamp = */ 0,
    /* last      = */ false,
    /* current   = */ false,
    /* sw_type   = */ PUSHBUTTON,
  },
  { /* button_info[DIE_SELECT_BUTTON] */
    /* pin       = */ DIE_BUTTON_PIN,
    /* in_type   = */ ACTIVE_HIGH_NO_PULLUP,
    /* state     = */ IS_NOT_PRESSED,
    /* status    = */ NOT_PROCESSED,
    /* timestamp = */ 0,
    /* last      = */ false,
    /* current   = */ false,
    /* sw_type   = */ PUSHBUTTON,
  },
#if NUMBER_OF_TILT_SENSORS > 0
  { /* button_info[TILT_INPUT_1] */
    /* pin       = */ TILT_SENSOR_1_PIN,
    /* in_type   = */ ACTIVE_HIGH,
    /* state     = */ IS_NOT_PRESSED,
    /* status    = */ NOT_PROCESSED,
    /* timestamp = */ 0,
    /* last      = */ false,
    /* current   = */ false,
    /* sw_type   = */ TILT_SENSOR,
  },
#endif
#if (NUMBER_OF_TILT_SENSORS > 1) && defined(USE_IIC_OLED)
  {                /* button_info[TILT_INPUT_2] */
  /* pin       = */ TILT_SENSOR_2_PIN,
  /* in_type   = */ ACTIVE_HIGH,
  /* state     = */ IS_NOT_PRESSED,
  /* status    = *//NOT_PROCESSED,
  /* timestamp = */ 0,
  /* last      = */ false,
  /* current   = */ false,
  /* sw_type   = */ TILT_SENSOR,
  },
#endif
};

/* ***************** */

byte roll_flag = false;
byte tilt_flag = false;
byte tilt_enabled = true;

byte activity_flag = true;

#ifndef USE_BATTERY
ADC_MODE(ADC_VCC);
#endif

void setup()
{
  //setup output to serial monitor
  Serial.begin(115200);

#if (NUMBER_OF_NEOPIXELS > 0)
  strip.begin();
  strip.setBrightness(50);
  strip.show(); // Initialize all pixels to 'off'
#endif

  Serial.println(F("Digital Dice v1.0"));

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
#ifdef USE_IIC_OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
#else //USE_SPI_OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC))
#endif
  {
    Serial.println(F("Display driver failed"));
    for (;;); // Don't proceed, loop forever
  }

  // Clear the display buffer
  display.clearDisplay();

  // initialize the button pins as an input:
  for (byte i = 0; i < NUMBER_OF_BUTTONS; i++)
  {
    if (button_info[i].in_type == ACTIVE_HIGH_NO_PULLUP)
    {
      pinMode(button_info[i].pin, INPUT); //special pin, Active = 3.3V
    }
    else
    {
      pinMode(button_info[i].pin, INPUT_PULLUP);
    }

#if NUMBER_OF_TILT_SENSORS > 0
    //start die rollers
    if (button_info[i].sw_type == TILT_SENSOR)
    {
      attachInterrupt(digitalPinToInterrupt(button_info[i].pin), handle_tilt_interrupt, CHANGE);
    }
#endif

  }

#ifndef USE_BATTERY
  pinMode(BATTERY_MONITOR_PIN, INPUT); //battery monitor, add solder jumper to J2 on the v1.2.0 battery shield. See https://arduinodiy.wordpress.com/2016/12/25/monitoring-lipo-battery-voltage-with-wemos-d1-minibattery-shield-and-thingspeak/
#endif

  pinMode(BUTTON_LED_PIN, OUTPUT);
  digitalWrite(BUTTON_LED_PIN, HIGH);

  display.fillScreen(BLACK); // erase the whole display
  display.drawBitmap((SCREEN_WIDTH - LOGO_WIDTH) / 2, (SCREEN_HEIGHT - LOGO_HEIGHT) / 2, logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(2000);

  display.fillScreen(BLACK);
  drawNameOfDie();
  drawShapeOfDie();
  display.display();

  WiFi.mode(WIFI_OFF); //We spend most of our time in light sleep with the radio off

  Serial.println(F("Startup Complete."));
}

void loop()
{

#if (NUMBER_OF_NEOPIXELS > 0)
  if ( started == 0 ) {
    colorWipe(strip.Color(0, 255, 0), 50); // Green
    started++;
  }
#endif

  //button and LED stuff
  poll_input_signals();
  process_button_presses();
  //process_led_state();

  //roll the dice when requested
  if (roll_flag || (tilt_flag && tilt_enabled))
  {
#ifdef ANIMATE_ROLLS
    animationTicker = 7;
  }
  if(animationTicker)
  {
    animationTicker--;
#endif
    process_roll_request();
    roll_flag = false;
    tilt_flag = false;
  }

  //power savings
  //check for activity timeout, then light sleep until the next poll interval
  check_for_inactivity_then_power_down();
  //check_for_low_battery_then_power_down();
  sleep_for_poll_rate(HOW_FAST_BUTTONS_AND_SCREEN_ARE_PROCESSED_IN_MS);

} //end loop

void callback() {
  Serial.flush();
}

void setup_text_to_display(int text_color, int text_size, int cursor_x, int cursor_y)
{
  display.setTextColor(text_color);
  display.setTextSize(text_size);
  display.setCursor(cursor_x, cursor_y);
}

void check_for_low_battery_then_power_down()
{
#ifdef USE_BATTERY
  static byte shutdown_timer = 10;
  
  float percent = get_battery_percent();

  if (percent < RESERVE_POWER_PERCENT_FOR_SELF_SHUTDOWN)
  {
    if (shutdown_timer > 0)
    {
      display_voltage();
    }
    else
    {
      turn_off_oled_and_deep_sleep();
    }
    shutdown_timer--;
  }
#endif
}

void check_for_inactivity_then_power_down()
{
  static long last_activity = 0;

  if (activity_flag)
  {
    activity_flag = false;
    last_activity = millis();
  }

  if (millis() >= (last_activity + DEEP_SLEEP_AFTER_THIS_MANY_MS_OF_INACTIVITY - 10000))
  {
    long seconds = ( (last_activity + DEEP_SLEEP_AFTER_THIS_MANY_MS_OF_INACTIVITY) - millis() ) / 1000;
    shutdown_message(seconds);
  }

  if (millis() >= (last_activity + DEEP_SLEEP_AFTER_THIS_MANY_MS_OF_INACTIVITY))
  {
    turn_off_oled_and_deep_sleep();
  }
}

void shutdown_message(long seconds)
{
  Serial.printf("Inactive for %ld minutes. Shutting down in %ld seconds.\n", DEEP_SLEEP_AFTER_THIS_MANY_MS_OF_INACTIVITY / 60000, seconds / 1000);

  display.fillScreen(BLACK); // erase the whole display
  setup_text_to_display(/* color = */ WHITE, /* size = */ 1, /* x = */ 0, /* y = */ 0);
  display.print("Shutting Down in");

  setup_text_to_display(/* color = */ WHITE, /* size = */ 2, /* x = */ 0, /* y = */ 18);
  display.print(seconds);

  setup_text_to_display(/* color = */ WHITE, /* size = */ 1, /* x = */ 0, /* y = */ 44);
  display.print("seconds.");

  setup_text_to_display(/* color = */ WHITE, /* size = */ 2, /* x = */ 0, /* y = */ 56);
  if (tilt_enabled)
  {
    display.print("Shake dice to stop.");
  }
  else
  {
    display.print("Press any button to stop.");
  }
  display.display(); // write to display
}

void turn_off_oled_and_deep_sleep()
{
  digitalWrite(BUTTON_LED_PIN, LOW);

  display.fillScreen(BLACK); // erase the whole display
  setup_text_to_display(/* color = */ WHITE, /* size = */ 1, /* x = */ 0, /* y = */ 0);
  display.print("Shutting Down");

  setup_text_to_display(/* color = */ WHITE, /* size = */ 2, /* x = */ 0, /* y = */ 24);
  display.print("Good Bye");
  display.display(); // write to display
  delay(1000);

  display.fillScreen(BLACK); // erase the whole display
  display.display(); // write to display

  //For this to work, connect D0 and RST pins with a jumper wire.
  Serial.println("Enter deep sleep mode");
  ESP.deepSleep(0, RF_DISABLED);
}

void sleep_for_poll_rate(long rate)
{
  //Source: https://www.mischianti.org/2019/11/21/wemos-d1-mini-esp8266-the-three-type-of-sleep-mode-to-manage-energy-savings-part-4/

  // Enter light sleep mode

  wifi_set_opmode(NULL_MODE);
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  wifi_fpm_open();
  wifi_fpm_set_wakeup_cb(callback);
  wifi_fpm_do_sleep(rate * 1000 );
  delay(rate + 1);

  // Exit light sleep mode

  wifi_set_sleep_type(NONE_SLEEP_T);
}

#if NUMBER_OF_TILT_SENSORS > 0
ICACHE_RAM_ATTR void handle_tilt_interrupt()
{
  /* This function must be very short */
  tilt_flag = true;
}
#endif

#if NUMBER_OF_TILT_SENSORS > 0
void enable_disable_tilt()
{
  tilt_enabled = !tilt_enabled;
  display.fillScreen(BLACK); // erase the whole display
  setup_text_to_display(/* color = */ WHITE, /* size = */ 1, /* x = */ 0, /* y = */ 0);
  display.print("Motion Roll");
  setup_text_to_display(/* color = */ WHITE, /* size = */ 1, /* x = */ 0, /* y = */ 12);
  if (tilt_enabled)
  {
    Serial.println("Tilt Enabled");
    display.print("Enabled");
    display.display(); // write to display
    delay(1000);
    display.setCursor(0, 24);
    display.print("Shake upside down to roll");
    display.display(); // write to display
  }
  else
  {
    Serial.println("Tilt Disabled");
    display.print("Disabled");
    display.display(); // write to display
    delay(1000);
    display.setCursor(0, 24);
    display.print("Press button to roll");
    display.display(); // write to display
  }
  delay(1000);
  display.fillScreen(BLACK);
  drawNameOfDie();
  drawShapeOfDie();
  display.display();
}
#endif

int get_random_number(int min, int max)
{
  //attempt to fix the RNG
  //this gives me a different number on sequential boots, and two dice side by side give different numbers
  randomSeed(ESP.getCycleCount());

  return random(min, max);
}

void process_roll_request()
{
  display.fillScreen(BLACK); // erase the whole display
  display.setTextColor(WHITE);
  display.setTextSize(3);
  int roll;
  int roll2 = 0;

#if (NUMBER_OF_NEOPIXELS > 0)
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
#endif

  // store the random number
#ifdef ENABLE_MAGIC_8BALL
  if (die[curDie] == 64)
  {
     roll = random(0, numMagic8Answers); 
  }
  else if (die[curDie] == 7)
#else
  if (die[curDie] == 7)
#endif
  {
    roll = get_random_number(1, die[curDie]);
    roll2 = get_random_number(1, die[curDie]);
  }
  else
  {
    roll = get_random_number(1, die[curDie] + 1);
  }

#ifdef ENABLE_MAGIC_8BALL  
  if (die[curDie] == 64) 
  {
    drawShapeOfDie();
    drawNameOfDie();
    drawMagic8Ball(roll);
  } 
  else if (roll < 10)
#else
  if (roll < 10)
#endif
  {
    // single character number
    switch (die[curDie])
    {
      case 2:
      case 4:
        display.setCursor(57, 29);
        display.println(roll); // write the roll
        break;
      case 6:
        draw6(roll);
        break;
      case 7:
        draw2xD6(roll, roll2);
        break;
      case 8:
      case 10:
      case 12:
        display.setCursor(57, 32);
        display.println(roll); // write the roll
        break;
      case 20:
        display.setCursor(57, 22);
        display.println(roll); // write the roll
        break;
      // case 64: // Magic 8 Ball Case handled before switch statement
    }
    drawShapeOfDie(); // draw the outline
    drawNameOfDie();
  }
  else
  {
    // dual character number
    switch (die[curDie])
    {
      case 10:
      case 12:
        display.setCursor(47, 32);
        break;
      case 20:
        display.setCursor(47, 22);
        break;
    }

    display.println(roll); // write the roll
    drawShapeOfDie(); // draw the outline
    drawNameOfDie();
  }

  display.display(); // write to display

#if (NUMBER_OF_NEOPIXELS > 0)
  if (die[curDie] != 7)
  {
    if ( roll == 1 ) {
      theaterChase(strip.Color(127,   0,   0), 50); // Red
    }
    else if ( roll == 20 ) {
      rainbowCycle(20);
    }
    else {
      colorWipe(strip.Color(0, 255, 0), 50); // Green
    }
  } else
  {
    if ( roll + roll2 == 2 ) {
      theaterChase(strip.Color(127,   0,   0), 50); // Red
    }
    else {
      colorWipe(strip.Color(0, 255, 0), 50); // Green
    }
  }
#endif

}

void process_die_change_request()
{
  curDie++;
  if (curDie >= numDie)
  {
    curDie = 0;
  }
  display.fillScreen(BLACK);
  drawNameOfDie();
  drawShapeOfDie();
  display.display();
}

void drawShapeOfDie()
{

  switch (die[curDie])
  {
    case 2: // Coin
      display.drawCircle(SCREEN_WIDTH / 2, 40, 23, SSD1306_WHITE);
      break;
    case 4: // D4
      display.drawTriangle(40, 63, 88, 63, 64, 16, SSD1306_WHITE);
      break;
    case 6: // D6
      display.drawRoundRect(40, 16, 48, 48, 5, WHITE);
      break;
    case 7: // 2XD6
      display.drawRoundRect(10, 16, 48, 48, 5, WHITE);
      display.drawRoundRect(70, 16, 48, 48, 5, WHITE);
      break;
    case 8: // D8
      display.drawTriangle(40, 60, 88, 60, 64, 16, SSD1306_WHITE);
      display.drawTriangle(40, 60, 88, 60, 64, 80, SSD1306_WHITE);
      display.drawTriangle(40, 60, 64, 16, 40, 36, SSD1306_WHITE);
      display.drawTriangle(88, 60, 64, 16, 88, 36, SSD1306_WHITE);
      break;
    case 10: // D10
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
#ifdef ENABLE_MAGIC_8BALL  
    case 64: // Magic 8 Ball
      display.drawCircle(SCREEN_WIDTH/2, 40, 23, SSD1306_WHITE);
      display.drawTriangle(46, 27, 82, 27, 64, 63, SSD1306_WHITE);
      break;
#endif
  }
} //end drawShapeOfDie

void drawNameOfDie()
{
  setup_text_to_display(/* color = */ WHITE, /* size = */ 2, /* x = */ 2, /* y = */ 0);
  if (die[curDie] == 2)
  {
    display.print("Coin:");
#ifdef ENABLE_MAGIC_8BALL
  } else if(die[curDie] == 64){
    display.print("   8 Ball"); 
#endif
  } else
  {
    display.print("Die:");
  }
  
#ifdef ENABLE_MAGIC_8BALL
  if( die[curDie] == 64)
    return;
#endif

  if ( die[curDie] == 7)
  {
    display.print("6x2");
  }
  else
  {
    display.print(die[curDie]);
  }

  display.setTextSize(1);
  display.println(" sides");
} //end drawNameOfDie

void draw6(int roll)
{
  switch (roll)
  {
    case 1:
      display.fillCircle(display.width() / 2, 40, 5, SSD1306_WHITE);
      break;
    case 2:
      display.fillCircle(display.width() / 2 - 15, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15, 25, 5, SSD1306_WHITE);
      break;
    case 3:
      display.fillCircle(display.width() / 2, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 - 15, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15, 25, 5, SSD1306_WHITE);
      break;
    case 4:
      display.fillCircle(display.width() / 2 - 15, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 - 15, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15, 55, 5, SSD1306_WHITE);
      break;
    case 5:
      display.fillCircle(display.width() / 2, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 - 15, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 - 15, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15, 55, 5, SSD1306_WHITE);
      break;
    case 6:
      display.fillCircle(display.width() / 2 - 15, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 - 15, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 - 15, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15, 55, 5, SSD1306_WHITE);
      break;
  }//end roll
}// end of draw6

void draw2xD6(int roll, int roll2)
{
  switch (roll)
  {
    case 1:
      display.fillCircle(display.width() / 2 - 30, 40, 5, SSD1306_WHITE);
      break;
    case 2:
      display.fillCircle(display.width() / 2 - 15 - 30, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15 - 30, 25, 5, SSD1306_WHITE);
      break;
    case 3:
      display.fillCircle(display.width() / 2 - 30, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 - 15 - 30, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15 - 30, 25, 5, SSD1306_WHITE);
      break;
    case 4:
      display.fillCircle(display.width() / 2 - 15 - 30, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15 - 30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 - 15 - 30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15 - 30, 55, 5, SSD1306_WHITE);
      break;
    case 5:
      display.fillCircle(display.width() / 2 - 30, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 - 15 - 30, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15 - 30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 - 15 - 30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15 - 30, 55, 5, SSD1306_WHITE);
      break;
    case 6:
      display.fillCircle(display.width() / 2 - 15 - 30, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15 - 30, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 - 15 - 30, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15 - 30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 - 15 - 30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15 - 30, 55, 5, SSD1306_WHITE);
      break;
  }//end roll

  switch (roll2)
  {
    case 1:
      display.fillCircle(display.width() / 2 + 30, 40, 5, SSD1306_WHITE);
      break;
    case 2:
      display.fillCircle(display.width() / 2 - 15 + 30, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15 + 30, 25, 5, SSD1306_WHITE);
      break;
    case 3:
      display.fillCircle(display.width() / 2 + 30, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 - 15 + 30, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15 + 30, 25, 5, SSD1306_WHITE);
      break;
    case 4:
      display.fillCircle(display.width() / 2 - 15 + 30, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15 + 30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 - 15 + 30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15 + 30, 55, 5, SSD1306_WHITE);
      break;
    case 5:
      display.fillCircle(display.width() / 2 + 30, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 - 15 + 30, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15 + 30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 - 15 + 30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15 + 30, 55, 5, SSD1306_WHITE);
      break;
    case 6:
      display.fillCircle(display.width() / 2 - 15 + 30, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15 + 30, 40, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 - 15 + 30, 55, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15 + 30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 - 15 + 30, 25, 5, SSD1306_WHITE);
      display.fillCircle(display.width() / 2 + 15 + 30, 55, 5, SSD1306_WHITE);
      break;
  }//end roll
}// end draw2xD6

#ifdef ENABLE_MAGIC_8BALL
void drawMagic8Ball(int roll) {
  display.fillRect(0, 32, 128, 22, BLACK);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,36);
  display.print(magic8Answers[roll]);
} // end drawMagic8Ball
#endif

float get_battery_voltage()
{
#ifdef USE_BATTERY
  float raw = analogRead(BATTERY_MONITOR_PIN);
  float volts = raw / 1023 * MAX_BATTERY_VOLTAGE;
#else
  float raw = ESP.getVcc();
  float volts = raw / 1000;
#endif

  return volts;
}

#ifdef USE_BATTERY
float get_battery_percent()
{
  const float battery_max = 4.20; //maximum voltage of battery
  const float battery_min = 3.0;  //minimum voltage of battery before shutdown
  float volts = get_battery_voltage();

  return ((volts - battery_min) / (battery_max - battery_min)) * 100;
}
#endif

void display_voltage()
{
  do
  {
    float volts = get_battery_voltage();
#ifdef USE_BATTERY
    float percent = get_battery_percent();
#endif
    display.fillScreen(BLACK); // erase the whole display
    setup_text_to_display(/* color = */ WHITE, /* size = */ 1, /* x = */ 0, /* y = */ 0);
    display.print("Battery Voltage:");

    setup_text_to_display(/* color = */ WHITE, /* size = */ 2, /* x = */ 0, /* y = */ 24);
    Serial.printf("The internal VCC reads %1.2f volts\n", volts);
    display.printf("%1.2f volts\n", volts);

#ifdef USE_BATTERY
    display.setCursor(0, 50);
    display.printf("%1.0f %%\n", percent);
#endif

    display.display(); // write to display

    //if we are here because of a held button, monitor the buttons and maintain loop timers
    if (button_info[DIE_SELECT_BUTTON].state == IS_HELD)
    {
        poll_input_signals();
        ESP.wdtFeed(); //main loop does this without the call
        check_for_inactivity_then_power_down(); //if user holds button too long, turn off //TBD: This wigs out, when this happens both routines are writing to screen
        sleep_for_poll_rate(1000);
    }
  } while (button_info[DIE_SELECT_BUTTON].state == IS_HELD);

  //go back to main screen
  display.fillScreen(BLACK);
  drawNameOfDie();
  drawShapeOfDie();
  display.display();
}

//--------------------------------------
//          Button Subroutines         |
//--------------------------------------

void process_button_presses()
{
  for (byte i = 0; i < NUMBER_OF_BUTTONS; i++)
  {
    if (button_info[i].state == WAS_RELEASED)
    {
      //CLICKED ACTION

      //Note: action taken on WAS_RELEASED instead of IS_PRESSED to prevent extra clicks during button holds

      button_info[i].last_interaction_timestamp = millis();

      if (button_info[i].status == NOT_PROCESSED)
      {
        activity_flag = true;

        //one action per press / hold for all buttons
        for (byte j = 0; j < NUMBER_OF_BUTTONS; j++)
        {
          button_info[j].status = PROCESSED;
        }

        //button was clicked
        switch (i)
        {
#if NUMBER_OF_TILT_SENSORS > 0
          case TILT_INPUT_1:
#endif
#if (NUMBER_OF_TILT_SENSORS > 1) && defined(USE_IIC_OLED)
          case TILT_INPUT_2:
#endif
            Serial.println("Tilt");
            tilt_flag = true;
            break;
          case ROLL_BUTTON:
            roll_flag = true;
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
    else if ( ( button_info[i].state == IS_HELD ) &&
              ( button_info[i].last_interaction_timestamp + BUTTON_HOLD_DELAY_IN_MILLIS_UNTIL_HELD_ACTION < millis() ) )
    {
      //HELD ACTION

      button_info[i].last_interaction_timestamp = millis();

      if (button_info[i].status == NOT_PROCESSED)
      {
        activity_flag = true;

        //one action per press / hold for all buttons
        for (byte j = 0; j < NUMBER_OF_BUTTONS; j++)
        {
          button_info[j].status = PROCESSED;
        } //TBD, move this to a routine

        //button was held
        switch (i)
        {
          case ROLL_BUTTON:
#if NUMBER_OF_TILT_SENSORS > 0
            enable_disable_tilt();
#endif
            break;
#if NUMBER_OF_TILT_SENSORS > 0
          case TILT_INPUT_1:
#endif
#if (NUMBER_OF_TILT_SENSORS > 1) && defined(USE_IIC_OLED)
          case TILT_INPUT_2:
#endif
            //TBD
            break;
          case DIE_SELECT_BUTTON:
            //show battery voltage
            display_voltage();
            break;
          default:
            Serial.print("Unknown button press: ");
            Serial.println(i);
            break;
        }
      }
    }
    else if (button_info[i].state == IS_NOT_PRESSED)
    {
      //reset timers
      button_info[i].last_interaction_timestamp = millis();
      button_info[i].status = NOT_PROCESSED;
    }

  }// end for loop
}

void poll_input_signals()
{
  for (byte i = 0; i < NUMBER_OF_BUTTONS; i++)
  {
    if ( (button_info[i].in_type == ACTIVE_HIGH) || (button_info[i].in_type == ACTIVE_HIGH_NO_PULLUP) )
    {
      /* read digital active high beam signal */
      button_info[i].current_button_read = digitalRead(button_info[i].pin);
    }
    else if ( button_info[i].in_type == ANALOG_THRESHOLD )
    {
      /* read analog input, active high */
      //button_info[i].current_button_read = get_analog_signal_as_bool(i);
    }
    else // button_info[i].in_type == ACTIVE_LOW
    {
      /* read digital active low button signal */
      button_info[i].current_button_read = !digitalRead(button_info[i].pin);
    }

    if (!button_info[i].last_button_read && button_info[i].current_button_read)
    {
      //button was just pressed
      button_info[i].state = IS_PRESSED;
      //Serial.print("Button ");
      //Serial.print(i);
      //Serial.println(" was pressed.");
    }
    else if  (button_info[i].last_button_read && button_info[i].current_button_read)
    {
      button_info[i].state = IS_HELD;
      //Serial.print("Button ");
      //Serial.print(i);
      //Serial.println(" is held.");
    }
    else if (button_info[i].last_button_read && !button_info[i].current_button_read)
    {
      //button was just released
      button_info[i].state = WAS_RELEASED;
      //Serial.print("Button ");
      //Serial.print(i);
      //Serial.println(" was released.");
    }
    else
    {
      button_info[i].state = IS_NOT_PRESSED;
    }

    button_info[i].last_button_read = button_info[i].current_button_read;
  }
}

//--------------------------------------
//        Neopixel Subroutines         |
//--------------------------------------

#if (NUMBER_OF_NEOPIXELS > 0)
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
#endif
