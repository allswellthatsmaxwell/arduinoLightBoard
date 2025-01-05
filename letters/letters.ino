#include <FastLED.h>

#define DIM 11

#define LED_PIN     6      // Pin connected to the data input of the strip
#define NUM_LEDS    DIM*DIM     // Number of LEDs in the strip (adjust as needed)
#define LED_TYPE    WS2812 // Try different types here if WS2812 doesn't work
#define COLOR_ORDER RGB    // Common color order for many addressable LEDs


CRGB leds[NUM_LEDS];

const unsigned long INITIAL_HOURS = 19;
const unsigned long INITIAL_MINUTES = 30;
String receivedData;
String currentTime;
String letters = "ITLISASTIMECDRETRAUQCATWENTYFIVEXOTFNETBFLAHPASTERUNINEEERHTXISENOFOURFIVETWONEVELETHGIESEVENTWELVEKCOLCOESNETPAMPIWINPMS";

const char* HOUR_WORDS[] = {"ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE", "TEN", "ELEVEN", "TWELVE"};


const int MAX_SIZE = 10;
String targetWords[MAX_SIZE];
int currentLength = 0;

void setup() {  
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();

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

void setMinutesWords(int currentMinutes) {
  if (currentMinutes >= 23 && currentMinutes <= 37) {
    targetWords[currentLength++] = "HALF";
    targetWords[currentLength++] = "PAST";
  } else if (currentMinutes >= 8 && currentMinutes <= 22) {
    targetWords[currentLength++] = "QUARTER";
    targetWords[currentLength++] = "PAST";
  } else if (currentMinutes >= 37 && currentMinutes <= 52) {
    targetWords[currentLength++] = "QUARTER";
    targetWords[currentLength++] = "TO";
  }
}

void setHoursWords(int currentHours) {  
  if (currentHours == 0) {  // midnight
      currentHours = 12;
  }
  targetWords[currentLength++] = HOUR_WORDS[currentHours - 1];
}

void setTargetWordsBasedOnTime() {
  int currentHours, currentMinutes;
  getTime(millis(), currentHours, currentMinutes);

  setMinutesWords(currentMinutes);
  // setHoursWords(currentHours);

}

void lightWord(String word) {
  int idx = letters.indexOf(word);
  Serial.println(idx);
  while (idx != -1) {
    for (int i = idx; i < idx + word.length(); i++) {
      leds[i] = CRGB::Purple;
    }    
    idx = letters.indexOf(word, idx + word.length());
  }
}

String reverseString(String str) {
  String reversed = "";
  for (int i = str.length() - 1; i >= 0; i--) {
    reversed += str.charAt(i);
  }
  return reversed;
}

void getTime(unsigned long milliseconds, int& hours, int& minutes) {  
  unsigned long totalMinutes = (milliseconds / 1000) / 60;
  
  totalMinutes += (INITIAL_HOURS * 60) + INITIAL_MINUTES;

  hours = (totalMinutes / 60) % 24;    // Use modulo 24 to keep in 24-hour format
  minutes = totalMinutes % 60;
}


void loop() {
  Serial.println("loop");
  // String targetWords[] = {"TW", "EN", "TY", "TIME", "IS"};
  setTargetWordsBasedOnTime();
  // String targetWords[] = {"TWENTY", "QUARTER"};
  int size = currentLength;
  for (int i = 0; i < size; i++) {
    lightWord(targetWords[i]);
    lightWord(reverseString(targetWords[i]));
  }
  FastLED.show();
}