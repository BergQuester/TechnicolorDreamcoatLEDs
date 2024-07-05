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
#define BRIGHTNESS  22

// Which scene we are displaying
char currentMode = ' ';

#define DREAMCOAT '1'
#define POTIPHAR '2'
#define GO_JOSEPH '3'
#define MARQUEE '4'

#define FADE_OUT 'f'
#define BLUE 'b'

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
    case DREAMCOAT:
      dreamcoat(20);
      break;
    case MARQUEE:
      marquee(500);
      break;
    case GO_JOSEPH:
      goJoseph(20);
      break;
    case BLUE:
      FastLED.setBrightness(BRIGHTNESS);
      setAll(255, 0, 0);  // For some reason the order is BRG
      copyLEDs();
      FastLED.show();
      break;
    case FADE_OUT:
      fadeout(0.9);
      break;
    case POTIPHAR:
      potiphar();
      break;
    default:
      clearAll();
      break;
  }
}

bool potipharData[101] = {true, true, true, false, true, true, true, false, false, true, true, true, false, true,
 true, true, true, true, true, false, false, false, true, true, true, false, true, true, true, true, true, true,
  false, false, true, true, true, true, true, true, false, true, true, true, true, true, true, true, true, true, 
  true, true, true, false, false, false, true, true, true, true, true, true, false, false, false, false, false, 
  false, true, true, true, false, true, true, true, false, false, false, false, false, false, false, false, true, 
  true, true, true, true, true, true, true, true, false, false, true, true, true, false, false, false, false};

void potiphar() {
  FastLED.setBrightness(16);
  setAll(10, 255, 70);
  copyLEDs();
  FastLED.show();
}

// Fadeout effect takes that current values and fades to nothing
void fadeout(float fadeScale) {
  uint8_t fade = FastLED.getBrightness();
  do {
    fade = fade * fadeScale;
    // Serial.println("Fade: " + String(fade));
    FastLED.setBrightness(fade);
    FastLED.show();
    delay(20);
  } while(fade > 0);

  currentMode = 'n';
  clearAll();
  FastLED.show();
  Serial.println("Fade completed");
}

// Marquee
void marquee(unsigned long speedDelay) {
  FastLED.setBrightness(64);
  do {
    unsigned long millisTime = millis();
    unsigned long cycle = (millisTime % speedDelay) / (speedDelay / 3);

    for(int i = 0; i < NUM_LEDS; i++ ) {
      if (i % 3 == cycle) {
        setPixel(i, 30, 255, 100); // For some reason the order is BRG
      } else {
        setPixel(i, 0, 0, 0);
      }
    }
    copyLEDs();
    FastLED.show();
    if (cycle == 0) {
      serialEvent();
    }
  } while (currentMode == MARQUEE);
}

// Rainbow display
void dreamcoat(int SpeedDelay) {

  byte *c;
  uint16_t i, j;

  FastLED.setBrightness(BRIGHTNESS);

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< NUM_LEDS; i++) {
      c=Wheel(((i * 256 / NUM_LEDS) + j*2) & 255);
      setPixel(i, *c, *(c+1), *(c+2));
    }

    serialEvent(); // Check for user input since we're blocking the main loop
    
    if (currentMode != DREAMCOAT) {
      if (currentMode != FADE_OUT) {
        clearAll();
      }
      return;
    } else {
      copyLEDs();
      FastLED.show();
    }
    delay(SpeedDelay);
  }
}

void goJoseph(int SpeedDelay) {

  byte *c;
  uint16_t i, j;

  FastLED.setBrightness(BRIGHTNESS);

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< NUM_LEDS; i++) {
      c=Wheel(((i * 256 / NUM_LEDS) + j*10) & 255);
      setPixel(i, *c, *(c+1), *(c+2));
    }
    
    serialEvent(); // Check for user input since we're blocking the main loop

    if (currentMode != GO_JOSEPH) {
      if (currentMode != FADE_OUT) {
        clearAll();
      }
      return;
    } else {
      copyLEDs();
      FastLED.show();
    }
    delay(SpeedDelay);
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

    if (inputChar == 'h') {
      Serial.println("*******************************************************");
      Serial.println("h: This help");
      Serial.println(String(DREAMCOAT) + ": Dreamcoat");
      Serial.println(String(POTIPHAR) + ": Potiphar / Jacob Comes to Egypt");
      Serial.println(String(GO_JOSEPH) + ": Go Go Go Joseph / Mega Mix");
      Serial.println(String(MARQUEE) + ": Pharoh / Pharoh Mega Mix Reprise");
      Serial.println(String(FADE_OUT) + ": Fade_Out");
      Serial.println(String(BLUE) + ": Blue");
      Serial.println("Space bar (or any other key not used above): Clear");
      Serial.println("*******************************************************");
    } else {
      currentMode = inputChar;
    }

    switch (currentMode) {
      case DREAMCOAT:
        Serial.println("Dreamcoat");
        break;
      case MARQUEE:
        Serial.println("Pharaoh");
        break;
      case GO_JOSEPH:
        Serial.println("Go Go Go Joseph");
        break;
      case BLUE:
        Serial.println("BLUE!");
        break;
      case POTIPHAR:
        Serial.println("Potiphar");
        break;
      case FADE_OUT:
        Serial.println("Starting fadeout");
        break;
      default:
        break;
    }

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
  copyLEDs();
  FastLED.show();
}