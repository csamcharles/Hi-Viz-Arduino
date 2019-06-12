// (c) Michael Schoeffler 2017, http://www.mschoeffler.de
#include "FastLED.h"

#define DATA_PIN 2
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 24
#define BRIGHTNESS 96
#define NUM_PROGRAMS 3

CRGB leds[NUM_LEDS];
// variables will change:
int buttonState = false;  
int prgm = 0;
const byte button = 3;
long lastRead = 0;
long debounceDelay = 500;
const int num_led_index = NUM_LEDS - 1;

void setup() {
  delay(3000); // initial delay of a few seconds is recommended
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // initializes LED strip
  FastLED.setBrightness(BRIGHTNESS);// global brightness
  pinMode(button, INPUT);
  attachInterrupt(button, programChange, RISING);
}

// switches off all LEDs
void showProgramCleanUp(long delayTime) {
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  delay(delayTime);
}

// switches on all LEDs. Each LED is shown in random color.
// numIterations: indicates how often LEDs are switched on in random colors
// delayTime: indicates for how long LEDs are switched on.
//void showProgramRandom(int numIterations, long delayTime) {
//  for (int iteration = 0; iteration < numIterations; ++iteration) {
//    for (int i = 0; i < NUM_LEDS; ++i) {
//      leds[i] = CHSV(random8(),255,255); // hue, saturation, value
//    }
//    FastLED.show();
//    delay(delayTime);
//  }
//}

// Shifts a single pixel from the start of strip to the end.
// crgb: color of shifted pixel
// delayTime: indicates how long the pixel is shown on each LED
//void showProgramShiftSinglePixel(CRGB crgb, long delayTime) {
//  for (int i = 0; i < NUM_LEDS; ++i) {
//    leds[i] = crgb;
//    FastLED.show();
//    delay(delayTime);
//    leds[i] = CRGB::Black;
//  }
//}

// Shifts multiple pixel from the start of strip to the end. The color of each pixel is randomized.
// delayTime: indicates how long the pixels are shown on each LED
void showProgramShiftMultiPixel(long delayTime) {
  int i = 0; 
  while(1) { 
    for (int j = i; j > 0; --j) {
      leds[j] = leds[j-1];
    }
    if(i < NUM_LEDS) {
      i++;
    }
    if(buttonState) {
        break;
    }
    CRGB newPixel = CHSV(random8(), 255, 255);
    leds[0] = newPixel;
    FastLED.show();
    delay(delayTime);
  }
}

void showProgramStrobe(CRGB crgb, long delayTime) {
  int j = 0;
  while(j < 2) {
    for (int i = 0; i < NUM_LEDS; ++i) {
      leds[i] = (j % 2 == 0) ? CRGB::Black : crgb; // hue, saturation, value
    }
    FastLED.show();
    delay(delayTime);
    j++;
  }
}


// main program
void loop() {
  //Reset button state
  buttonState = false;
  switch(prgm) {
//    case 0: 
//      showProgramStrobe(CRGB::Red, 300);
//      break;
//    case 1: 
//      showProgramStrobe(CRGB::Magenta, 150);
//    break;
//    case 2: 
//      showProgramStrobe(CRGB::Blue, 75);
//    break;
//    case 3: 
//      showProgramShiftMultiPixelToggle(CRGB::Red, CRGB::Purple, 100);
//      break;
//    case 4:
//      showProgramShiftMultiPixelToggle(CRGB::Blue, CRGB::Green, 100);
//      break;
//    case 5:
//      showProgramShiftMultiPixelColored(CRGB::Red, 10);
//      break;
//    case 6:
//      showProgramShiftMultiPixel(100);
//      break;
//    case 7:
//      showProgramShiftMultiPixelColoredSnake(CRGB::Green, 6, 50);
//      break;
//    case 8:
//      showProgramShiftMultiPixelColoredSnake(CRGB::Red, 6, 50);
//      break;
//    case 9:
//      showProgramShiftMultiPixelColoredSnake(CRGB::Magenta, 6, 50);
//      break;
//    case 10:
//      showProgramMeetMiddle(CRGB::Red, CRGB::Blue, 100);
//      break;
//    case 11:
//      showProgramMeetMiddle(CRGB::Pink, CRGB::Green, 100);
//      break;
//    case 12:
//      showProgramRainbow(75);
//    break;
//    case 13:
//      showProgramShiftMultiPixelColoredSnake(CRGB::White, 13, 25);
//    break;
//    case 14:
//      showProgramShiftMultiPixelColoredSnake(CRGB::Yellow, 13, 25);
//    break;
//    case 15:
//      showProgramShiftMultiPixelColoredSnake(CRGB::Magenta, 13, 25);
//    break;
//    case 16:
//      showProgramAlternate(CRGB::Red, 250);
//    break;
//    case 17:
//      showProgramAlternate(CRGB::Green, 250);
//    break;
    default:
      showProgramCleanUp(2500); // clean up
    break;   
  }
}

void programChange() {
  if(!buttonState && ((millis() - lastRead) > debounceDelay)) {
    lastRead = millis();
    prgm++;
    prgm = prgm % NUM_PROGRAMS;
    buttonState = true;
 }
}
  
