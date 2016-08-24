#include <Adafruit_NeoPixel.h>
#include "WS2812_Definitions.h"
#include "led_dress.h"

#define CAMPLIGHTS

#ifdef CAMPLIGHTS
#define PIN_A 8
#define LED_COUNT 300
#define GROUP_COUNT 3
#define CHYRON_WIDTH 51
#define COLOR_DROPOFF 8
#define SPEED 40
#endif


unsigned long COLORS[] = {
  NAVY, DARKBLUE, MEDIUMBLUE, BLUE, DARKGREEN, GREEN, TEAL, DARKCYAN, 
  DEEPSKYBLUE, DARKTURQUOISE, MEDIUMSPRINGGREEN, LIME, SPRINGGREEN, 
  AQUA, CYAN, MIDNIGHTBLUE, DODGERBLUE, LIGHTSEAGREEN, FORESTGREEN, 
  SEAGREEN, DARKSLATEGRAY, LIMEGREEN, MEDIUMSEAGREEN, TURQUOISE, 
  ROYALBLUE, STEELBLUE, DARKSLATEBLUE, MEDIUMTURQUOISE, INDIGO, 
  DARKOLIVEGREEN, CADETBLUE, CORNFLOWERBLUE, MEDIUMAQUAMARINE, DIMGRAY, 
  SLATEBLUE, OLIVEDRAB, SLATEGRAY, LIGHTSLATEGRAY, MEDIUMSLATEBLUE, 
  LAWNGREEN, CHARTREUSE, AQUAMARINE, MAROON, PURPLE, OLIVE, GRAY, 
  SKYBLUE, LIGHTSKYBLUE, BLUEVIOLET, DARKRED, DARKMAGENTA, SADDLEBROWN, 
  DARKSEAGREEN, LIGHTGREEN, MEDIUMPURPLE, DARKVIOLET, PALEGREEN, 
  DARKORCHID, YELLOWGREEN, SIENNA, BROWN, DARKGRAY, LIGHTBLUE, 
  GREENYELLOW, PALETURQUOISE, LIGHTSTEELBLUE, POWDERBLUE, FIREBRICK, 
  DARKGOLDENROD, MEDIUMORCHID, ROSYBROWN, DARKKHAKI, SILVER, 
  MEDIUMVIOLETRED, INDIANRED, PERU, CHOCOLATE, TAN, LIGHTGRAY, 
  THISTLE, ORCHID, GOLDENROD, PALEVIOLETRED, CRIMSON, GAINSBORO, PLUM, 
  BURLYWOOD, LIGHTCYAN, LAVENDER, DARKSALMON, VIOLET, PALEGOLDENROD, 
  LIGHTCORAL, KHAKI, ALICEBLUE, HONEYDEW, AZURE, SANDYBROWN, WHEAT, 
  BEIGE, WHITESMOKE, MINTCREAM, GHOSTWHITE, SALMON, ANTIQUEWHITE, 
  LINEN, LIGHTGOLDENRODYELLOW, OLDLACE, RED, FUCHSIA, MAGENTA, 
  DEEPPINK, ORANGERED, TOMATO, HOTPINK, CORAL, DARKORANGE, LIGHTSALMON, 
  ORANGE, LIGHTPINK, PINK, GOLD, PEACHPUFF, NAVAJOWHITE, MOCCASIN, 
  BISQUE, MISTYROSE, BLANCHEDALMOND, PAPAYAWHIP, LAVENDERBLUSH, SEASHELL, 
  CORNSILK, LEMONCHIFFON, FLORALWHITE, SNOW, YELLOW, LIGHTYELLOW, IVORY
};

Adafruit_NeoPixel led_a = Adafruit_NeoPixel(LED_COUNT, PIN_A, NEO_GRB + NEO_KHZ800);

void setup() {
  // To ensure that the first random color in `loop` is never the same.
  randomSeed(analogRead(0));

  led_a.begin();

  clearLEDs(true);
}

void loop() {
  uint32_t color = randomColor();
  color = randomColor();
  int speed = random(SPEED, SPEED*1.5);
  int i = 2;
  while (i--) {
    raindrop(color, speed);
  }
}

void raindrop(unsigned long color, byte wait) {
  // weight determines how much lighter the outer "eye" colors are
  const double weight = COLOR_DROPOFF;  
  // It'll be easier to decrement each of these colors individually
  // so we'll split them out of the 24-bit color value
  byte red = (color & 0xFF0000) >> 16;
  byte green = (color & 0x00FF00) >> 8;
  byte blue = (color & 0x0000FF);

  // Start at closest LED, and move to the outside
  for (int i=0; i<=LED_COUNT-1; i++) {
    // clearLEDs(false);
    for (int g=0; g<GROUP_COUNT; g++) {
      int p = ((LED_COUNT / GROUP_COUNT) * g + i) % LED_COUNT;
      led_a.setPixelColor(p, red, green, blue);
    }
    // Now set two eyes to each side to get progressively dimmer
    for (int j=1; j<CHYRON_WIDTH; j++) {
      byte redWJ = red/ceil(j/weight);
      byte greenWJ = green/ceil(j/weight);
      byte blueWJ = blue/ceil(j/weight);
      if (i-j >= 0) {
        for (int g=0; g<GROUP_COUNT; g++) {
          int p = ((LED_COUNT / GROUP_COUNT) * g + (i-j)) % LED_COUNT;
          led_a.setPixelColor(p, redWJ, greenWJ, blueWJ);
        }
      }
      if (i-j <= LED_COUNT) {
        for (int g=0; g<GROUP_COUNT; g++) {
          int p = ((LED_COUNT / GROUP_COUNT) * g + (i+j)) % LED_COUNT;
          led_a.setPixelColor(p, redWJ, greenWJ, blueWJ);
        }
      }
    }
    led_a.show();
    delay(wait);
  }
  
  return; 

  // Now we go back to where we came. Do the same thing.
  for (int i=LED_COUNT-CHYRON_WIDTH; i>=1; i--) {
    clearLEDs(false);
    for (int g=0; g<GROUP_COUNT; g++) {
      int p = ((LED_COUNT / GROUP_COUNT) * g + i) % LED_COUNT;
      led_a.setPixelColor(p, red, green, blue);
    }
    
    // Now set two eyes to each side to get progressively dimmer
    for (int j=1; j<CHYRON_WIDTH; j++)
    {
      byte redWJ = red/(weight*j);
      byte greenWJ = green/(weight*j);
      byte blueWJ = blue/(weight*j);

      if (i-j >= 0) {
        for (int g=0; g<GROUP_COUNT; g++) {
          int p = ((LED_COUNT / GROUP_COUNT) * g + (i-j)) % LED_COUNT;
          led_a.setPixelColor(p, redWJ, greenWJ, blueWJ);
        }
      }
      if (i-j <= LED_COUNT) {
        for (int g=0; g<GROUP_COUNT; g++) {
          int p = ((LED_COUNT / GROUP_COUNT) * g + (i+j)) % LED_COUNT;
          led_a.setPixelColor(p, redWJ, greenWJ, blueWJ);
        }
      }
    }
    led_a.show();
    delay(wait);
  }
}

// Sets all LEDs to off, but DOES NOT update the display;
// call leds.show() to actually turn them off after this.
void clearLEDs(bool immediate)
{
  for (int i=0; i<LED_COUNT; i++)
  {
    led_a.setPixelColor(i, 0);
  }

  if (immediate) {
    led_a.show();
  }
}

unsigned long randomColor() {
  return COLORS[random(sizeof(COLORS)/sizeof(unsigned long))];
}
