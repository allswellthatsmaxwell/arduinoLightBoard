#include <FastLED.h>

#define DIM 11

#define LED_PIN     6      // Pin connected to the data input of the strip
#define NUM_LEDS    DIM*DIM     // Number of LEDs in the strip (adjust as needed)
#define LED_TYPE    WS2812 // Try different types here if WS2812 doesn't work
#define COLOR_ORDER RGB    // Common color order for many addressable LEDs


CRGB leds[NUM_LEDS];

const unsigned long INITIAL_HOURS = 1;
const unsigned long INITIAL_MINUTES = 40;
String receivedData;
String currentTime;
String letters = "ITLISASTIMECDRETRAUQCATWENTYFIVEXOTFNETBFLAHPASTERUNINEEERHTXISENOFOURFIVETWONEVELETHGIESEVENTWELVEKCOLCOESNETPAMPIWINPMS";

const char* HOUR_WORDS[] = {"ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE", "TEN", "ELEVEN", "TWELVE"};


const int MAX_SIZE = 10;
String targetWordsMinutes[MAX_SIZE];
int targetWordsMinutesLength = 0;

String targetWordsHours[MAX_SIZE];
int targetWordsHoursLength = 0;

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
  } else if (currentMinutes >= 33 && currentMinutes <= 37) {
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
  } else if (currentMinutes >= 53) {
    targetWordsMinutes[targetWordsMinutesLength++] = "FIVE";
    targetWordsMinutes[targetWordsMinutesLength++] = "TO";
  } else {
    targetWordsMinutes[targetWordsMinutesLength++] = "OCLOCK";
  }

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
  getTime(millis(), currentHours, currentMinutes);

  setMinutesWords(currentMinutes);
  setHoursWords(currentHours);
}


void lightMinutesWord(String word) {
  int idx = letters.indexOf(word);
  if (idx != -1) {
    for (int i = idx; i < idx + word.length(); i++) {
      leds[i] = CRGB::Purple;
    }
  }
}


void lightHoursWord(String word) {
  int idx = letters.lastIndexOf(word);
  if (idx != -1) {
    for (int i = idx; i < idx + word.length(); i++) {
      leds[i] = CRGB::Purple;
    }
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

  hours = (totalMinutes / 60) % 24;
  minutes = totalMinutes % 60;
}


void loop() {
  targetWordsMinutes[targetWordsMinutesLength++] = "IT";
  targetWordsMinutes[targetWordsMinutesLength++] = "IS";
  Serial.println("loop");
  // String targetWords[] = {"TW", "EN", "TY", "TIME", "IS"};
  setTargetWordsBasedOnTime();
  // String targetWords[] = {"TWENTY", "QUARTER"};
  for (int i = 0; i < targetWordsMinutesLength; i++) {
    lightMinutesWord(targetWordsMinutes[i]);
    lightMinutesWord(reverseString(targetWordsMinutes[i]));
  }
  for (int i = 0; i < targetWordsHoursLength; i++) {
    lightHoursWord(targetWordsHours[i]);
    lightHoursWord(reverseString(targetWordsHours[i]));
  }
  targetWordsMinutesLength = 0;
  targetWordsHoursLength = 0;
  FastLED.show();
}