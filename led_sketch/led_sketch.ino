#include "FastLED.h"

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

// FastLED Config
#define DATA_PIN    3
#define TR_PIN      3
#define TL_PIN      3
#define MR_PIN      3
#define ML_PIN      3
#define BR_PIN      3
#define BL_PIN      3
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    100
#define TR_COUNT      3
#define TL_COUNT      3
#define MR_COUNT      3
#define ML_COUNT      3
#define BR_COUNT      3
#define BL_COUNT      3
#define BRIGHTNESS  255

CRGB leds[NUM_LEDS];
CRGB leds2[NUM_LEDS];
CRGB leds3[NUM_LEDS];
CRGB leds4[NUM_LEDS];
CRGB leds5[NUM_LEDS];
CRGB leds6[NUM_LEDS];

// Which scene we are displaying
char currentMode = '0';

void setup() {

  delay(3000); // 3 second delay for recovery

  Serial.begin(9600); // Set up serial coms

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,2,COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip)
    .setDither(BRIGHTNESS < 255);

  FastLED.addLeds<LED_TYPE,3,COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip)
    .setDither(BRIGHTNESS < 255);

  FastLED.addLeds<LED_TYPE,4,COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip)
    .setDither(BRIGHTNESS < 255);

  FastLED.addLeds<LED_TYPE,5,COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip)
    .setDither(BRIGHTNESS < 255);

  FastLED.addLeds<LED_TYPE,6,COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip)
    .setDither(BRIGHTNESS < 255);

  FastLED.addLeds<LED_TYPE,7,COLOR_ORDER>(leds, NUM_LEDS)
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
      break;
    default:

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