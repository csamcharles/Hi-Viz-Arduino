// (c) Michael Schoeffler 2017, http://www.mschoeffler.de
#include "FastLED.h"

#define DATA_PIN 5
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 276
#define BRIGHTNESS 128
#define NUM_PROGRAMS 7

CRGB leds[NUM_LEDS];
// variables will change:
int buttonState = false;  
int prgm = 0;
int lastPrgm = 0;
const byte button = 3;
long lastRead = 0;
long debounceDelay = 500;
const int num_led_index = NUM_LEDS - 1;

void setup() {
  delay(3000); // initial delay of a few seconds is recommended
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // initializes LED strip
  FastLED.setBrightness(BRIGHTNESS);// global brightness
  pinMode(button, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(button), programChange, FALLING);
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
void showProgramRandom(int numIterations, long delayTime) {
  for (int iteration = 0; iteration < numIterations; ++iteration) {
    for (int i = 0; i < NUM_LEDS; ++i) {
      leds[i] = CHSV(random8(),255,255); // hue, saturation, value
    }
    FastLED.show();
    delay(delayTime);
  }
}

// Shifts a single pixel from the start of strip to the end.
// crgb: color of shifted pixel
// delayTime: indicates how long the pixel is shown on each LED
void showProgramShiftSinglePixel(CRGB crgb, long delayTime) {
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = crgb;
    FastLED.show();
    delay(delayTime);
    leds[i] = CRGB::Black;
  }
}

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
//    if(buttonState)
//        break;
  }
}

void showProgramShiftMultiPixelRandom(CRGB crgb, long delayTime) {
  int oldcolor = crgb;
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
    int newcolor = (int(oldcolor * 0.6) + int(random8() * 0.4)) % 255;
    oldcolor = newcolor;
    CRGB newPixel = CHSV(newcolor, 255, 255);
    leds[0] = newPixel;
    FastLED.show();
    delay(delayTime);
  }
}

void showProgramShiftMultiPixelToggle(CRGB crgb1, CRGB crgb2, long delayTime) {
  int i = 0;
  while (1) { 
    for (int j = i; j > 0; --j) {
      leds[j] = leds[j-1];
    }
    if(i < NUM_LEDS) {
      i++;
    } else {
      i--;
    }
    if(buttonState) {
        break;
    }
   CRGB newPixel = (i % 2 == 0) ? crgb1 : crgb2;
    leds[0] = newPixel;
    FastLED.show();
    delay(delayTime);

  }
}

void showProgramShiftMultiPixelColored(CRGB crgb, long delayTime) {
  int oldcolor = crgb;
  int i = 0;
    while (1) { 
      for (int j = i; j > 0; --j) {
        leds[j] = leds[j-1];
      }
      if(i < NUM_LEDS) {
        i++;
      }
      if(buttonState) {
        break;
      }
     // CRGB newPixel = (i % 2 == 0) ? crgb1 : crgb2;
      int newcolor = (oldcolor+5) % 255;
      oldcolor = newcolor;
      CRGB newPixel = CHSV(newcolor, 255, 255);
      leds[0] = newPixel;
      
      FastLED.show();
      delay(delayTime);

  
    }
}

void showProgramStrobe(CRGB crgb, long delayTime) {
  int j = 0;
  while(1) {
    for (int i = 0; i < NUM_LEDS; ++i) {
      leds[i] = (j % 2 == 0) ? CRGB::Black : crgb; // hue, saturation, value
    }
    if(buttonState) {
        break;
    }
    FastLED.show();
    delay(delayTime);
    j++;
  }
}

void showProgramAlternate(CRGB crgb, long delayTime) {
  int j = 0;
  while(1) {
    for (int i = 0; i < NUM_LEDS; ++i) {
      leds[(i+j)%NUM_LEDS] = (i % 2 == 0) ? CRGB::Black : crgb; // hue, saturation, value
    }
    if(buttonState) {
        break;
    }
    FastLED.show();
    delay(delayTime);
    j++;
  }
}

void showProgramShiftMultiPixelRandomSnake(int snakeLength, long delayTime) {
    int i = 0;
    while(1) { 
      for (int j = i + snakeLength; j > i; --j) {
        leds[j%NUM_LEDS] = CHSV(random8(), 255, 255);
      }
      leds[i%NUM_LEDS] = CRGB::Black;
      if(buttonState) {
        break;
      }
      
      FastLED.show();
      delay(delayTime);
      i++;
    }
}

void showProgramShiftMultiPixelColoredSnake(CRGB crgb, int snakeLength, long delayTime) {
    int i = 0;
    while(1) {
      for (int j = i + snakeLength; j > i; --j) {
        leds[j%NUM_LEDS] = crgb;
      }
      leds[i%NUM_LEDS] = CRGB::Black;
      if(buttonState) {
        break;
      }
      
      FastLED.show();
      delay(delayTime);
      i++;
    }
}

void showProgramMeetMiddle(CRGB crgb1, CRGB crgb2, long delayTime) {
    bool bounce = false;
    int i = 0;
    while(1) {

      // first stripe
      leds[i] = crgb1;
      leds[i + 1] = crgb1;
      leds[NUM_LEDS - (i + 1)] = crgb2;
      leds[NUM_LEDS - (i + 2)] = crgb2; 

      if (bounce == false && i >= (NUM_LEDS / 2) - 2) {
        bounce = true;
      } else if (i == 0) {
        bounce = false;
      }

      if(buttonState) {
        break;
      }
      
      FastLED.show();
      delay(delayTime);
      
      if(!bounce) {
        leds[i] = CRGB::Black;
        leds[NUM_LEDS - (i + 1)] = CRGB::Black;
        i++;
      } else {
        leds[i + 1] = CRGB::Black;
        leds[NUM_LEDS - (i + 2)] = CRGB::Black;
        i--;
      } 
    }
}

void showProgramRainbow(long delayTime) {
    bool bounce = false;
    int i = 0;
    while(1){
      leds[i] = CRGB::Red;
      leds[NUM_LEDS - i - 1] = CRGB::Red;
      
      leds[i+1] = CRGB::Orange;
      leds[NUM_LEDS - i - 2] = CRGB::Orange;

      leds[i+2] = CRGB::Yellow;
      leds[NUM_LEDS - i - 3] = CRGB::Yellow;
      
      leds[i+3] = CRGB::Green;
      leds[NUM_LEDS - i - 4] = CRGB::Green;

      leds[i+4] = CRGB::Blue;
      leds[NUM_LEDS - i - 5] = CRGB::Blue;
      
      leds[i+5] = CRGB::Purple;
      leds[NUM_LEDS - i - 6] = CRGB::Purple;
      
      if(bounce == false && i >= (NUM_LEDS / 2) - 5) {
        bounce = true;
      } else if( i == 0) {
        bounce = false;
      }
           
      if(buttonState) {
        break;
      }
      FastLED.show();
      leds[i] = CRGB::Black;
      leds[NUM_LEDS - i - 1] = CRGB::Black;
      leds[i+5] = CRGB::Black;
      leds[NUM_LEDS - i - 6] = CRGB::Black;
      
      if(bounce) {
          i--;
      } else {
          i++;
      }
      
      delay(delayTime);
    }
}


// main program
void loop() {
  //Reset button state
  buttonState = false;

  if (lastPrgm != prgm) {
    showProgramCleanUp(0);
  }
  
  lastPrgm = prgm;
  switch(prgm) {
    case 0:
      showProgramShiftMultiPixelColoredSnake(CRGB::Green, 12, 20);
      break;
    case 1: 
      showProgramShiftMultiPixelColoredSnake(CRGB::Blue, 15, 30);
    break;
    case 2:
      showProgramShiftMultiPixelColoredSnake(CRGB::Red, 20, 50);
    break;
    case 3:
      showProgramMeetMiddle(CRGB::DeepPink, CRGB::DeepSkyBlue, 10);
      break;
    case 4:
      showProgramMeetMiddle(CRGB::Green, CRGB::Blue, 10);
      break;
    case 5:
      showProgramShiftMultiPixelRandomSnake(20, 10);
      break;
    case 6:
      showProgramRainbow(10);
      break;
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
//      showProgramShiftMultiPixelColoredSnake(CRGB::Green, 12, 20);
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
