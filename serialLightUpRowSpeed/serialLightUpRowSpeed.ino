#include <FastLED.h>

#define DIM 11

#define LED_PIN     6      // Pin connected to the data input of the strip
#define NUM_LEDS    DIM*DIM     // Number of LEDs in the strip (adjust as needed)
#define LED_TYPE    WS2812 // Try different types here if WS2812 doesn't work
#define COLOR_ORDER RGB    // Common color order for many addressable LEDs

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
}

int getDelayTime() {
  bool goFast = random(0, 2) == 1;
  return goFast ? 40 : 200;
}

void loop() {
  // Test color effect: lighting up LEDs with red color
  int delayTime;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Red;
    FastLED.show();
    if (i % DIM == 0) {
      delayTime = getDelayTime();
    }
    delay(delayTime);
    leds[i] = CRGB::Purple;  // Turn off the LED
  }
}