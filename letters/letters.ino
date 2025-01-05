#include <FastLED.h>

#define DIM 11

#define LED_PIN     6      // Pin connected to the data input of the strip
#define NUM_LEDS    DIM*DIM     // Number of LEDs in the strip (adjust as needed)
#define LED_TYPE    WS2812 // Try different types here if WS2812 doesn't work
#define COLOR_ORDER RGB    // Common color order for many addressable LEDs

CRGB leds[NUM_LEDS];

String receivedData;
String currentTime;
String letters = "ITLISASTIMECDRETRAUQCATWENTYFIVEXOTFNETBFLAHPASTERUNINEEERHTXISENOFOURFIVETWONEVELETHGIESEVENTWELVEKCOLCOESNETPAMPIWINPMS";

void setup() {  
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();
  // for (int i = 0; i < NUM_LEDS; i++) {
  //   leds[i] = CRGB::Black;
  // }

  // Serial.begin(9600);
  // Serial.println("Waiting for data........");

  // // Wait for data to arrive
  // while (Serial.available() == 0);

  // receivedData = Serial.readString();
  // Serial.print("receivedData: ");
  // Serial.println(receivedData);

  // Parse the data
  // currentTime = readValue("TIME=", receivedData);
  // letters = readValue("LETTERS=", receivedData);
  // Serial.println(currentTime);
  // Serial.println(letters);
}

String readValue(String key, String data) {
  int index = data.indexOf(key);
  if (index != -1) {
    int endIdx = data.indexOf(";", index);
    return data.substring(index + key.length(), endIdx);
  }
}

void lightWord(String word) {
  // for (int i = 0; i < NUM_LEDS - word.length(); i++) {
  //   if (letters.substring(i, word.length()) == word) {
  //     for (int j = i; j < i + word.length(); j++) {
  //       leds[j] = CRGB::Purple;
  //     }
  //   }
  // }
  int idx = letters.indexOf(word);
  Serial.println(idx);
  int endIdx = idx + word.length();
  if (idx != -1) {
    for (int i = idx; i < endIdx; i++) {
      leds[i] = CRGB::Purple;
    }
    // idx = letters.indexOf(word, endIdx);
    // Serial.println(idx);
  }
} 


void loop() {
  Serial.println("loop");
  // String targetWords[] = {"TW", "EN", "TY", "TIME", "IS"};
  String targetWords[] = {"IT", "IS"};
  int size = sizeof(targetWords) / sizeof(targetWords[0]);
  for (int i = 0; i < size; i++) {
    lightWord(targetWords[i]);
  }
  FastLED.show();
}