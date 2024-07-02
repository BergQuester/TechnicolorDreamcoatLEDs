#include "FastLED.h"

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

// FastLED Config
#define BOTTOM_PIN  2
#define MIDDLE_PIN  3
#define TOP_PIN     4

//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS      200 // Bottom
#define MIDDLE_COUNT  100
#define TOP_COUNT     MIDDLE_COUNT

CRGB leds[NUM_LEDS];  // Bottom
CRGB middle_leds[MIDDLE_COUNT];
CRGB top_leds[TOP_COUNT];

// #define BRIGHTNESS  255
#define BRIGHTNESS  32

// Which scene we are displaying
char currentMode = '0';

void setup() {

  delay(3000); // 3 second delay for recovery

  Serial.begin(9600); // Set up serial coms
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,BOTTOM_PIN,COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip)
    .setDither(BRIGHTNESS < 255);

  FastLED.addLeds<LED_TYPE,MIDDLE_PIN,COLOR_ORDER>(middle_leds, MIDDLE_COUNT)
    .setCorrection(TypicalLEDStrip)
    .setDither(BRIGHTNESS < 255);

  FastLED.addLeds<LED_TYPE,TOP_PIN,COLOR_ORDER>(leds, TOP_COUNT)
    .setCorrection(TypicalLEDStrip)
    .setDither(BRIGHTNESS < 255);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  while (!Serial) {
    ;  // Wait
  }

  Serial.println("Connected");
}

// the loop function runs over and over again forever
void loop() {

  switch (currentMode) {
    case '1':
      rainbow();
      copyLEDs();
      break;
    default:
      clearAll();
      copyLEDs();
      break;
  }

  FastLED.show();
}

// Rainbow display
// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void rainbow() 
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    CRGB newcolor = CHSV( hue8, sat8, bri8);
    
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS-1) - pixelnumber;
    
    nblend( leds[pixelnumber], newcolor, 64);
  }
}

// Creates a dealy while in a mode
// Returns true if the dealy was canceled by a mode change
bool delayWhileInMode(unsigned long delayTime, char mode) {

  unsigned long remainingDelay = delayTime;
  unsigned long checkInterval = 100;
  do {
    // If we are no longer in the mode, break out of this delay
    if (currentMode != mode) { return true; }

    // The check interval is the greater of `remainingDelay` and `checkInterval`
    // And the check interval is subtracted from the remaining delay
    if (remainingDelay < checkInterval) {
      checkInterval = remainingDelay;
      remainingDelay = 0;
    } else {
      remainingDelay -= checkInterval;
    }

    delay(checkInterval);
  } while (remainingDelay != 0);

  // If we the mod changed while in the last delay, we still need to return `true`
  // otherwise, return false
  return currentMode != mode;
}

// Get user input
void serialEvent() {
  if (Serial.available()) {
    // get the new byte:
    char inputChar = (char)Serial.read();
    if (inputChar == '\n') {  // don't care about newlines
      return;
    }
    currentMode = inputChar;
    Serial.println(currentMode);

    // clear buffer (only interested in the first character)
    while (Serial.available()) {
      char devNull = (char)Serial.read();
    }
  }
}

// Copy LEDs
void copyLEDs() {
  for(int i = 0; i < TOP_COUNT; i++) {
    top_leds[i] = leds[i];
    middle_leds[TOP_COUNT - i] = leds[i];
  }
}

// FastLED Convienence methods
void showStrip() {
   // FastLED
   FastLED.show();
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}

void clearAll() {
  setAll(0,0,0);
}