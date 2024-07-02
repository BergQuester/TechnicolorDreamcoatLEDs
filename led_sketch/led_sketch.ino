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
      rainbowCycle(20);
      break;
    case '2':
      marquee(500);
      copyLEDs();
      break;
    default:
      clearAll();
      copyLEDs();
      break;
  }
}

// Marquee
void marquee(unsigned long speedDelay) {
  unsigned long millisTime = millis();
  unsigned long cycle = (millisTime % speedDelay) / (speedDelay / 3);

  for(int i = 0; i < NUM_LEDS; i++ ) {
    if (i % 3 == cycle) {
      setPixel(i, 255, 255, 255);
    } else {
      setPixel(i, 0, 0, 0);
    }
  }

  FastLED.show();
}

// Rainbow display
void rainbowCycle(int SpeedDelay) {

  byte *c;
  uint16_t i, j;

  for(j=0; j<256*5/10; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< NUM_LEDS; i++) {
      c=Wheel(((i * 256 / NUM_LEDS) + j*10) & 255);
      setPixel(i, *c, *(c+1), *(c+2));
      if (delayWhileInMode(SpeedDelay, '1')) {
        return;
      }
    }
    copyLEDs();
    FastLED.show();
    if (delayWhileInMode(SpeedDelay, '1')) {
      return;
    }
  }
}

byte * Wheel(byte WheelPos) {
  static byte c[3];
 
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
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
  for(int i = 0; i < MIDDLE_COUNT; i++) {
    middle_leds[MIDDLE_COUNT - i] = leds[i];
  }
}

// FastLED Convienence methods

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
  FastLED.show();
}

void clearAll() {
  setAll(0,0,0);
}