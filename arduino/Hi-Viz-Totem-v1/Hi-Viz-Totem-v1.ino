// (c) Michael Schoeffler 2017, http://www.mschoeffler.de
#include "FastLED.h"

#define BUTTON 3
#define DATA_PIN 5
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 276
#define SIGN_START 105
#define BRIGHTNESS 128
#define NUM_PROGRAMS 9
#define BUTTON_DELAY 400

#define NUM_MODES 3

#define MODE_SWITCH 1500 // How long to hold button for mode switch

// For snakey mode
#define MAX_SNAKES 4
#define SNAKE_LENGTH 60

CRGB leds[NUM_LEDS];
// variables will change:
int buttonState = false;  
int prgm = 0;
int lastPrgm = 0;
long lastRead = 0;
const int num_led_index = NUM_LEDS - 1;
int state = 2;

// For snakey mode
int firstSnake = 0;
int snakeCount = 0;
typedef struct snakeState {
  float color;
  float colorStep;
  int pos;
} snakeState;
snakeState snakes[MAX_SNAKES];
int signHue = 0;
int signState = -1;

// For auto snakey mode
float nextSnake = 0;

// For changing modes
long lastButtonStateTime = 0;
int buttonHeld = false;

void setup() {
  delay(3000); // initial delay of a few seconds is recommended
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // initializes LED strip
  FastLED.setBrightness(BRIGHTNESS);// global brightness
  pinMode(BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON), programChange, FALLING);
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

void showProgramShiftMultiPixelRandomSnake(int snakeLength, float changeStep, long delayTime) {
    int i = -snakeLength;
    float randColor = random8();
    
    while(1) {
      // THIS HAS TO GO IN YOUR LOOPY BOI, we should think of a better way
      checkButtonHeld();
      
      for (int j = 0; j < snakeLength; j++) {
        int index = i + j + 1;
        if (index < NUM_LEDS && index > 0)
          leds[i + j] = CHSV(round(randColor) % 256, 255, 255 - (255 / snakeLength) * (snakeLength - j));
      }

      if (i > 0)
        leds[i - 1] = CRGB::Black;
      
      if(buttonState) {
        break;
      }
      
      FastLED.show();
      delay(delayTime);
      i++;
      randColor += changeStep;

      if (i >= NUM_LEDS) {
        i = -snakeLength;
        randColor = random8();
      }
    }
}

void showProgramShiftMultiPixelColoredSnake(CRGB crgb, int snakeLength, long delayTime) {
    int i = 0;
    while(1) {
      // THIS HAS TO GO IN YOUR LOOPY BOI, we should think of a better way
      checkButtonHeld();
      
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
      // THIS HAS TO GO IN YOUR LOOPY BOI, we should think of a better way
      checkButtonHeld();

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
      // THIS HAS TO GO IN YOUR LOOPY BOI, we should think of a better way
      checkButtonHeld();
      
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

void addSnake() {
  if (snakeCount < MAX_SNAKES) {
    snakeCount++;
    int nextIndex = (firstSnake + (snakeCount - 1)) % MAX_SNAKES;
    snakes[nextIndex].pos = 0;
    snakes[nextIndex].color = random8();
    snakes[nextIndex].colorStep = random(0, 50) / 10.0;
  }
}

void startSign(int hue) {
  signHue = hue;

  if (signState > 4) {
    signState = 5;
  } else if (signState < 0) {
    signState = 0;
  }
}

void updateSign() {
  if (signState < 0)
    return;

  // Fade in for the first few values of sign state, then slow fade out
  int brightness;
  if (signState <= 4) {
    brightness = signState * 32;
  } else {
    brightness = 128 - (signState - 4);
  }

  if (brightness < 0) {
    signState = -1;
    return;
  }
  
  for (int i = SIGN_START; i < NUM_LEDS; i++) {
    leds[i] = CHSV(signHue, 255, brightness);
  }

  signState++;
}

void updateSnakes() {
  for (int i = firstSnake; i < firstSnake + snakeCount; i++) {
    snakeState *snake = &(snakes[i % MAX_SNAKES]);
    if (snake->pos == SIGN_START) {
      startSign(snake->color);
    }
    
    // Draw this snake
    int j;
    for (j = 0; j < SNAKE_LENGTH; j++) {      
      if (snake->pos - j >= 0 && snake->pos - j < SIGN_START) {
        leds[snake->pos - j] = CHSV(round(snake->color) % 256, 255, 255 - ((255 / SNAKE_LENGTH) * j));
      }
    }
    
    // Set pixel after snake to black
    if (snake->pos - j >= 0) {
      leds[snake->pos - j] = CRGB::Black;
    }

    // Move snake forward
    snake->pos++;

    // Update snake color (if not past sign)
    if (snake->pos <= SIGN_START) {
      snake->color += snake->colorStep;
    }
  }

  // If the first snake is all the way past the sign, 'delete' it by marking the next snake in line as the first
  if (snakeCount > 0 && snakes[firstSnake].pos - SNAKE_LENGTH >= SIGN_START) {
    firstSnake = (firstSnake + 1) % MAX_SNAKES;
    snakeCount--;
  }
}

void showProgramState() {
    CRGB color;
    switch (state) {
      case 0:
        color = CRGB::Red;
        break;
      case 1:
        color = CRGB::Blue;
        break;
      case 2:
        color = CRGB::Green;
        break;
      default:
        color = CRGB::Purple; // If we see this things aren't right
        break;
    }
    
    for (int i = 0; i < NUM_LEDS; i++) {
      if (i < 20) {
        leds[i] = color;
      } else {
        leds[i] = CRGB::Black;
      }
    }
    
    FastLED.show();
    delay(1000);
    showProgramCleanUp(0);
}

void checkButtonHeld() {
  int val = digitalRead(BUTTON);
  if (val == 0) { // Button goes low when pressed
    if (!buttonHeld) {
      lastButtonStateTime = millis();
      buttonHeld = true;
    } else if (millis() - lastButtonStateTime > MODE_SWITCH) { 
      state = (state + 1) % NUM_MODES;

      // Make sure snakeys are cleared
      snakeCount = 0;
      
      showProgramState();
      buttonHeld = false;
      programChange();
    }
  } else {
    buttonHeld = false;
  }
}

// main program
void loop() {
  checkButtonHeld();
  
  if (lastPrgm != prgm) {
    showProgramCleanUp(0);
  }
  
  lastPrgm = prgm;

  if (state == 0) {
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
        showProgramRainbow(10);
        break;
      case 6:
        showProgramShiftMultiPixelRandomSnake(80, 1, 5);
        break;
      case 7:
        showProgramShiftMultiPixelRandomSnake(80, 0.5, 10);
        break;
      case 8:
        showProgramShiftMultiPixelRandomSnake(80, 4, 20);
        break;
      default:
        showProgramCleanUp(50); // clean up
        break;
    }

    //Reset button state
    buttonState = false;
  } else if (state == 1 || state == 2) {
    if (buttonState) {
      addSnake();

      // Only does anything for state 2
      nextSnake = random(50, 400);
    }

    if (state == 2) {
      if (nextSnake <= 0) {
        addSnake();
        nextSnake = random(50, 400);
      }

      nextSnake--;
    }

    //Reset button state (after we have checked it)
    buttonState = false;

    updateSnakes();
    updateSign();
    FastLED.show();
    //delay(0);
  }
}

void programChange() {
  long timeSince = millis() - lastRead;
  if(!buttonState && (timeSince > BUTTON_DELAY)) {
    lastRead = millis();

    if (state == 0) {
      prgm = (prgm + 1) % NUM_PROGRAMS;
    } else {
      prgm = -1;
    }
    
    buttonState = true;
 }
}
