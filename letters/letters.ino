#include <FastLED.h>
#include <Wire.h>
#include <RTClib.h>

#define DIM 11

#define LED_PIN     6        // Pin connected to the data input of the strip
#define NUM_LEDS    DIM*DIM  // Number of LEDs in the strip (adjust as needed)
#define LED_TYPE    WS2812   // Try different types here if WS2812 doesn't work
#define COLOR_ORDER RGB

RTC_DS3231 rtc;

CRGB leds[NUM_LEDS];
CRGB newLeds[NUM_LEDS];

const unsigned long INITIAL_HOURS = 22;
const unsigned long INITIAL_MINUTES = 3;
unsigned int MINUTE_THAT_TO_STATEMENTS_BEGIN = 33;
String currentTime;
const char letters[] PROGMEM = "ITLISASTIMECDRETRAUQCATWENTYFIVEXOTFNETBFLAHPASTERUNINEEERHTXISENOFOURFIVETWONEVELETHGIESEVENTWELVEKCOLCOESNETPAMPIWINPMS";
char reversedBuffer[20]; 

const char* HOUR_WORDS[] = {"ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE", "TEN", "ELEVEN", "TWELVE"};


const int MAX_SIZE = 10;
const char* targetWordsMinutes[MAX_SIZE];
int targetWordsMinutesLength = 0;

const char* targetWordsHours[MAX_SIZE];
int targetWordsHoursLength = 0;

void setup() {
  Serial.begin(9600);
  // Serial.println("Start");

  Wire.begin();  // Add this line
  delay(1000);   // Add a delay to let I2C stabilize
  
  Serial.println("Attempting to find RTC...");
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.println("Check your connections!");
    while (1);
  }
  Serial.println("Found RTC");

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();
}

String readValue(String key, String data) {
  int index = data.indexOf(key);
  if (index != -1) {
    int endIdx = data.indexOf(";", index);
    return data.substring(index + key.length(), endIdx);
  }
}

void setMinutesWords(int currentMinutes) {
  if (currentMinutes >= 28 && currentMinutes <= 32) {
    targetWordsMinutes[targetWordsMinutesLength++] = "HALF";
    targetWordsMinutes[targetWordsMinutesLength++] = "PAST";
  } else if (currentMinutes >= 23 && currentMinutes <= 27) {
    targetWordsMinutes[targetWordsMinutesLength++] = "TWENTYFIVE";
    targetWordsMinutes[targetWordsMinutesLength++] = "PAST";
  } else if (currentMinutes >= 18 && currentMinutes <= 22) {
    targetWordsMinutes[targetWordsMinutesLength++] = "TWENTY";
    targetWordsMinutes[targetWordsMinutesLength++] = "PAST";
  } else if (currentMinutes >= 13 && currentMinutes <= 17) {
    targetWordsMinutes[targetWordsMinutesLength++] = "QUARTER";
    targetWordsMinutes[targetWordsMinutesLength++] = "PAST";
  } else if (currentMinutes >= 8 && currentMinutes <= 12) {
    targetWordsMinutes[targetWordsMinutesLength++] = "TEN";
    targetWordsMinutes[targetWordsMinutesLength++] = "PAST";
  } else if (currentMinutes >= 3 && currentMinutes <= 7) {
    targetWordsMinutes[targetWordsMinutesLength++] = "FIVE";
    targetWordsMinutes[targetWordsMinutesLength++] = "PAST";
  } else if (currentMinutes >= MINUTE_THAT_TO_STATEMENTS_BEGIN && currentMinutes <= 37) {
    targetWordsMinutes[targetWordsMinutesLength++] = "TWENTYFIVE";
    targetWordsMinutes[targetWordsMinutesLength++] = "TO";
  } else if (currentMinutes >= 38 && currentMinutes <= 42) {
    targetWordsMinutes[targetWordsMinutesLength++] = "TWENTY";
    targetWordsMinutes[targetWordsMinutesLength++] = "TO";
  } else if (currentMinutes >= 43 && currentMinutes <= 47) {
    targetWordsMinutes[targetWordsMinutesLength++] = "QUARTER";
    targetWordsMinutes[targetWordsMinutesLength++] = "TO";
  } else if (currentMinutes >= 48 && currentMinutes <= 52) {
    targetWordsMinutes[targetWordsMinutesLength++] = "TEN";
    targetWordsMinutes[targetWordsMinutesLength++] = "TO";
  } else if (currentMinutes >= 53 and currentMinutes <= 57) {
    targetWordsMinutes[targetWordsMinutesLength++] = "FIVE";
    targetWordsMinutes[targetWordsMinutesLength++] = "TO";
  } else {
    targetWordsMinutes[targetWordsMinutesLength++] = "OCLOCK";
  }
}

bool thereWasAnUpdate() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (leds[i] != newLeds[i]) {
      return true;
    }
  }
  return false;
}

void setHoursWords(int currentHours) {  
  if (currentHours == 0) {  // midnight
      currentHours = 12;
      targetWordsHours[targetWordsHoursLength++] = "AM";
  } else if (currentHours < 12) {
    targetWordsHours[targetWordsHoursLength++] = "AM";
  } else {
    targetWordsHours[targetWordsHoursLength++] = "PM";
  }

  targetWordsHours[targetWordsHoursLength++] = HOUR_WORDS[(currentHours == 12 ? 12 : currentHours % 12) - 1];
}


void setTargetWordsBasedOnTime() {
  int currentHours, currentMinutes;
  getTime(currentHours, currentMinutes);

  setMinutesWords(currentMinutes);
  setHoursWords(currentHours);  
}

int findInProgmem(const char* word) {
  char letter;
  int wordLen = strlen(word);
  
  // Search through possible starting positions
  for(int i = 0; i < sizeof(letters)-wordLen; i++) {
    // Check if word matches starting at position i
    bool match = true;
    for(int j = 0; j < wordLen; j++) {
      letter = pgm_read_byte(&letters[i+j]);
      if(letter != word[j]) {
        match = false;
        break;
      }
    }
    if(match) {
      // Check if it crosses a row boundary
      if(i/DIM != (i+wordLen-1)/DIM) {
        continue; // Skip this match, look for next
      }
      return i;
    }
  }
  return -1;
}

int lightWord(const char* word) {
  if (strlen(word) == 0) {
    // Serial.println("Word of length 0; skipping.");
    return -1;
  }
  int idx = findInProgmem(word);
  if (idx / DIM != (idx + strlen(word) - 1) / DIM) {
    // match crosses rows; don't count it
    Serial.println("Crosses rows, looking for next match");

    // idx = letters.indexOf(word, idx + 1);
    idx = findInProgmem(word);
  }  
  if (idx != -1) {      
    for (int i = idx; i < idx + strlen(word); i++) {
      newLeds[i] = CRGB::Orange;
    }
  }
  return idx;
}


const char* reverseString(const char* str) {
  int len = strlen(str);
  for (int i = 0; i < len; i++) {
    reversedBuffer[i] = str[len - 1 - i];
  }
  reversedBuffer[len] = '\0';  // Null terminate the string
  return reversedBuffer;
}

void getTime(int& hours, int& minutes) {
  static DateTime now;  // Make it static to reuse the memory
  now = rtc.now();     // Just update the values
  
  // Get the values immediately
  hours = (int)now.hour();    // Explicit cast
  minutes = (int)now.minute(); // Explicit cast

  if (minutes >= MINUTE_THAT_TO_STATEMENTS_BEGIN) {
    hours = hours == 23 ? 0 : hours + 1;      
  }
}


void clearStatusBetweenLoops() {
  targetWordsMinutesLength = 0;
  targetWordsHoursLength = 0;

  for (int i = 0; i < NUM_LEDS; i++) {
    newLeds[i] = CRGB::Black;
  }
}

void loop() {
  clearStatusBetweenLoops();
  targetWordsMinutes[targetWordsMinutesLength++] = "IT";
  targetWordsMinutes[targetWordsMinutesLength++] = "IS";
  
  setTargetWordsBasedOnTime();
  
  for (int i = 0; i < targetWordsMinutesLength; i++) {
    if (lightWord(targetWordsMinutes[i]) == -1) {    
      lightWord(reverseString(targetWordsMinutes[i]));
    }
  }

  for (int i = 0; i < targetWordsHoursLength; i++) {    
    if (lightWord(targetWordsHours[i]) == -1) {
      lightWord(reverseString(targetWordsHours[i]));
    }    
  }
  
  if (thereWasAnUpdate()) {
    for(int i = 0; i < NUM_LEDS; i++) {
      leds[i] = newLeds[i];
    }
  }
  FastLED.show();
  delay(1000);
}